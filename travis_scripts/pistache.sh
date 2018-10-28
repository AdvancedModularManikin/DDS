#!/usr/bin/env bash
git clone https://github.com/oktal/pistache.git
git pull
mkdir build
cd build
/usr/local/bin/cmake ../
make
sudo make install
