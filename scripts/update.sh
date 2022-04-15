#!/bin/bash

bash clean.sh 
popd > /dev/null
  git pull
pushd > /dev/null
bash clang_format.sh
bash cmake_format.sh
bash build.sh

cd ../ > /dev/null
