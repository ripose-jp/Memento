#!/bin/bash

# Exit on error
set -e

./windows/build.sh $1
cp -p ./windows/installer.nsi ./build/.
cp -p ./windows/license.rtf ./build/.
cp -p ./windows/logo.ico ./build/.
cd ./build
makensis installer.nsi
