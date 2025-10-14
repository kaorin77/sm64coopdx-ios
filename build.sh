codesign --remove-signature "${BUILT_PRODUCTS_DIR}/${FULL_PRODUCT_NAME}"

#rm -rf $(BUILT_PRODUCTS_DIR)

cat > ./levels/level_rules.mk << 'EOF'
#define STUB_LEVEL(_0, _1, _2, _3, _4, _5, _6, _7, _8)
#define DEFINE_LEVEL(_0, _1, _2, folder, texturebin, _5, _6, _7, _8, _9, _10) $(eval $(call level_rules,folder,texturebin))

#include "levels/level_defines.h"

#undef STUB_LEVEL
#undef DEFINE_LEVEL
EOF

#touch "levels/level_rules.mk"
#echo "#define STUB_LEVEL(_0, _1, _2, _3, _4, _5, _6, _7, _8) \
##define DEFINE_LEVEL(_0, _1, _2, folder, texturebin, _5, _6, _7, _8, _9, _10) $(eval \ $(call level_rules,folder,texturebin)) \
#\
##include \"levels/level_defines.h\" \
#\
##undef STUB_LEVEL \
##undef DEFINE_LEVEL \
#" > levels/level_rules.mk

case ${PLATFORM_NAME} in
    iphoneos)
        export CC="clang -arch arm64 --target=arm64-apple-ios -mios-version-min=12.2 -fobjc-weak  -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk -DUSE_GLES -DAAPI_SDL2 -DWAPI_SDL2 -DTARGET_OS_IOS"
        export CXX="clang++ -arch arm64 -mios-version-min=12.2 -fobjc-weak -std=c++11 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk -DUSE_GLES -DAAPI_SDL2 -DWAPI_SDL2 -DTARGET_OS_IOS"
        export TARGET_OS_IOS=1
        export TARGET_OS_IOS_SIM=0
        rsync -aP --exclude="Info.plist" "sm64ios/sm64ios/" "${BUILT_PRODUCTS_DIR}/${FULL_PRODUCT_NAME}"
        ;;
    iphonesimulator)
        export CC="clang -arch $(eval "uname -m") -mios-simulator-version-min=12.2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk -DUSE_GLES -DAAPI_SDL2 -DWAPI_SDL2 -fobjc-weak -DTARGET_OS_IOS"
        export CXX="clang++ -arch $(eval "uname -m") -mios-simulator-version-min=12.2 -fobjc-weak -std=c++11 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk -DUSE_GLES -DAAPI_SDL2 -DWAPI_SDL2 -DTARGET_OS_IOS"
        export TARGET_OS_IOS=1
        export TARGET_OS_IOS_SIM=1
        rsync -aP --exclude="Info.plist" "sm64ios/sm64ios/" "${BUILT_PRODUCTS_DIR}/${FULL_PRODUCT_NAME}"
        ;;
esac

export IOSINCLUDE="${SDL2_INCLUDE_DIR}/"
export IOSLIBS="${BUILT_PRODUCTS_DIR}/"

gmake -j4 TARGET_IOS=1 TARGET_IOS_SIM=$TARGET_OS_IOS_SIM

GAMEDIR_CP_SRC="build/us_pc/sm64coopdx"
if [[ $TARGET_OS_IOS_SIM -eq 1 ]]; then
    GAMEDIR_CP_SRC="build_sim/us_pc/sm64coopdx"
fi

if [[ $EXTERNAL_DATA -eq 1 ]]; then
    if [[ $TARGET_IOS_SIM -eq 1 ]]; then
        GAMEDIR_CP_SRC="build_sim/us_pc/."
    else
        GAMEDIR_CP_SRC="build/us_pc/."
    fi
fi

cp -R $GAMEDIR_CP_SRC "${BUILT_PRODUCTS_DIR}/${FULL_PRODUCT_NAME}/"
cp -R "build/us_pc/Base.lproj" "${BUILT_PRODUCTS_DIR}/${FULL_PRODUCT_NAME}/"
[[ -e "build/us_pc/dynos" ]] && cp -R "build/us_pc/dynos" "${BUILT_PRODUCTS_DIR}/${FULL_PRODUCT_NAME}/"
[[ -e "build/us_pc/lang" ]] && cp -R "build/us_pc/lang" "${BUILT_PRODUCTS_DIR}/${FULL_PRODUCT_NAME}/"
rsync -aP --exclude="README.md" "ios/" "${BUILT_PRODUCTS_DIR}/${FULL_PRODUCT_NAME}"

codesign --force --deep --sign "${CODE_SIGN_IDENTITY}" "${BUILT_PRODUCTS_DIR}/${FULL_PRODUCT_NAME}"
