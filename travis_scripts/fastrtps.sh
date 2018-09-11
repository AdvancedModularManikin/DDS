#!/usr/bin/env bash
cd ../AMM_Modules
git clone https://github.com/eProsima/Fast-RTPS
cd Fast-RTPS
mkdir build && cd build
cmake -DTHIRDPARTY=ON -DBUILD_JAVA=ON .. 
sudo make
sudo make install
