#!/bin/bash
make clean
find . -name "*.swp" |xargs rm -v 2>/dev/null
find . -name "*.swo" |xargs rm -v 2>/dev/null
find . -name "*.swx" |xargs rm -v 2>/dev/null
find . -name "*~" |xargs rm -v 2>/dev/null
find . -name CMakeFiles -exec rm -rf {} \;
find . -iname CMakeCache.txt |xargs rm -rf
find . -name cmake_* -exec rm -rf {} \;
