#!/bin/bash

if brew ls --versions imagemagick > /dev/null; then
    # iOS App Icons
    # convert -size 1024X1024 gradient:'#0390fc'-'#00589c' "sm64ios/sm64ios/Assets.xcassets/AppIcon.appiconset/gradient.png"
    magick convert -interpolate Nearest -filter point -resize 720X720 "res/icon_ios.png" "sm64ios/sm64ios/Assets.xcassets/AppIcon.appiconset/head.png"
    magick convert "sm64ios/sm64ios/Assets.xcassets/AppIcon.appiconset/head.png" -gravity center -extent 1024X1024 -background none -transparent white -layers Flatten "sm64ios/sm64ios/Assets.xcassets/AppIcon.appiconset/icon1024x1024.png"
    
    declare -a sizes=( 120x120 180x180 76x76 152x152 167x167 )
    for i in "${sizes[@]}"
    do
        magick convert -interpolate Nearest -filter point -resize $i "sm64ios/sm64ios/Assets.xcassets/AppIcon.appiconset/icon1024x1024.png" "sm64ios/sm64ios/Assets.xcassets/AppIcon.appiconset/icon$i.png"
    done
    
    # rm "sm64ios/sm64ios/Assets.xcassets/AppIcon.appiconset/gradient.png"
    rm "sm64ios/sm64ios/Assets.xcassets/AppIcon.appiconset/head.png"
fi
