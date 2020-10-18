<div align="center">
  <img src=".screens/ff7.png" alt="Final Fantasy VII running on Vulkan!">
  <br><strong><small>Final Fantasy VII running on Vulkan!</small></strong>
</div>
<br><br>
<div align="center">
  <img src=".screens/ff8.png" alt="Final Fantasy VIII running on Vulkan!">
  <br><strong><small>Final Fantasy VIII running on Vulkan!</small></strong>
</div>
---

# FFNx

Next generation driver for Final Fantasy VII and Final Fantasy VIII ( with native Steam 2013 release support! )

## Introduction
Welcome to FFNx project. This is an attempt to move forward what you were used to know as [FF7_OpenGL](https://github.com/Aali132/ff7_opengl) driver, [made by Aali](http://forums.qhimm.com/index.php?topic=14922.0).

Some of the improvements that you can find on FFNx are:
- FF7/FF8: [/LARGEADDRESSAWARE](https://docs.microsoft.com/en-us/cpp/build/reference/largeaddressaware-handle-large-addresses?view=vs-2019) support!

  Up to 3.5GB of space available for mods ( this requires the [4GB Patch](https://ntcore.com/?page_id=371) in your ff7.exe ).
- FF7/FF8: High DPI support!
- FF7/FF8: Up to 16x Anisotropic support!
- FF7/FF8: Up to 16x Antialiasing support!
- FF7/FF8: Steam support! No Game converter required.
- FF7/FF8: Steam savegame preservation ( you will no more loose saves created with FFNx! )
- FF7/FF8: XInput controller support (Xbox 360 and compatible ones) with D-Pad working out-of-the-box!
- FF7/FF8: Native speedhack support!
- FF7/FF8: The game will continue to run when not in focus in Window mode ( FF7: always enabled, FF8: [disabled by default]((misc/FFNx.toml#L354)) )
- FF7: eStore support! No Game converter required.
- FF7: Vertical centering for Fields and Movies
- FF7: Fullscreen Battle scenes
- FF7: Menu cursor on the middle of words vertical alignment
- FF7: Movies will continue to play if the window game loses focus ( in window mode )
- FF7: Movies volume will respect global sound volume
- FF7: Steam sound and music volume configuration preservation ( configure at your pleasure and on the next run it will be inherited )
- FF7: Configurable background transparency in battle dialogs ( by default set to 75% )!
- FF7: SFX volume change will now apply in real-time, instead of you requiring to close and re-open the game.
- FF7: Support for animated textures ( like Aerith waterfall, light fading, etc. )
- FF7: Support for soft-reset while you're playing, like on the PSX!
- FF7: Support for battle toggle ( enable/disable at your own pleasure )
- FF7: Save everywhere! You are no more required to stay next to crystals in order to keep your progress.
- FF7: **Voice acting**! One of the first mods to make use of this will be [Echo-S](https://forum.tsunamods.com/viewtopic.php?f=65&t=9) when it will be released!
- FF7: Support for external SFX audio effects
- FF8: Enable the VRAM debug window while playing in order to see how the engine uploads textures

And, on top of all of this you will also find:
- One single .dll file to be copied
- Support for 7h 1.x/[2.x](#how-to-install-on-7h-2x)
- Support for newest video and audio codecs ( WEBM, H.265, etc.)
- VGMStream built-in library WITH LOOP support for .ogg files!
- Five different Rendering Backends:
  - OpenGL ( safe default )
  - DirectX 9 ( Tech Preview: Not Recommended )
  - DirectX 11 ( suggested alternative to OpenGL )
  - DirectX 12
  - Vulkan

## Modding
In order to enhance the modding experience, FFNx does also provide:
- Game rendering inspection through [RenderDoc](https://renderdoc.org/)!
- [DDS Texture support](https://beyondskyrim.org/tutorials/the-dds-texture-format) up to BC7 format, with PNG Texture support as fallback.
- Support for configurable external textures path using [mod_path](misc/FFNx.toml#L100)
- Support for an override layer of the data directory using [override_path](misc/FFNx.toml#L116)
- Support for MINIPSF audio files using the emulated PSX/PS2 AKAO Engine
- Support for [Hext](https://forums.qhimm.com/index.php?topic=13574.0) patching files inside of the [hext_patching_path](misc/FFNx.toml#L113)
- Debug in-game engine data through [imgui](https://github.com/ocornut/imgui) integration.

## Tech Stack
If you're curious to know it, FFNx is made with:
- C++ code base
- Latest MSVC available on [Visual Studio 2019 Community Edition](https://visualstudio.microsoft.com/vs/features/cplusplus/)
- [CMake](https://cmake.org/) ( as make files )
- [BGFX](https://github.com/bkaradzic/bgfx) ( as backend renderer )
- [BIMG](https://github.com/bkaradzic/bimg) ( for custom textures )
- [FFMpeg](https://www.ffmpeg.org/) with H/W Accelleration support
- [VGMStream](https://github.com/losnoco/vgmstream) using FFMpeg as backend (with loop support!)
- [tomlplusplus](https://github.com/marzer/tomlplusplus) ( for the configuration management )
- [StackWalker](https://github.com/JochenKalmbach/StackWalker) ( for stack traces in the log file )
- [pugixml](https://github.com/zeux/pugixml) ( for the Steam XML manifest )
- [md5](http://www.zedwood.com/article/cpp-md5-function) ( for the Steam XML manifest )
- [libpng](http://www.libpng.org/pub/png/libpng.html) ( for a better and faster PNG texture support )
- [imgui](https://github.com/ocornut/imgui) ( to better debug the in-game engine )
- [discohash](https://github.com/cris691/discohash) ( to extract an hash from palette game textures data known as animated textures )
- [SoLoud](https://github.com/jarikomppa/soloud) ( as the audio engine used to playback audio, music or voice files )
- [openpsf](https://github.com/myst6re/openpsf) ( as the MINIPSF emulation engine to playback PSX/PS2 music files )

## Canary vs Latest Release
When you access the releases page, you will see two available download options:
- [**Canary:**](https://github.com/julianxhokaxhiu/FFNx/releases/tag/canary) the latest snapshot of the code. Like a nightly, but untested.
- [**Latest Release:**](https://github.com/julianxhokaxhiu/FFNx/releases/latest) the official release, which is tested and should be stable enough for long gameplay sessions.

## How to build

Tested build profiles:
- x86-Release ( default, the same used to release artifacts in this Github page )
- x86-RelWithDebInfo ( used while developing to better debug some issues )

Output folder: `.dist/build/[CHOSEN_PROFILE]/bin` ( eg. `.dist/build/x86-Release/bin` )

### Visual Studio

> **Please note:**
>
> By default Visual Studio will pick the **x86-Release** build configuration, but you can choose any other profile available.

0) Download the the latest [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) installer
1) Run the installer and import this [.vsconfig](.vsconfig) file in the installer to pick the required components to build this project
2) Once installed, open this repository **as a folder** in Visual Studio 2019 and click the build button.

### Visual Studio Code

0) **REQUIRED!** Follow up the steps to install Visual Studio, which will also install the MSVC toolchain
1) Download and install the latest [Visual Studio Code](https://code.visualstudio.com/) release
2) Install the following extensions:
   - https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
   - https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools
3) Open this repository as a folder in Visual Studio code
4) Choose as build profile in the status bar `CMake: [Release]` ( or one of the aforementioned profiles )
5) Click the button on the status bar `Build`

## How to install
In either way, in order to use this driver you MUST have a legal copy of the game. DO NOT ask for a copy unless bought legally.

### Final Fantasy VII

**Supported Languages:** EN, DE, FR, SP, JP*

> *: Japanese support is currently work in progress. The game starts fine but font is not rendering properly and battles do crash sometimes.

#### [1998 Eidos Release](https://www.mobygames.com/game/windows/final-fantasy-vii)
1. Install the game on this path: `C:\Games\Final Fantasy VII`
2. Update your game to v1.02 ( https://www.gamefront.com/games/final-fantasy-7-advent-children/file/final-fantasy-7-retail-v1-02-patch )
2. Download the latest `FFNx-FF7_1998` release here: https://github.com/julianxhokaxhiu/FFNx/releases
3. Extract the ZIP content next to `ff7.exe` file
4. Double click on [`FFNx.reg`](misc/FF7.reg)
5. Click on Yes.
6. Enjoy!

#### [2013 Steam Release](https://store.steampowered.com/app/39140/FINAL_FANTASY_VII/)
0. Install the game using Steam Client
1. Make sure you run at least your game once ( until the new game screen )
2. Open the installation directory of the game ( see [How to access game files](https://steamcommunity.com/sharedfiles/filedetails/?id=760447682) )
3. Download the latest `FFNx-Steam` release here: https://github.com/julianxhokaxhiu/FFNx/releases
4. Extract the ZIP content next to your `ff7_*.exe` file ( for eg. for EN language `ff7_en.exe`)
5. Replace all files when asked.
6. Enjoy!

#### [2013 eStore Release](http://www.jp.square-enix.com/ffvii-pc-jp/)
1. Install the game using eStore installer.
2. Open the installation directory of the game
3. Download the latest `FFNx-Steam` release here: https://github.com/julianxhokaxhiu/FFNx/releases
4. Extract the ZIP content next to your `ff7_*.exe` file ( for eg. for EN language `ff7_en.exe`)
5. Replace all files when asked.
6. Enjoy!

#### [Android Release](https://play.google.com/store/apps/details?id=com.square_enix.android_googleplay.FFVII)
1. Install the game in your Android device.
2. Locate the OBB file ( usually in `Android/obb` or `/obb` in your internal storage )
3. Save the OBB file in your Windows desktop
4. Rename the OBB file extension from `.obb` to `.zip` and extract it
5. In the extracted folder, go to `ff7_1.02` directory
6. Download the latest `FFNx-Steam` release here: https://github.com/julianxhokaxhiu/FFNx/releases
7. Extract the ZIP content next to the `ff7_*.exe` files
8. Update `FFNx.toml` flags with these values:
```
ffmpeg_video_ext = webm
external_music_path = data/music_2
external_music_ext = akb
```
9. You can now run any `ff7_*.exe` file you prefer. Enjoy!

### Final Fantasy VIII

**Supported Languages:** EN, DE, FR, SP, IT, JP

#### [2000 Squaresoft Release](https://www.mobygames.com/game/windows/final-fantasy-viii)
1. Install the game on this path: `C:\Games\Final Fantasy VIII`
2. Update your game to v1.2 ( search for `ff8_1.2.zip` or `ff8_1.2G.zip` here http://forums.qhimm.com/index.php?topic=12909.msg180223#msg180223 )
2. Download the latest `FFNx-FF8_2000` release here: https://github.com/julianxhokaxhiu/FFNx/releases
3. Extract the ZIP content next to `ff8.exe` file
4. Double click on [`FFNx.reg`](misc/FF8.reg)
5. Enjoy!

#### [2013 Steam Release](https://store.steampowered.com/app/39150/FINAL_FANTASY_VIII/)
0. Install the game using Steam Client
1. Make sure you run at least your game once ( until the new game screen )
2. Open the installation directory of the game ( see [How to access game files](https://steamcommunity.com/sharedfiles/filedetails/?id=760447682) )
3. Download the latest `FFNx-Steam` release here: https://github.com/julianxhokaxhiu/FFNx/releases
4. Extract the ZIP content next to your `ff8_*.exe` file ( for eg. for EN language `ff8_en.exe`)
5. Replace all files when asked.
6. Enjoy!

## How to install on 7h 2.x
Since 7h 2.3+ FFNx will be bundled by default! Just make sure you use always the latest version of 7h and you'll be good to go :)

If you want to use the latest release ( or canary release ) of FFNx, you can follow the [installation instructions](#how-to-install) and finally on 7th **make sure** to use `Play with minimal validation` available within the little arrow menu next to the play button.

> Please remember: **the 7h team WILL NOT be able to support you** if you upgrade, and **limited support** will be provided on the FFNx Discord channel instead.

## Tweak the configuration
> TOML is a standard syntax!
>
> Feel free to checkout [Learn TOML in Y minutes](https://learnxinyminutes.com/docs/toml/) for a quick tutorial.

FFNx is highly configurable! Feel free to tweak every available option within the [FFNx.toml](misc/FFNx.toml) file.

## Audio Engine configuration

The new FFNx Audio Engine is completely configurable by you! Every audio layer can now have its own `config.toml` file, which through it you can customize the Audio Engine behavior when a track is going to be played.

The `config.toml` MUST BE within the relative configured `external_*_path` entry in the [FFNx.toml](misc/FFNx.toml) file.
For example, for the SFX layer on default configuration, the file should be placed in `sfx/config.toml`.

Feel free to check the example configuration included in each FFNx release.

### Audio Engine Layers

The current supported Audio Engine layers are:
- [SFX](misc/FFNx.SFX.toml): in-game audio sound effects ( menu cursor sound, battle sword slash sound, etc. )
- [Music](misc/FFNx.music.toml): in-game audio music ( world theme, field theme, etc. )
- Voice: in-game audio voice acting ( dialog voice acting )

## Cheat codes

FFNx comes with different cheat codes built-in which would allow you to boost or enjoy your gameplay experience.

### FMV Skip

> **Final Fantasy VII ONLY!**

This cheat will allow you to skip movies in-game.

Shortcuts:
- Keyboard Shortcut: `CTRL + S`
- Gamepad Shortcut: `SELECT + START`

### Battle mode toggle

> **Final Fantasy VII ONLY!**

This cheat will allow you to disable battle encounters while in the worldmap or in field.

Shortcuts:
- Keyboard Shortcut: `CTRL + B`
- Gamepad Shortcut: `L3 + R3`

### Speedhack

This cheat will allow you to boost the gameplay timing up to 8x ( by default, [you can configure this]((misc/FFNx.toml#L200)) up to your pleasure ).

Shortcuts:
- Keyboard Shortcut: `CTRL + Arrow Up/Down`
- Gamepad Shortcut: `L2 + R2 + Up/Down` ( DPad works too if using an XInput controller )

You can toggle the speedhack (enable/disable) when you want by using these Shortcuts:
- Keyboard Shortcut: `CTRL + Arrow Left/Right`
- Gamepad Shortcut: `L2 + R2 + Left/Right` ( DPad works too if using an XInput controller )

### Soft Reset

This cheat will allow you to reset the game like on PSX, by triggering the game over screen wherever you are.

Shortcuts:
- Keyboard Shortcut: `CTRL + R`
- Gamepad Shortcut: `L1 + L2 + R1 + R2 + START + SELECT`

## Inspect logs
If you want to check what is going on behind the scene, or you may happen to have a crash, feel free to check the `FFNx.log` file.

## Join us on Discord
If you want to join our community, you can find us on Discord: https://discord.gg/Urq67Uz ( remember to hit the Red Chocobo reaction! )

## Credits

This project could have not been a reality if those people would have not worked on FF7 and FF8 with their deep passion and knowledge.
FFNx makes use also of their work, and I will never be enough grateful to those people. The order is purely Alphabetical.

These people are:
- [Aali](http://forums.qhimm.com/index.php?action=profile;u=2862):
  - for the original Driver code FFNx is based on.
- [Chrysalis](http://forums.qhimm.com/index.php?action=profile;u=674):
  - for the battle fullscreen hext patch
  - for the menu cursor vertical centering in menu
- [DLPB](https://forums.qhimm.com/index.php?action=profile;u=6439):
  - for original Hext concept and specification that FFNx implemented as well
  - for the field vertical centering hext patch, which FFNx provides a default patch for
  - for the Soft-Reset original concept
  - for the no battle original concept
- [dziugo](http://forums.qhimm.com/index.php?action=profile;u=1660):
  - for the original FMV skip concept
- [ficedula](http://forums.qhimm.com/index.php?action=profile;u=68):
  - for 7h 1.x which FFNx provides support for
- [Iros](https://forums.qhimm.com/index.php?action=profile;u=21785):
  - for 7h 1.x which FFNx provides support for
- [JWP](http://forums.qhimm.com/index.php?action=profile;u=3772):
  - for imgui integration within FFNx
- [Kranmer](http://forums.qhimm.com/index.php?action=profile;u=4909)
  - for PHS save everywhere Hext patch
  - for the no battle original concept
- [Maki](http://forums.qhimm.com/index.php?action=profile;u=23937):
  - for FF8 UV Hext Patch in the world map, which FFNx provides a default patch for
  - for the help in getting the first iterations of FFNx running on FF8 2000/2013 release
- [myst6re](http://forums.qhimm.com/index.php?action=profile;u=4778):
  - for the great tools like Makou Reactor, Deling and many others he did which helped a lot in improving FF8 while working on the code
  - for the great help in the code implementing the MINIPSF emulation layer being present inside FFNx
  - for the heavy testing and lifting of a lot of bugs being catched in FFNx, for FF8
  - for the Steam savegame logic in the manifest.xml for FF8
  - for the real-time SFX volume change for FF7
- [quantumpencil](http://forums.qhimm.com/index.php?action=profile;u=23810) and [Nax](https://github.com/nax):
  - for the original CMake files FFNx has based its work upon
  - for all the help in getting some logics wired up in the game engine and a lot of hex addresses I would never been able to figure out myself
- [Satsuki](http://forums.qhimm.com/index.php?action=profile;u=24647):
  - for the heavy testing and lifting of a lot of bugs being catched in FFNx, for FF7
  - for the field vertical centering hext patch, which FFNx provides a default patch for
  - for a lot of hex addresses I would have never been able to figure out myself otherwise
  - for the original Speedhack concept and help in getting it natively into FFNx
- [Sebanisu](http://forums.qhimm.com/index.php?action=profile;u=22866):
  - for the help in getting the first iterations of FFNx running on FF8 2000/2013 release
  - for the heavy testing and lifting of a lot of bugs being catched in FFNx, for FF8
- [sithlord48](http://forums.qhimm.com/index.php?action=profile;u=6501):
  - for the Steam savegame logic in the manifest.xml for FF7
- [TurBoss](https://github.com/TurBoss):
  - for 7h 1.x which FFNx provides support for
- [unab0mb](https://forums.qhimm.com/index.php?action=profile;u=31071):
  - for providing quick help in integrating FFNx on top of 7h 2.x until it gets officially supported

I'm sure I forgot many others through the route. In case you feel you're missing here, feel free to open a PR! I'll be happy to include you because you deserve this.

## License
FFNx is released under GPLv3 license, and you can get a copy of the license here: [COPYING.txt](COPYING.txt)

If you paid for it, remember to ask for a refund to the person who sold you a copy. Make also sure you get a copy of the source code if you got it as a binary only.

If the person who gave you a copy will refuse to give you the source code, report it here: https://www.gnu.org/licenses/gpl-violation.html

All rights belong to their respective owners.
