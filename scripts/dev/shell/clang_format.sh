#!/bin/bash

echo "-- Formatting Files..."

cd ../../../ > /dev/null
		clang-format -i -style=file Serenity/src/Color/*.cpp > /dev/null
		clang-format -i -style=file Serenity/src/MessageDetails/*.cpp > /dev/null
		clang-format -i -style=file Serenity/src/Targets/*.cpp > /dev/null
		clang-format -i -style=file Serenity/src/Utilities/*.cpp > /dev/null
		clang-format -i -style=file Serenity/include/serenity/*.h > /dev/null
		clang-format -i -style=file Serenity/include/serenity/Color/*.h > /dev/null
		clang-format -i -style=file Serenity/include/serenity/MessageDetails/*.h > /dev/null
		clang-format -i -style=file Serenity/include/serenity/Targets/*.h > /dev/null
		clang-format -i -style=file Serenity/include/serenity/Utilities/*.h > /dev/null
		clang-format -i -style=file Sandbox/src/*.cpp  > /dev/null
cd scripts/dev/shell > /dev/null
echo "-- Formatting Finished"
