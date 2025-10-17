#!/bin/sh

if [ "$PLATFORM_NAME" = "iphoneos" ]; then
    rm ../build/us_pc/sm64coopdx.ipa

    mkdir Payload
    mv ${BUILT_PRODUCTS_DIR}/sm64coopdx.app ./Payload/sm64coopdx.app

    zip -r "sm64coopdx.ipa" Payload
    mv sm64coopdx.ipa ../build/us_pc/sm64coopdx.ipa

    rm -rf Payload
    rm -rf ${BUILT_PRODUCTS_DIR}
fi


