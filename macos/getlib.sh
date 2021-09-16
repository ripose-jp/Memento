#!/bin/sh

# This script find the otool path of a specific library and outputs it to stdout
if [ "$#" -ne 2 ]
then
    >&2 echo "getlib: Invalid number of arguments $#"
    >&2 echo 'getlib: Usage ./getlib.sh <path to executable or library> <library name>'
    exit 1
fi

otool -L "$1" | grep -oE '(\/.+?) ' | grep "$2" | xargs | tr -d '\n'