#!/usr/bin/env bash
git clone https://github.com/Tencent/rapidjson.git
cd rapidjson
git submodule update --init
mkdir build && cd build
/usr/local/bin/cmake -DRAPIDJSON_BUILD_TESTS=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF -DBUILD_DOC=OFF ../
make
sudo make install
