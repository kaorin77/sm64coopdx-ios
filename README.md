# sm64coopdx-ios
![sm64coopdx Logo](textures/segment2/custom_coopdx_logo.rgba32.png)

## Description

A port of [sm64coopdx](https://github.com/coop-deluxe/sm64coopdx) to iOS

## Building
### Prerequisites
 - macOS 12+
 - [Homebrew](https://brew.sh/)
### Installing requirements
1. Download Xcode 14.2 if you have macOS <=13 or Xcode 16.2 if you have macOS >=14 (they're both tested)

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
2. `cd` into `tools` directory and run `make`
3. Open `sm64ios.xcworkspace` (not `sm64coopdx_ios.xcodeproj`)
4. Select sm64coopdx_ios in Project navigator and under "Signing & Capabilities" choose your team and change bundle identifier
5. Set your build target to `sm64coopdx -> <Your iOS build target>` (simulators are supported)
6. Press `Command + B` or press Product -> Build


## TODOs
 - [X] Compile
 - [X] Fix touch controls
 - [x] Compile libjuice for iOS
 - [x] Try to understand the code
 - [x] Compile coopnet for iOS
 - [x] Replace touch controls with better implementation from android port
 - [ ] Remove simulatedStartFlag
 - [X] Add app icon
 - [ ] Add gamepad support
 - [x] Delete support for tvOS
 - [x] Write building tutorial

## Thanks to...
 - [ckosmic/sm64ex-ios](https://github.com/ckosmic/sm64ex-ios) for iOS port
 - [ManIsCat2/sm64coopdx](https://github.com/ManIsCat2/sm64coopdx) for touch controls
 - [coop-deluxe/sm64coopdx](https://github.com/coop-deluxe/sm64coopdx) for the game!

..and Qwen for writing most of the Objective C code
