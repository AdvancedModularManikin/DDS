#!/usr/bin/env bash
git clone --recursive https://github.com/eProsima/Fast-RTPS
cd Fast-RTPS
git fetch && git fetch --tags
git checkout v1.8.1
git submodule update --init
mkdir build && cd build
/usr/local/bin/cmake -DTHIRDPARTY=ON -DBUILD_JAVA=OFF .. 
make
sudo make install
