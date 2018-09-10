#!/usr/bin/env bash
cd ../AMM_Modules
mkdir build && cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make
