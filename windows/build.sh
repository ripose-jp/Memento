#!/bin/bash

# Based off of https://github.com/amazingfate/baka-build/blob/master/bakamplayer.sh

# Exit on error
set -e

if [[ $1 == 'x86_64' ]]; then
arch=x86_64
elif [[ $1 == 'i686' ]]; then
arch=i686
else
echo "Please specify either x86_64 or i686 architecture."
exit;
fi

# Exit on error
set -e

if [[ $arch == 'x86_64' ]]; then
PREFIX=/mingw64
LIBGCC=libgcc_s_seh-1.dll
else
PREFIX=/mingw32
LIBGCC=libgcc_s_dw2-1.dll
fi

# build Memento
mkdir -p build
cd build
$PREFIX/bin/cmake -DCMAKE_BUILD_TYPE=Release ${CMAKE_ARGS} ..
$PREFIX/bin/cmake --build . -- -j$(nproc)

# move DLLs and exe to a new directory
mkdir -p Memento_$arch
rm -rf Memento_$arch/memento.exe
cp src/memento.exe Memento_$arch
rm -rf Memento_$arch/memento_debug.exe
cp src/memento_debug.exe Memento_$arch
if [[ "${CMAKE_ARGS}" == *'-DMECAB_SUPPORT=ON'* ]]
then
    cp -r ../dic Memento_$arch
fi
if [[ "${CMAKE_ARGS}" == *'-DOCR_SUPPORT=ON'*  ]]
then
    cp _deps/libmocr-src/build/libmocr.dll Memento_$arch
    cp _deps/libmocr-src/build/libmocr++.dll Memento_$arch
fi

python3 ../windows/mingw-bundledlls.py --copy ./Memento_$arch/memento.exe
cp $PREFIX/bin/libssl-*.dll ./Memento_$arch

cd Memento_$arch
windeployqt6 memento.exe
rm -rf translations

# Get the latest youtube-dl
curl -L https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe -o youtube-dl.exe
