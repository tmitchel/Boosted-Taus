#!/bin/bash



# - Echo each command to stdout
# - Stop on first command that fails
set -ex

# set permissions for output
sudo chmod 777 /home/travis/build/tmitchel/Boosted-Taus/

cd /home/travis/build/tmitchel/Boosted-Taus/

for i in plugins/*; do
    if [ -f $i ] ; then
        ./build $i test_build
    fi
done