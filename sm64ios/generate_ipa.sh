#!/bin/sh

rm ../build/us_pc/sm64coopdx.ipa

mkdir Payload
mv ${BUILT_PRODUCTS_DIR}/sm64ios.app ./Payload/sm64ios.app
mv ./Payload/sm64ios.app/sm64coopdx ./Payload/sm64ios.app/sm64ios

zip -r "sm64coopdx.ipa" Payload
mv sm64coopdx.ipa ../build/us_pc/sm64coopdx.ipa

rm -rf Payload
rm -rf ${BUILT_PRODUCTS_DIR}
