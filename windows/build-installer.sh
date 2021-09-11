#!/bin/bash

./windows/windows-build.sh x86_64
cp -p ./windows/installer.nsi ./build/.
cp -p ./windows/license.rtf ./build/.
cp -p ./windows/logo.ico ./build/.
cd ./build
makensis installer.nsi
