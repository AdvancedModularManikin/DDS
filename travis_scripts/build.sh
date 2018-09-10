#!/usr/bin/env bash
mkdir build && pushd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make
popd
