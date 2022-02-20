@echo off

echo -- Formatting Files...

pushd ..
		clang-format -i -style=file Serenity/src/*.cpp
		clang-format -i -style=file Serenity/src/Color/*.cpp
		clang-format -i -style=file Serenity/src/MessageDetails/*.cpp
		clang-format -i -style=file Serenity/src/Targets/*.cpp
		clang-format -i -style=file Serenity/src/Utilities/*.cpp
		clang-format -i -style=file Serenity/include/serenity/*.h
		clang-format -i -style=file Serenity/include/serenity/Color/*.h
		clang-format -i -style=file Serenity/include/serenity/MessageDetails/*.h
		clang-format -i -style=file Serenity/include/serenity/Targets/*.h
		clang-format -i -style=file Serenity/include/serenity/Utilities/*.h
		clang-format -i -style=file Sandbox/src/*.cpp
popd

echo -- Formatting Finished
