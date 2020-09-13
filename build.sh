#!/bin/bash
cmake -H. -B bin
cmake --build bin -- -j3