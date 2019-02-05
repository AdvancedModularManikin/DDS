#!/usr/bin/env bash
git clone --recursive https://github.com/eProsima/Fast-RTPS
cd Fast-RTPS
git checkout tags/v1.6.0
git submodule update --init
mkdir build && cd build
/usr/local/bin/cmake -DTHIRDPARTY=ON -DBUILD_JAVA=OFF .. 
make
sudo make install
