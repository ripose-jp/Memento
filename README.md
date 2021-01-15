# Memento

## Overview

Memento is a FOSS, mpv-based video player intended for studying Japanese.

![image info](example.png)

## Dependencies

* Qt5
* mpv
* sqlite3
* MeCab
* Apache Expat XML Parser

For the best experience, install Noto Sans JP

https://fonts.google.com/specimen/Noto+Sans+JP

## Building

# Linux

To install Memento on Linux, type the following commands:

```
mkdir build
cd build
cmake ..
cmake --build .. --target install
```

# Windows

1. Install [MSys2](https://www.msys2.org/)
1. Make sure MSys2 is up to date by running these commands:
    ```
    pacman -Sy --needed msys2-runtime pacman
    pacman -Su
    ```
1. Install the needed tools and dependencies:
    ```
    pacman -S git mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-cmake mingw-w64-x86_64-sqlite3 mingw-w64-x86_64-qt5 mingw-w64-x86_64-mpv mingw-w64-x86_64-expat mingw-w64-x86_64-mecab mingw-w64-x86_64-mecab-naist-jdic
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

## Configuration

Most mpv shaders, plugins, and configuration files will work out of the box.
The Memento config directory is located at:

# Linux
```
~/.config/memento
```

# Windows
```
[installation directory]\config
```

If any mpv binds or plugins do not work, please create an issue in the issue
tracker.

## JMDict

JMdict can be updated from within Memento by going to "Settings" -> 
"Update JMDict" and selected the JMDict file.

The JMDict file with only English translations can be downloaded here:

ftp://ftp.monash.edu.au/pub/nihongo/JMdict_e.gz

The multilingual version can be downloaded here:

ftp://ftp.monash.edu.au/pub/nihongo/JMdict.gz

## To-do

* Improve query speed.
* Display subtitles over the video player.
* Compile an OS X version.

## Acknowledgements

JMDICT is the property of the Electronic Dictionary Research and Development
Group, and are used in conformance with the Group's licence.
https://www.edrdg.org/jmdict/j_jmdict.html

This code uses code from Jim Breen's jmdict program for creating a database from
JMDict and querying said database.
http://jmdict.sourceforge.net/

MPV Widget code is based off of libmpv example code by w4m.
https://github.com/mpv-player/mpv-examples/tree/master/libmpv/qt_opengl

Some code based off of Baka-MPlayer.
https://github.com/u8sand/Baka-MPlayer

This project uses icons made by Georgiana Ionescu for the Noun Project.
https://thenounproject.com/georgiana.ionescu/

Plus and minus button images were sourced from here.
https://www.svgrepo.com/vectors/plus-symbol-button/