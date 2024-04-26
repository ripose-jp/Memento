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

if [[ $2 == 'debug' ]]; then
build_type=debug
elif [[ $2 == 'release' ]]; then
build_type=release
else
echo "Please specify either debug or release."
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

root=$(pwd)

# build Memento
mkdir -p build/${build_type}_${arch}
cd build/${build_type}_${arch}
if [ ! -f ./build.ninja ]; then
$PREFIX/bin/cmake -G "Ninja" -DCMAKE_BUILD_TYPE=$build_type ${CMAKE_ARGS} $root
fi
$PREFIX/bin/cmake --build . -- -j$(nproc)

# move DLLs and exe to a new directory
mkdir -p Memento
rm -rf Memento/memento.exe
cp src/memento.exe Memento
rm -rf Memento/memento_debug.exe
cp src/memento_debug.exe Memento
cp -r $root/dic Memento
if [[ "${CMAKE_ARGS}" == *'-DOCR_SUPPORT=ON'*  ]]
then
    cp _deps/libmocr-src/build/libmocr.dll Memento
    cp _deps/libmocr-src/build/libmocr++.dll Memento
fi

cd Memento

if [ ! -f ./Qt6Core.dll ]; then
    python3 $root/windows/mingw-bundledlls.py --copy ./memento.exe
    cp $PREFIX/bin/libssl-*.dll .

    windeployqt6 memento.exe
    rm -rf translations
fi

# Get the latest youtube-dl
if [ ! -f youtube-dl.exe ]; then
    curl -L https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe -o youtube-dl.exe
fi
