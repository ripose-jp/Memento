# Memento

[![AUR](https://img.shields.io/aur/version/memento)](https://aur.archlinux.org/packages/memento/)

Memento is a FOSS, mpv-based video player for studying Japanese.

![Example](example.png)

## Features

* Grammar aware subtitle search
* Yomichan-style Kanji cards
* Support for Yomichan dictionaries
* Anki card creation through [AnkiConnect](https://ankiweb.net/shared/info/2055492159)
* Support for mpv upscalers, plugins, and configuration files

## Dictionaries

*   **[JMdict](https://www.edrdg.org/jmdict/edict_doc.html)** (Japanese vocabulary)
    *   [jmdict\_dutch.zip](https://foosoft.net/projects/yomichan/dl/dict/jmdict_dutch.zip)
    *   [jmdict\_english.zip](https://foosoft.net/projects/yomichan/dl/dict/jmdict_english.zip)
    *   [jmdict\_french.zip](https://foosoft.net/projects/yomichan/dl/dict/jmdict_french.zip)
    *   [jmdict\_german.zip](https://foosoft.net/projects/yomichan/dl/dict/jmdict_german.zip)
    *   [jmdict\_hungarian.zip](https://foosoft.net/projects/yomichan/dl/dict/jmdict_hungarian.zip)
    *   [jmdict\_russian.zip](https://foosoft.net/projects/yomichan/dl/dict/jmdict_russian.zip)
    *   [jmdict\_slovenian.zip](https://foosoft.net/projects/yomichan/dl/dict/jmdict_slovenian.zip)
    *   [jmdict\_spanish.zip](https://foosoft.net/projects/yomichan/dl/dict/jmdict_spanish.zip)
    *   [jmdict\_swedish.zip](https://foosoft.net/projects/yomichan/dl/dict/jmdict_swedish.zip)
*   **[JMnedict](https://www.edrdg.org/enamdict/enamdict_doc.html)** (Japanese names)
    *   [jmnedict.zip](https://foosoft.net/projects/yomichan/dl/dict/jmnedict.zip)
*   **[KireiCake](https://kireicake.com/rikaicakes/)** (Japanese slang)
    *   [kireicake.zip](https://foosoft.net/projects/yomichan/dl/dict/kireicake.zip)
*   **[KANJIDIC](http://nihongo.monash.edu/kanjidic2/index.html)** (Japanese kanji)
    *   [kanjidic\_english.zip](https://foosoft.net/projects/yomichan/dl/dict/kanjidic_english.zip)
    *   [kanjidic\_french.zip](https://foosoft.net/projects/yomichan/dl/dict/kanjidic_french.zip)
    *   [kanjidic\_portuguese.zip](https://foosoft.net/projects/yomichan/dl/dict/kanjidic_portuguese.zip)
    *   [kanjidic\_spanish.zip](https://foosoft.net/projects/yomichan/dl/dict/kanjidic_spanish.zip)
*   **[Innocent Corpus](https://web.archive.org/web/20190309073023/https://forum.koohii.com/thread-9459.html#pid168613)** (Term and kanji frequencies across 5000+ novels)
    *   [innocent\_corpus.zip](https://foosoft.net/projects/yomichan/dl/dict/innocent_corpus.zip)
*   **[Kanjium](https://github.com/mifunetoshiro/kanjium)** (Pitch dictionary, see [related project page](https://github.com/toasted-nutbread/yomichan-pitch-accent-dictionary) for details)
    *   [kanjium_pitch_accents.zip](https://foosoft.net/projects/yomichan/dl/dict/kanjium_pitch_accents.zip)

## Troubleshooting

### Secondary Subtitles Don't Work

This is (unfortunately) intended behavior.
If you would like to use secondary subtitles, there are two methods to do so.

#### Method 1

Go to Settings → Options → Search, uncheck "Hide mpv subtitles when search bar
is visible", and check "Hide search bar while playing media".

If the searchable subtitles become harder to read, consider adding a background.
This can be done in Interface settings.
Make sure you set the alpha/opacity channel to 255 in order to avoid your
background being transparent.

#### Method 2

When the Memento is paused, manually toggle subtitle visibility to reveal the
secondary subtitle.
Subtitle visibility is bound to **v** by default.

### Windows: MSVCR100.dll is Missing When Trying to Stream

This error can be fixed by installing the
[Microsoft Visual C++ 2010 Service Pack 1 Redistributable Package (x86)](https://download.microsoft.com/download/1/6/5/165255E7-1014-4D0A-B094-B6A430A6BFFC/vcredist_x86.exe).

### Windows: Updating youtube-dl/yt-dlp

If you're version of youtube-dl is out of date, you may experience degraded streaming performance or websites not working entirely.

Memento's version of youtube-dl can be updated by doing the following:

1. Download [yt-dlp](https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe)
2. Rename the file to `youtube-dl.exe`
3. Put `youtube-dl.exe` in Memento's install directory. For the portable version of Memento, this is located in the same folder as the executable. For the installed version, this is located at `C:\Program Files\Memento` by default.

### macOS: Streaming video doesn't work

This means that mpv cannot find your youtube-dl installation.

To install youtube-dl, paste these commands into Terminal:
```
curl -L https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp -o ~/Library/Preferences/memento/youtube-dl
chmod a+rx ~/Library/Preferences/memento/youtube-dl
```

### macOS: Could not initialize MeCab

Move the Memento application to a directory that doesn't have spaces in the path
like `/Applications`. Unfortunately, this is an limitation of MeCab and there is
no workaround that can be implemented.

## Dependencies

* Qt5 (>= 5.15)
    * QtCore
    * QtGui
    * QtWidgets
    * QtNetwork
    * QtSvg
    * QtDBus (Linux)
    * X11Extras (Linux)
* mpv
* sqlite3
* MeCab
    * With either ipadic or NAIST-jdic installed as a system dictionary on Linux and macOS. This only applies to self-compiled versions, not appimages or app bundles.
* Json-C
* libzip
* youtube-dl or yt-dlp (optional)

For the best experience, install [Noto Sans JP](https://fonts.google.com/noto/specimen/Noto+Sans+JP)
and the [Kanji Stroke Order](https://drive.google.com/uc?export=download&id=1oyQoTB531tbhlYaOW7ugvutXZ7HSlJfW) fonts.

## Building

**I do not guarantee that any branch will successfully build or be bug-free.**
**If you want to build a stable version of Memento, compile a [release](https://github.com/ripose-jp/Memento/releases) from source.**

### Linux

To install Memento on Linux, type the following commands:

```
make
sudo make install
```

### Windows

1. Install [MSYS2](https://www.msys2.org/)
1. Open **MSYS2 MinGW 64-bit**
1. (Optional, may break things if MSYS2 is buggy) Make sure MSYS2 is up to date by running these commands:
    ```
    pacman -Sy --needed msys2-runtime pacman
    pacman -Su
    ```
1. Install the necessary tools and dependencies:
    ```
    pacman -S git make mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-sqlite3 mingw-w64-x86_64-qt5 mingw-w64-x86_64-mpv mingw-w64-x86_64-mecab mingw-w64-x86_64-json-c mingw-w64-x86_64-libzip
    ```
2. Clone the repository:
    ```
    git clone https://github.com/ripose-jp/Memento.git
    ```
3. Build Memento:
    ```
    cd Memento
    ./windows/windows-build.sh x86_64
    ```
4. The resulting file will be in
    ```
    build/Memento_x86_64
    ```

### macOS

**Important:** Clang is the only officially supported compiler for building Memento on macOS.

1. Install [Homebrew](https://brew.sh/) with this command:
   ```
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```
1. Install the necessary tools and dependencies:
   ```
   brew install git cmake sqlite3 qt5 mpv mecab mecab-ipadic json-c libzip
   ```
1. Clone the repository:
   ```
   git clone https://github.com/ripose-jp/Memento.git
   ```
1. Build Memento:
   ```
   cd Memento
   make
   ```
1. The resulting executable will be:
   ```
   Memento/build/src/Memento
   ```

### macOS App Bundle

1. Follow steps 1 - 3 of the macOS build instructions.

1. `dylibbundler` is also needed when creating an app bundle, so install it with:
```
brew install dylibbundler
```

1. Open the **Keychain Access** app.

1. Go to **Keychain Access** > **Certificate Assistant** > **Create a Certificate...**
   in the menubar.

1. Put the name of your certificate in the 'Name' field, set the 'Certificate Type'
   to 'Code Signing', and click 'Create'.

1. Return to your terminal and input:
    ```
    cd Memento
    make appbundle CERT_NAME='<name entered in the last step>'
    ```

1. The resulting app bundle will located at:
    ```
    Memento/build/src/Memento.app
    ```

## Configuration

Most mpv shaders, plugins, and configuration files will work without modification.

It is important to note that mpv and Memento's configuration directories are
**separate**. This means mpv configuration files, scripts, etc. intended to modify
Memento's behavior should be placed in Memento's configuration directory.

The Memento configuration directory is located at:

### Linux
```
~/.config/memento
```

### Windows

Version 0.5.1 and later
```
%APPDATA%\Local\memento
```

Version 0.5.0-1 and earlier
```
[installation directory]\config
```

### macOS
```
~/Library/Preferences/memento
```

If any mpv binds or plugins do not work, please create an issue in the issue
tracker.

## Contributing

Before making a pull request, please read [CONTRIBUTING.md](CONTRIBUTING.md).

## Animebook

If you hate Memento, try [Animebook](https://animebook.github.io/).

https://github.com/animebook/animebook.github.io

## Acknowledgements

MpvWidget code is based off of libmpv example code by w4m.

https://github.com/mpv-player/mpv-examples/tree/master/libmpv/qt_opengl

Some code based off of Baka-MPlayer.

https://github.com/u8sand/Baka-MPlayer

Hardware acceleration achieved on Linux using mpc-qt code.

https://github.com/mpc-qt/mpc-qt

UI inspired by and dictionaries provided by Yomichan.

https://foosoft.net/projects/yomichan/

Fullscreen icons sourced from here.

https://www.iconfinder.com/iconsets/material-core

Various icons sourced from here.

https://www.iconfinder.com/iconsets/ionicons

Pause, play, skip, and seek icons sourced from here and used under the [CC 3.0 License](https://creativecommons.org/licenses/by/3.0/)

https://www.iconfinder.com/iconsets/play-rounded

window_build.sh depends on this script with a slightly expanded blacklist

https://github.com/mpreisler/mingw-bundledlls
