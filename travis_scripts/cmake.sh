#!/usr/bin/env bash
wget https://cmake.org/files/v3.12/cmake-3.12.0-rc3.tar.gz
tar -xf cmake-3.12.0*.tar.gz
cd cmake-3.12.0-rc3
./bootstrap --parallel=4 --prefix=/usr/local
make -j4
sudo make install
