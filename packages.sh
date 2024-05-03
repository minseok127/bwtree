#!/bin/bash

DIR="$( cd "$( dirname "$(BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR

# fail if tying to reference a variable that is not set
set -u

# exit immediately if a command fails
set -e

# echo commands
set -x

# prepare cmake install
apt-get update
apt-get install wget
apt-get install libssl-dev

# make build directory
mkdir -p build
cd build

# download latest cmake version
wget https://github.com/Kitware/CMake/releases/download/v3.29.2/cmake-3.29.2.tar.gz
tar xvf cmake-3.29.2.tar.gz

# build and install cmake
cd cmake-3.29.2
./bootstrap && make && make install
