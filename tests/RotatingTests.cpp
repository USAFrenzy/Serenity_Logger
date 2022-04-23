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

	for( int i = 0; i < limits.maxNumberOfFiles; ++i ) {
			rtLogger.Info("{}", testView);
		}
	rtLogger.CloseFile();
	for( size_t i = 0; i < limits.maxNumberOfFiles; ++i ) {
			auto file { fileName.string().append("_").append(numberStr[ i + 1 ]).append(".txt") };
			REQUIRE(std::filesystem::file_size(file) <= limits.fileSizeLimit);
		}
	std::filesystem::remove_all(RotatingLogPath());
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

	for( int i = 0; i < limits.maxNumberOfFiles; ++i ) {
			rtLogger.Info("{}", testView);
		}
	rtLogger.CloseFile();
	for( size_t i = 0; i < limits.maxNumberOfFiles; ++i ) {
			auto file { fileName.string().append("_").append(numberStr[ i + 1 ]).append(".txt") };
			REQUIRE(std::filesystem::file_size(file) <= limits.fileSizeLimit);
		}
	std::filesystem::remove_all(RotatingLogPath());
}
