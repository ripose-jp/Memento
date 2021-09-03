#!/bin/bash

# Based off of https://github.com/amazingfate/baka-build/blob/master/bakamplayer.sh

if [[ $1 == 'x86_64' ]]; then
arch=x86_64
elif [[ $1 == 'i686' ]]; then
arch=i686
else
echo "Please specify either x86_64 or i686 architecture."
exit;
fi

if [[ $arch == 'x86_64' ]]; then
PREFIX=/mingw64
LIBGCC=libgcc_s_seh-1.dll
else
PREFIX=/mingw32
LIBGCC=libgcc_s_dw2-1.dll
fi

# build Memento
mkdir build
cd build
$PREFIX/bin/cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release ..
$PREFIX/bin/cmake --build . -- -j4

# move DLLs and exe to a new directory
mkdir Memento_$arch
rm -rf Memento_$arch/memento.exe
cp src/memento.exe Memento_$arch
cp -r ../dic Memento_$arch

python3 ../mingw-bundledlls.py --copy ./Memento_$arch/memento.exe
cp $PREFIX/bin/libssl-1_1-x64.dll ./Memento_$arch

cd Memento_$arch
windeployqt memento.exe
rm -rf translations

# Get the latest youtube-dl
wget https://yt-dl.org/downloads/latest/youtube-dl.exe
