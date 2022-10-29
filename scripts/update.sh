#!/bin/bash
cd .. > /dev/null
	git pull
cd scripts/dev/bat
	bash clang_format.sh
	bash cmake_format.sh
echo -- Removing all build folders
	bash clean.sh
cd ../../ > /dev/null
