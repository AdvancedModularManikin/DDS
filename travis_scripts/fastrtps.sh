#!/usr/bin/env bash
git clone https://github.com/eProsima/Fast-RTPS
pushd Fast-RTPS
mkdir build && pushd build
cmake -DTHIRDPARTY=ON -DBUILD_JAVA=ON .. 
make
make install
popd && popd
