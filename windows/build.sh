#!/bin/bash

# Based off of https://github.com/amazingfate/baka-build/blob/master/bakamplayer.sh

# Exit on error
set -e

ARCH=x86_64
PREFIX=/ucrt64
LIBGCC=libgcc_s_seh-1.dll

# build Memento
mkdir -p build
cd build
$PREFIX/bin/cmake "$@" ..
$PREFIX/bin/cmake --build . -- -j$(nproc)

# move DLLs and exe to a new directory
mkdir -p Memento_$ARCH
rm -rf Memento_$ARCH/memento.exe
cp memento.exe Memento_$ARCH
if [[ -d translations ]]
then
    cp -r translations Memento_$ARCH
elif [[ -d src/translations ]]
then
    cp -r src/translations Memento_$ARCH
fi
if [[ " $@ " =~ ' -DMEMENTO_MECAB_SUPPORT=ON ' ]]
then
    cp -r ../dic Memento_$ARCH
fi
if [[ " $@ " =~ ' -DMEMENTO_OCR_SUPPORT=ON '  ]]
then
    cp _deps/libmocr-src/build/libmocr.dll Memento_$ARCH
    cp _deps/libmocr-src/build/libmocr++.dll Memento_$ARCH
fi

python3 ../windows/mingw-bundledlls.py --copy ./Memento_$ARCH/memento.exe
cp $PREFIX/bin/libssl-*.dll ./Memento_$ARCH

cd Memento_$ARCH
PATH="${PREFIX}/share/qt6/bin:$PATH" windeployqt --qmldir ../../src/qml memento.exe

# Get the latest youtube-dl
curl -L https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe -o yt-dlp.exe
