#!/usr/bin/env bash
git clone --recursive https://github.com/eProsima/Fast-RTPS
cd Fast-RTPS
git submodule update --init
mkdir build && cd build
/usr/local/bin/cmake -DTHIRDPARTY=ON -DBUILD_JAVA=ON .. 
make
sudo make install
