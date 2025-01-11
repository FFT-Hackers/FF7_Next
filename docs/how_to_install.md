# How-to: Install

Independently of the way you decide to install FFNx, in order to use it you MUST have a legal copy of the game. Support will NOT be provided if the game will NOT be detected as genuine.

## Canary vs Stable

FFNx comes in two flavors, resembling the Google Chrome release names. Unlike this one though, we don't need all the others.

- **Stable:** this is a fully tested, and hopefully, bug free release.
  > It is commonly accepted for long gameplay sessions and generic users. If in doubt, use this one first.
- **Canary:** this is like a nightly release, but untested. Feel free to use it at your own risk.
  > This is what users need to try before reporting any issue encountered in this channel, or if you want to try the latest development updates going on in FFNx.

## Standalone

### Final Fantasy VII

**Supported Languages:** EN, DE, FR, SP, JP

#### [1998 Eidos Release](https://www.mobygames.com/game/windows/final-fantasy-vii)

0. Install the game on this path: `C:\Games\Final Fantasy VII`
1. Update your game to v1.02 ( https://www.gamefront.com/games/final-fantasy-7-advent-children/file/final-fantasy-7-retail-v1-02-patch )
2. Download the latest `FFNx-FF7_1998` release here: https://github.com/julianxhokaxhiu/FFNx/releases
3. Extract the ZIP content next to `ff7.exe` file
4. Double click on [`FFNx.reg`](https://github.com/julianxhokaxhiu/FFNx/blob/master/misc/FF7.reg)
5. Click on Yes.
6. Enjoy!

#### [2013 Steam Release](https://store.steampowered.com/app/39140/FINAL_FANTASY_VII/)

0. Install the game using the Steam Client
1. Make sure you run at least your game once ( until the new game screen )
2. Open the installation directory of the game ( see [How to access game files](https://steamcommunity.com/sharedfiles/filedetails/?id=760447682) )
3. Download the latest `FFNx-Steam` release here: https://github.com/julianxhokaxhiu/FFNx/releases
4. Extract the ZIP content next to your `ff7_*.exe` file ( for eg. for EN language `ff7_en.exe`)
5. Replace all files when asked.
6. Enjoy!

#### [2013 eStore Release](http://www.jp.square-enix.com/ffvii-pc-jp/)

0. Install the game using the eStore installer.
1. Open the installation directory of the game
2. Download the latest `FFNx-Steam` release here: https://github.com/julianxhokaxhiu/FFNx/releases
3. Extract the ZIP content next to your `ff7_*.exe` file ( for eg. for EN language `ff7_en.exe`)
4. Replace all files when asked.
5. Enjoy!

#### [Android Release](https://play.google.com/store/apps/details?id=com.square_enix.android_googleplay.FFVII)

0. Install the game in your Android device.
1. Locate the OBB file ( usually in `Android/obb` or `/obb` in your internal storage )
2. Save the OBB file in your Windows desktop
3. Rename the OBB file extension from `.obb` to `.zip` and extract it
4. In the extracted folder, go to `ff7_1.02` directory
5. Download the latest `FFNx-Steam` release here: https://github.com/julianxhokaxhiu/FFNx/releases
6. Extract the ZIP content next to the `ff7_*.exe` files
7. Update `FFNx.toml` flags with these values:

```toml
ffmpeg_video_ext = "webm"
external_music_path = "data/music_2"
external_music_ext = "akb"
```

9. You can now run any `ff7_*.exe` file you prefer. Enjoy!

### Final Fantasy VIII

**Supported Languages:** EN, DE, FR, SP, IT, JP

#### [2000 Squaresoft Release](https://www.mobygames.com/game/windows/final-fantasy-viii)

0. Install the game on this path: `C:\Games\Final Fantasy VIII`
1. Update your game to v1.2 ( you can find a collection here: https://www.ff8.fr/telechargements/programmes )
2. Download the latest `FFNx-FF8_2000` release here: https://github.com/julianxhokaxhiu/FFNx/releases
3. Extract the ZIP content next to `ff8.exe` file
4. Double click on [`FFNx.reg`](https://github.com/julianxhokaxhiu/FFNx/blob/master/misc/FF8.reg)
5. Enjoy!

#### [2013 Steam Release](https://store.steampowered.com/app/39150/FINAL_FANTASY_VIII/)

0. Install the game using Steam Client
1. Make sure you run at least your game once ( until the new game screen )
2. Open the installation directory of the game ( see [How to access game files](https://steamcommunity.com/sharedfiles/filedetails/?id=760447682) )
3. Download the latest `FFNx-Steam` release here: https://github.com/julianxhokaxhiu/FFNx/releases
4. Extract the ZIP content next to your `ff8_*.exe` file ( for eg. for EN language `ff8_en.exe`)
5. Replace all files when asked.
6. Enjoy!

## Mod Launchers

### 7thHeaven 2.4.0+

0. Install Final Fantasy VII using the Steam Client
1. Download and install the latest 7th Heaven release here: https://github.com/tsunamods-codes/7th-Heaven/releases
2. Launch `7th Heaven` and click Play on the top left
3. Enjoy!

### SYW Steam all in one

0. Install Final Fantasy VII using the Steam Client
1. Follow installation instructions here: http://forum.tsunamods.com/viewtopic.php?f=69&p=716&sid=a91d0f964f413cc24c649298cf6f764b#p716
2. Enjoy!
