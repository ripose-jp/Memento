# [Memento]

--------------

## Overview

Memento is a FOSS, mpv-based video player intended for studying Japanese.

![image info](example.png)

## Dependencies

* Qt5
* mpv
* sqlite3

## Installing

To install Memento on Linux, type the following commands:

```
cmake .
sudo cmake --build . --target install
```

Currently Linux is the only officially supported OS. Windows and OS X will need
some configuration to be compiled, but the source code is likely portable as is.

## Configuration

Most mpv shaders, plugins, and configuration files will work out of the box.
The Memento config directory is located at:

```
~/.config/memento
```

If any mpv binds or plugins do not work, please create an issue in the issue
tracker.

## To-do

* Display definitions for all potential matches, rather than just the first 
match.
* Improve query processing to be aware of Japanese grammar.
    * Example: 食べられない　→　食べられる　→　食べる
* Improve query speed.
* Add AnkiConnect support for automatically making flashcards from defintions.
* Add support for regenerating the JMDict database from within Memento.
* Display subtitles over the video player.
* Compile Window and OS X versions.

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