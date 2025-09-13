#import <UIKit/UIKit.h>
#include <SDL2/SDL.h>

#include "pc/cliopts.h"
#include "pc/fs.h"
#include "pc/configfile.h"
#include "pc/network.h"
#include "pc/gfx/gfx_screen.h"
#include "pc/pc_main.h"
#include "pc/debuglog.h"
#include "audio/audio_api.h"
#include "audio/audio_null.h"
#include "audio/audio_sdl.h"
#include "src/game/game_init.h"
#include "src/game/level_update.h"
#include "src/pc/djui/djui.h"
#include "src/pc/djui/djui_unicode.h"
#include "src/pc/djui/djui_console.h"
#include "src/pc/djui/djui_lua_profiler.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    int argc = 1;
    char *argv[] = { "app", NULL };

    // CLI
    if (!parse_cli_opts(argc, argv)) {
        return NO;
    }

#if defined(RAPI_DUMMY) || defined(WAPI_DUMMY)
    gCLIOpts.headless = true;
#endif

    // FS
    fs_init(gCLIOpts.savePath[0] ? gCLIOpts.savePath : sys_user_path());

    // Config
    configfile_load();
    legacy_folder_handler();

    // Графон
    if (!gGfxInited) {
        gfx_init(&WAPI, &RAPI, TITLE);
        WAPI.set_keyboard_callbacks(keyboard_on_key_down, keyboard_on_key_up,
                                    keyboard_on_all_keys_up,
                                    keyboard_on_text_input,
                                    keyboard_on_text_editing);
        WAPI.set_scroll_callback(mouse_on_scroll);
    }

    // ROM setup
    if (!main_rom_handler()) {
        NSLog(@"ERROR: no valid rom");
        return NO;
    }

    // Init game
    main_game_init(NULL);
    thread5_game_loop(NULL);

    // Аудио
    if (gCLIOpts.headless) {
        audio_api = &audio_null;
    }
#if defined(AAPI_SDL1) || defined(AAPI_SDL2)
    if (!audio_api && audio_sdl.init()) {
        audio_api = &audio_sdl;
    }
#endif
    if (!audio_api) {
        audio_api = &audio_null;
    }

    // djui
    djui_init();
    djui_unicode_init();
    djui_init_late();
    djui_console_message_dequeue();
    show_update_popup();

    // Сеть (пока в NT_NONE, хочешь — раскинешь под клиент/сервер)
    network_init(NT_NONE, false);

    // Запуск главного цикла в фоне
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{
        while (true) {
            debug_context_reset();
            CTX_BEGIN(CTX_TOTAL);
            WAPI.main_loop(produce_one_frame);
#ifdef DISCORD_SDK
            discord_update();
#endif
            mumble_update();
#ifdef DEBUG
            fflush(stdout);
            fflush(stderr);
#endif
            CTX_END(CTX_TOTAL);
#ifdef DEVELOPMENT
            djui_ctx_display_update();
#endif
            djui_lua_profiler_update();
        }
    });

    return YES;
}

@end

int main(int argc, char *argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
