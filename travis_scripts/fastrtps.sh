#!/usr/bin/env bash
git clone --recursive https://github.com/eProsima/Fast-RTPS
cd Fast-RTPS
mkdir build && cd build
cmake -DTHIRDPARTY=ON -DBUILD_JAVA=ON .. 
make
sudo make install
