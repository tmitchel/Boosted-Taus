#!/bin/bash

echo `ls`
echo `pwd`
echo $TRAVIS_BUILD_DIR
$TRAVIS_BUILD_DIR/build genStudy.cc