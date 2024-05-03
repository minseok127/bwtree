#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR

# fail if tying to reference a variable that is not set
set -u

# exit immediately if a command fails
set -e

# echo commands
set -x

mkdir -p build
cd build
cmake ..
cmake --build .
