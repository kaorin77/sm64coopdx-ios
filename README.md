# sm64coopdx-ios
![sm64coopdx Logo](textures/segment2/custom_coopdx_logo.rgba32.png)

## Description

A port of [sm64coopdx](https://github.com/coop-deluxe/sm64coopdx) to iOS

## Building
### Prerequisites
 - macOS 12.5+
 - [Homebrew](brew.sh)
### Installing requirements
1. Download Xcode 14.2 [from official Apple website](https://developer.apple.com/services-account/download?path=/Developer_Tools/Xcode_14.2/Xcode_14.2.xip)
or from [xcodereleases.com](https://xcodereleases.com/) (Xcode 14.2+ is not tested and not supported, Xcode 16.2 is tested and does not work)

2. Install required packages using Homebrew:
```bash
brew install make gcc@12 mingw-w64 imagemagick
```

### Cloning repo
1. Create a working directory to put project files: `mkdir coopdxios && cd coopdxios`
2. Clone repository and SDL2:
```bash
git clone https://github.com/artemius466/sm64coopdx-ios.git && \
git clone -b SDL2 https://github.com/libsdl-org/SDL.git SDL2 && \
mkdir include && \
cp -a SDL2/include include/ && \
mv include/include include/SDL2
```

### Building
1. If you want an app icon, then run `./appicon.sh`
2. Open `sm64ios.xcworkspace` (not `sm64coopdx_ios.xcodeproj`)
3. Select sm64coopdx_ios in Project navigator and under "Signing & Capabilities" choose your team and change bundle identifier
4. Set your build target to `sm64coopdx -> <Your iOS build target>` **Note:** simulators are not working right now
5. Press `Command + B` or press Product -> Build
6. IPA file will be placed in `build/us_pc` folder


## TODOs
 - [X] Compile
 - [X] Fix touch controls
 - [x] Compile libjuice for iOS
 - [x] Try to understand the code
 - [x] Compile coopnet for iOS
 - [x] Replace touch controls with better implementation from android port
 - [ ] Remove simulatedStartFlag
 - [ ] Add gamepad support
 - [x] Delete support for tvOS
 - [x] Write building tutorial
 - [ ] Make first release
 - [ ] Add ability to change touch control buttons size and position

## Thanks to...
 - [ckosmic/sm64ex-ios](https://github.com/ckosmic/sm64ex-ios) for iOS port
 - [ManIsCat2/sm64coopdx](https://github.com/ManIsCat2/sm64coopdx) for touch controls
 - [coop-deluxe/sm64coopdx](https://github.com/coop-deluxe/sm64coopdx) for the game!

..and Qwen for writing most of the Objective C code
