# Memento

## Overview

Memento is a FOSS, mpv-based video player intended for studying Japanese.

![image info](example.png)

## Features

* Grammar aware subtitle search
* Yomichan-style Kanji cards
* Support for Yomichan dictionaries (except pitch accent dictionaries like Kanjium)
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

## Dependencies

* Qt5
* ffmpeg
* mpv
* sqlite3
* MeCab
* Json-C
* libzip
* Qt DBus (Linux)

For the best experience, install [Noto Sans CJK JP](https://www.google.com/get/noto/#sans-jpan)
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
    pacman -S git make mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ffmpeg mingw-w64-x86_64-sqlite3 mingw-w64-x86_64-qt5 mingw-w64-x86_64-mpv mingw-w64-x86_64-mecab mingw-w64-x86_64-json-c mingw-w64-x86_64-libzip
    ```
1. Clone the repository:
    ```
    git clone https://github.com/ripose-jp/Memento.git
    ```
1. Build Memento:
    ```
    cd Memento
    ./windows-build.sh x86_64
    ```
1. The resulting file will be in
    ```
    build/Memento_x86_64
    ```

### Mac OS X

**Important Note:** The OS X version of Memento has a myriad of problems that make it
unusable for casual use. This build guide is intended for OS X developers interested
in fixing the OS X port of Memento. Check this [issue](https://github.com/ripose-jp/Memento/issues/7) 
for a list of confirmed bugs or to contribute new bugs.

1. Install [Homebrew](https://brew.sh/) with this command:
   ```
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```
1. Install the necessary tools and dependencies:
   ```
   brew install git cmake gcc ffmpeg sqlite3 qt5 mpv mecab mecab-ipadic json-c libzip
   ```
1. Clone the repository:
   ```
   git clone https://github.com/ripose-jp/Memento.git
   ```
1. Build Memento:
   ```
   cd memento
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```
1. The resulting executable will be:
   ```
   memento/build/src/memento
   ```

## Configuration

Most mpv shaders, plugins, and configuration files will work out of the box.
The Memento config directory is located at:

### Linux
```
~/.config/memento
```

### Windows
```
[installation directory]\config
```

### Mac OS X
```
~/.config/Memento
```

If any mpv binds or plugins do not work, please create an issue in the issue
tracker.

## Acknowledgements

MpvWidget code is based off of libmpv example code by w4m.

https://github.com/mpv-player/mpv-examples/tree/master/libmpv/qt_opengl

Some code based off of Baka-MPlayer.

https://github.com/u8sand/Baka-MPlayer

UI inspired by and dictionaries provided by Yomichan.

https://foosoft.net/projects/yomichan/

Fullscreen icons sourced from here.

https://www.iconfinder.com/iconsets/material-core

Various icons sourced from here.

https://www.iconfinder.com/iconsets/ionicons

Pause, play, skip, and seek icons sourced from here and used under the [CC 3.0 License](https://creativecommons.org/licenses/by/3.0/)

https://www.iconfinder.com/iconsets/play-rounded
