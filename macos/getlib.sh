#!/bin/bash

# This script find the otool path of a specific library and outputs it to stdout
if [ "$#" -ne 2 ]
then
    >&2 echo "getlib: Invalid number of arguments $#"
    >&2 echo 'getlib: Usage ./getlib.sh <path to executable or library> <library name>'
    exit 1
fi

# https://stackoverflow.com/questions/1055671/how-can-i-get-the-behavior-of-gnus-readlink-f-on-a-mac
# Resolive symlinks
TARGET_FILE=$1

cd `dirname $TARGET_FILE`
TARGET_FILE=`basename $TARGET_FILE`

# Iterate down a (possible) chain of symlinks
while [ -L "$TARGET_FILE" ]
do
    TARGET_FILE=`readlink $TARGET_FILE`
    cd `dirname $TARGET_FILE`
    TARGET_FILE=`basename $TARGET_FILE`
done

# Compute the canonicalized name by finding the physical path
# for the directory we're in and appending the target file.
LIB_DIR=`pwd -P`
LIB_PATH="$LIB_DIR/$TARGET_FILE"

PATH=$(otool -L "$LIB_PATH" | grep -oE '(\/.+?) ' | grep "$2" | xargs | tr -d '\n')
if [[ $PATH == /..* ]]
then
    PATH="$LIB_DIR/$PATH"
fi
echo -n "$PATH"
