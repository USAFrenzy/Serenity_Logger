#pragma once

#include "catch.hpp"
#include <serenity/Targets/RotatingTarget.h>

// This one will be extracted to a common file for the target tests
static auto BaseLogPath() {
	auto baseLogPath { std::filesystem::current_path() /= "TempLogs" };
	if( !std::filesystem::exists(baseLogPath) ) {
			std::filesystem::create_directories(baseLogPath);
	}
	return baseLogPath;
}

static auto RotatingLogPath() {
	auto rotatingPath { BaseLogPath() /= "RotatingTests" };
	if( std::filesystem::exists(rotatingPath) ) {
			std::filesystem::remove_all(rotatingPath);
	}
	std::filesystem::create_directories(rotatingPath);
	return rotatingPath;
}

using namespace serenity::experimental;
using namespace serenity::experimental::targets;
using namespace serenity::SERENITY_LUTS;

// clang-format off
static constexpr std::string_view testView{
"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse sed porttitor orci. Nullam "
"aliquet ultrices nisl, porta eleifend tortor. Sed commodo tellus at lorem tincidunt feugiat. Nam "
"porta elit vitae eros dapibus, quis aliquet ante commodo. Pellentesque tempor a purus nec porta."
" Quisque vitae ullamcorper ante. Fusce ac mauris magna. In vulputate at leo vel dapibus. Ut ornare"
" mi non odio."
};
// clang-format on

TEST_CASE("RotateLimits Construction") {
	CHECK_NOTHROW(RotateLimits());
}

TEST_CASE("RotateLimits Construction Default Values") {
	RotateLimits temp;
	REQUIRE(temp.maxNumberOfFiles == 5);
	REQUIRE(temp.fileSizeLimit == 512 * KB);
	REQUIRE(temp.dayModeSettingHour == 0);
	REQUIRE(temp.dayModeSettingMinute == 0);
	REQUIRE(temp.weekModeSetting == 0);
	REQUIRE(temp.monthModeSetting == 1);
}

TEST_CASE("Rotate Target Constructor [1/3]") {
	REQUIRE_NOTHROW(RotatingTarget());
}

TEST_CASE("Rotate Target Constructor [2/3] - 1") {
	REQUIRE_NOTHROW(RotatingTarget("Constructor 2", (RotatingLogPath() /= "Constructor(2-1).txt").make_preferred().string(), false));
}

TEST_CASE("Rotate Target Constructor [2/3] - 2") {
	REQUIRE_NOTHROW(RotatingTarget("Constructor 2", (RotatingLogPath() /= "Constructor(2-2).txt").make_preferred().string(), true));
}

TEST_CASE("Rotate Target Constructor [2/3] - 3  (Default File If Path Doesn't Include File)") {
	RotatingTarget temp("Constructor 2", (RotatingLogPath()).make_preferred().string(), false);
	REQUIRE(temp.FileCacheHelper()->FileName() == "Rotating_Log");
}

TEST_CASE("Rotate Target Constructor [3/3] - 1") {
	REQUIRE_NOTHROW(RotatingTarget("Constructor 2", "|%l| %a %n %T [%N]: %+",
	                             (RotatingLogPath() /= "Constructor(3-1).txt").make_preferred().string(), true));
}

TEST_CASE("Rotate Target Constructor [3/3] - 2") {
	REQUIRE_NOTHROW(RotatingTarget("Constructor 2", "|%l| %a %n %T [%N]: %+",
	                             (RotatingLogPath() /= "Constructor(3-2).txt").make_preferred().string(), false));
}

TEST_CASE("Rotate Target Constructor [3/3] - 3 (Default File If Path Doesn't Include File)") {
	RotatingTarget temp("Constructor 3", (RotatingLogPath()).make_preferred().string(), false);
	REQUIRE(temp.FileCacheHelper()->FileName() == "Rotating_Log");
}

TEST_CASE("SetRotationLimits - 1") {
	RotatingTarget temp;
	RotateLimits limits {};
	temp.SetRotationLimits(limits);
	auto tempLimits { temp.RotationLimits() };
	REQUIRE(tempLimits.dayModeSettingHour == limits.dayModeSettingHour);
	REQUIRE(tempLimits.dayModeSettingMinute == limits.dayModeSettingMinute);
	REQUIRE(tempLimits.fileSizeLimit == limits.fileSizeLimit);
	REQUIRE(tempLimits.maxNumberOfFiles == limits.maxNumberOfFiles);
	REQUIRE(tempLimits.monthModeSetting == limits.monthModeSetting);
	REQUIRE(tempLimits.weekModeSetting == limits.weekModeSetting);
}

TEST_CASE("SetRotationLimits - 2") {
	RotatingTarget temp;
	RotateLimits limits {};
	limits.dayModeSettingHour   = 2;
	limits.dayModeSettingMinute = 23;
	limits.fileSizeLimit        = 1024 * KB;
	limits.maxNumberOfFiles     = 10;
	limits.monthModeSetting     = 8;
	limits.weekModeSetting      = 30;
	temp.SetRotationLimits(limits);
	auto tempLimits { temp.RotationLimits() };
	REQUIRE(tempLimits.dayModeSettingHour == limits.dayModeSettingHour);
	REQUIRE(tempLimits.dayModeSettingMinute == limits.dayModeSettingMinute);
	REQUIRE(tempLimits.fileSizeLimit == limits.fileSizeLimit);
	REQUIRE(tempLimits.maxNumberOfFiles == limits.maxNumberOfFiles);
	REQUIRE(tempLimits.monthModeSetting == limits.monthModeSetting);
	REQUIRE(tempLimits.weekModeSetting == limits.weekModeSetting);
}

TEST_CASE("Rotate On File Size Test [Without Truncation]") {
	auto path { RotatingLogPath() /= "FileSize.txt" };
	auto fileName { path };
	fileName.replace_extension();
	static_assert(testView.size() == 400);

	RotatingTarget rtLogger("FileSizeRotation", path.string(), true);
	rtLogger.SetRotationMode(IntervalMode::file_size);

	RotateLimits limits {};
	limits.fileSizeLimit = 400;
	rtLogger.SetRotationLimits(limits);

	for( int i = 0; i < (limits.maxNumberOfFiles + 5); ++i ) {
			rtLogger.Info("{}", testView);
		}
	rtLogger.CloseFile();

	int fileCount {};
	auto dir { path };
	dir._Remove_filename_and_separator();
	for( const std::filesystem::directory_entry& entry: std::filesystem::recursive_directory_iterator(dir) ) {
			if( entry.is_regular_file() && entry.path().filename().string().find(fileName.string()) )
				REQUIRE(std::filesystem::file_size(entry) <= limits.fileSizeLimit);
			fileCount++;
		}
	REQUIRE(fileCount <= limits.maxNumberOfFiles);
}

TEST_CASE("Rotate On File Size Test [With Truncation]") {
	auto path { RotatingLogPath() /= "FileSize.txt" };
	auto fileName { path };
	fileName.replace_extension();
	static_assert(testView.size() == 400);

	RotatingTarget rtLogger("FileSizeRotation", path.string(), true);
	rtLogger.SetRotationMode(IntervalMode::file_size);
	rtLogger.TruncLogOnFileSize(true);

	RotateLimits limits {};
	limits.fileSizeLimit = 400;
	rtLogger.SetRotationLimits(limits);

	for( int i = 0; i < (limits.maxNumberOfFiles + 5); ++i ) {
			rtLogger.Info("{}", testView);
		}
	rtLogger.CloseFile();
	int fileCount {};
	auto dir { path };
	dir._Remove_filename_and_separator();
	for( const std::filesystem::directory_entry& entry: std::filesystem::recursive_directory_iterator(dir) ) {
			if( entry.is_regular_file() && entry.path().filename().string().find(fileName.string()) )
				REQUIRE(std::filesystem::file_size(entry) <= limits.fileSizeLimit);
			fileCount++;
		}
	REQUIRE(fileCount <= limits.maxNumberOfFiles);
	// Remove files created up to this point
	std::filesystem::remove_all(BaseLogPath().make_preferred());
	std::filesystem::remove_all((std::filesystem::current_path() /= "Logs").make_preferred());
}
