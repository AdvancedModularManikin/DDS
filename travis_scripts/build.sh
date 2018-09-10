#!/usr/bin/env bash
pushd ../AMM_Modules
mkdir build && pushd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make
popd && popd
