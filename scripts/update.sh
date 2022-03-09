#!/bin/bash

bash clean.sh 
  git pull
bash clang_format.sh
bash cmake_format.sh
bash build.sh

cd ../ > /dev/null
