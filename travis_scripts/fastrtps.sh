#!/usr/bin/env bash
git clone --recursive https://github.com/eProsima/Fast-RTPS
cd Fast-RTPS
git submodule update --init
git pull
mkdir build && cd build
cmake -DTHIRDPARTY=ON -DBUILD_JAVA=ON .. 
make
sudo make install
