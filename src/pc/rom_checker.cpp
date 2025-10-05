#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdarg>
#include <sys/stat.h>
#include <dirent.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

extern "C" {
#include "platform.h"
#include "mods/mods_utils.h" // for str_ends_with
#include "mods/mod_cache.h"  // for md5 hashing
#include "mods/mods.h"
#include "loading.h"
#include "fs/fs.h"
}

bool gRomIsValid = false;
char gRomFilename[SYS_MAX_PATH] = "";

struct VanillaMD5 {
    const char *localizationName;
    const char *md5;
};

// lookup table for vanilla sm64 roms
static struct VanillaMD5 sVanillaMD5[] = {
    // { "eu", "45676429ef6b90e65b517129b700308e" },
    // { "jp", "85d61f5525af708c9f1e84dce6dc10e9" },
    // { "sh", "2d727c3278aa232d94f2fb45aec4d303" },
    { "us", "20b854b239203baf6c961b850a4a51a2" },
    { NULL, NULL },
};

// Проверка существования файла/директории
static bool file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

// Копирование файла
static bool copy_file(const char *source, const char *dest) {
    FILE *src = fopen(source, "rb");
    if (!src) return false;
    
    FILE *dst = fopen(dest, "wb");
    if (!dst) {
        fclose(src);
        return false;
    }
    
    char buffer[4096];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, n, dst) != n) {
            fclose(src);
            fclose(dst);
            return false;
        }
    }
    
    fclose(src);
    fclose(dst);
    return true;
}

inline static void rename_tmp_folder() {
    char userPath[SYS_MAX_PATH];
    char oldPath[SYS_MAX_PATH];
    char newPath[SYS_MAX_PATH];
    
    snprintf(userPath, sizeof(userPath), "%s", fs_get_write_path(""));
    snprintf(oldPath, sizeof(oldPath), "%s%s", userPath, "tmp");
    snprintf(newPath, sizeof(newPath), "%s%s", userPath, TMP_DIRECTORY);
    
    if (file_exists(oldPath) && !file_exists(newPath)) {
#if defined(_WIN32) || defined(_WIN64)
        SetFileAttributesA(oldPath, FILE_ATTRIBUTE_HIDDEN);
#endif
        rename(oldPath, newPath);
    }
}

static bool is_rom_valid(const char *romPath) {
    u8 dataHash[16] = { 0 };
    mod_cache_md5(romPath, dataHash);

    char hashStr[33];
    for (int i = 0; i < 16; i++) {
        snprintf(&hashStr[i * 2], 3, "%02x", dataHash[i]);
    }
    hashStr[32] = '\0';

    for (VanillaMD5 *md5 = sVanillaMD5; md5->localizationName != NULL; md5++) {
        if (strcmp(md5->md5, hashStr) == 0) {
            char destPath[SYS_MAX_PATH];
            snprintf(destPath, sizeof(destPath), "%sbaserom.%s.z64",
                    fs_get_write_path(""), md5->localizationName);

            // Copy the rom to the user path
            if (strcmp(romPath, destPath) != 0 && !file_exists(destPath)) {
                if (!copy_file(romPath, destPath)) {
                    printf("Failed to copy ROM file\n");
                }
            }

            snprintf(gRomFilename, SYS_MAX_PATH, "%s", destPath);
            gRomIsValid = true;
            return true;
        }
    }

    return false;
}

inline static bool scan_path_for_rom(const char *dir) {
    DIR *d = opendir(dir);
    if (!d) return false;
    
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (entry->d_type == DT_REG) {
            const char *ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".z64") == 0) {
                char fullpath[SYS_MAX_PATH];
                snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, entry->d_name);
                if (is_rom_valid(fullpath)) {
                    closedir(d);
                    return true;
                }
            }
        }
    }
    closedir(d);
    return false;
}

extern "C" {
void legacy_folder_handler(void) {
    rename_tmp_folder();
}

bool main_rom_handler(void) {
    if (scan_path_for_rom(fs_get_write_path(""))) { return true; }
    scan_path_for_rom(sys_exe_path_dir());
    return gRomIsValid;
}

#ifdef LOADING_SCREEN_SUPPORTED
void rom_on_drop_file(const char *path) {
    static bool hasDroppedInvalidFile = false;
    if (strlen(path) > 0 && !is_rom_valid(path) && !hasDroppedInvalidFile) {
        hasDroppedInvalidFile = true;
        strcat(gCurrLoadingSegment.str, "\n\\#ffc000\\The file you last dropped was not a valid, vanilla SM64 rom.");
    }
}
#endif
}
