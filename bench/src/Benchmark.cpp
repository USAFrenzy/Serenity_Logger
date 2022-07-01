#include <iostream>

#include <serenity/Targets/ColorConsoleTarget.h>
#include <serenity/Targets/FileTarget.h>
#include <serenity/Targets/RotatingTarget.h>
#include <serenity/Utilities/Utilities.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#define INCLUDE_SPDLOG 1

// This is just used to write all logs in the same directory as the serenity
// targets
std::string SpdlogPath(bool rotate = false) {
	if( !rotate ) {
			auto filePath { std::filesystem::current_path() };
			filePath /= "Logs/Spdlog_File.txt";
			return filePath.make_preferred().string();
	} else {
			auto filePath { std::filesystem::current_path() };
			filePath /= "Logs/Spdlog_Rotating.txt";
			return filePath.make_preferred().string();
		}
}

std::filesystem::path LogDirPath() {
	return (std::filesystem::current_path() /= "Logs");
}

// This is from
// (https://stackoverflow.com/questions/16605967/set-precision-of-stdto-string-when-converting-floating-point-values/16606128)
// Just using this to set throughput precision
template<typename T> std::string SetPrecision(const T value, const int precision = 3) {
	std::ostringstream temp;
	temp.precision(precision);
	temp << std::fixed << value;
	return temp.str();
}

// clang-format off
static constexpr std::string_view testView { 
	"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse sed porttitor orci. Nullam "
    "aliquet ultrices nisl, porta eleifend tortor. Sed commodo tellus at lorem tincidunt feugiat. Nam "
    "porta elit vitae eros dapibus, quis aliquet ante commodo. Pellentesque tempor a purus nec porta."
    " Quisque vitae ullamcorper ante. Fusce ac mauris magna. In vulputate at leo vel dapibus. Ut ornare"
   " mi non odio." 
};
// clang-format on

constexpr const char* msg { "{}" };

int main() {
	std::vector<spdlog::sink_ptr> sinks;
	std::string spdlogPattern { "%^|%L| %a %d%b%C %T [%n]: %v%$" };

	auto stdoutSink { std::make_shared<spdlog::sinks::stdout_color_sink_st>() };
	sinks.emplace_back(stdoutSink);
	auto spdlogConsoleLogger = std::make_shared<spdlog::logger>("Console Logger", begin(sinks), end(sinks));
	spdlog::register_logger(spdlogConsoleLogger);
	spdlogConsoleLogger->set_pattern(spdlogPattern);    // equivalent to Target's Default Pattern

	bool truncate = true;
	auto fileSink { std::make_shared<spdlog::sinks::basic_file_sink_st>(SpdlogPath(), truncate) };
	sinks.clear();
	sinks.emplace_back(fileSink);
	auto spdlogFileLogger = std::make_shared<spdlog::logger>("File Logger", begin(sinks), end(sinks));
	spdlog::register_logger(spdlogFileLogger);
	spdlogFileLogger->set_pattern(spdlogPattern);    // equivalent to Target's Default Pattern

	auto fileSize { 512 * KB };
	auto numberOfFiles { 5 };
	auto rotateOnOpen { false };
	// above settings equivalent to default RotatingTarget Settings
	bool isRotateFilePath { true };
	auto rotatingSink { std::make_shared<spdlog::sinks::rotating_file_sink_st>(SpdlogPath(isRotateFilePath), fileSize, numberOfFiles, rotateOnOpen) };
	sinks.clear();
	sinks.emplace_back(rotatingSink);
	auto spdlogRotatingLogger { std::make_shared<spdlog::logger>("Rotating_Logger", begin(sinks), end(sinks)) };
	spdlog::register_logger(spdlogRotatingLogger);
	spdlogRotatingLogger->set_pattern(spdlogPattern);    // equivalent to Target's Default Pattern

	using namespace serenity;
	using namespace se_utils;
	using namespace se_colors;
	using namespace experimental;

	serenity::targets::ColorConsole C;
	serenity::targets::FileTarget testFile;
	serenity::experimental::targets::RotatingTarget rotatingFile("Rotating_Logger", (LogDirPath() /= "Rotating_Log.txt").string(), true);

	Instrumentator timer;
	static_assert(testView.size() == 400);
	// string view gets ~0.25us, c-string gets ~0.36us on basic file target
	constexpr std::string_view test { testView };
	// const char* test {testView.data()};

	auto testStrInMB { (testView.length()) / static_cast<float>(MB) };

	unsigned long int i { 0 };
	const unsigned long int iterations { 2'000'000 };
	C.SetLogLevel(LoggerLevel::off);
	spdlogConsoleLogger->set_level(spdlog::level::off);

	std::cout << "Benching Color Console Target...\n";

	for( i; i < iterations; i++ ) {
			C.Info(msg, test);
		}
	timer.StopWatch_Stop();
	auto consoleSeconds { timer.Elapsed_In(time_mode::sec) };
	auto consoleMillSec { timer.Elapsed_In(time_mode::ms) };
	auto consoleMicroSec { timer.Elapsed_In(time_mode::us) };
	auto consoleNanoSec { timer.Elapsed_In(time_mode::ns) };
	auto ConsoleThroughput { (testStrInMB * iterations) / consoleSeconds };
	std::cout << "\nColor Console Target Bench Finished. Benching Spdlog Color "
				 "Console Sink...\n";

#if INCLUDE_SPDLOG
	i = 0;    // reset
	timer.StopWatch_Reset();
	for( i; i < iterations; i++ ) {
			spdlogConsoleLogger->info(msg, test);
		}
	timer.StopWatch_Stop();
#endif
	auto spdlogConsoleSeconds { timer.Elapsed_In(time_mode::sec) };
	auto spdlogConsoleMillSec { timer.Elapsed_In(time_mode::ms) };
	auto spdlogConsoleMicroSec { timer.Elapsed_In(time_mode::us) };
	auto spdlogConsoleNanoSec { timer.Elapsed_In(time_mode::ns) };
	auto SpdlogConsoleThroughput { (testStrInMB * iterations) / spdlogConsoleSeconds };
	std::cout << "\nSpdlog Color Console Sink Bench Finished.  Benching File "
				 "Target...\n";
	i = 0;    // reset
	timer.StopWatch_Reset();
	for( i; i < iterations; i++ ) {
			testFile.Info(msg, test);
		}
	timer.StopWatch_Stop();
	testFile.Flush();
	auto fileSeconds { timer.Elapsed_In(time_mode::sec) };
	auto fileMillSec { timer.Elapsed_In(time_mode::ms) };
	auto fileMicroSec { timer.Elapsed_In(time_mode::us) };
	auto fileNanoSec { timer.Elapsed_In(time_mode::ns) };
	auto FileThroughput { (testStrInMB * iterations) / fileSeconds };

	std::cout << "\nFile Target Bench Finished. Benching Spdlog Basic File Sink...\n";

#if INCLUDE_SPDLOG
	i = 0;    // reset
	timer.StopWatch_Reset();
	for( i; i < iterations; i++ ) {
			spdlogFileLogger->info(msg, test);
		}
	timer.StopWatch_Stop();
	spdlogFileLogger->flush();
#endif
	auto spdlogFileSeconds { timer.Elapsed_In(time_mode::sec) };
	auto spdlogFileMillSec { timer.Elapsed_In(time_mode::ms) };
	auto spdlogFileMicroSec { timer.Elapsed_In(time_mode::us) };
	auto spdlogFileNanoSec { timer.Elapsed_In(time_mode::ns) };
	auto SpdlogFileThroughput { (testStrInMB * iterations) / spdlogFileSeconds };
	std::cout << "\nSpdlog Basic File Sink Bench Finished. Benching Rotating "
				 "Target...\n";
	i = 0;    // reset
	timer.StopWatch_Reset();
	for( i; i < iterations; i++ ) {
			rotatingFile.Info(msg, test);
		}
	timer.StopWatch_Stop();
	rotatingFile.Flush();
	auto rotateSeconds { timer.Elapsed_In(time_mode::sec) };
	auto rotateMillSec { timer.Elapsed_In(time_mode::ms) };
	auto rotateMicroSec { timer.Elapsed_In(time_mode::us) };
	auto rotateNanoSec { timer.Elapsed_In(time_mode::ns) };
	auto rotatingThroughput { (testStrInMB * iterations) / rotateSeconds };
	std::cout << "\nRotating Target Bench Finished. Benching Spdlog Rotating Sink...\n";

#if INCLUDE_SPDLOG
	i = 0;    // reset
	timer.StopWatch_Reset();
	for( i; i < iterations; i++ ) {
			spdlogRotatingLogger->info(msg, test);
		}
	timer.StopWatch_Stop();
	spdlogRotatingLogger->flush();
#endif
	auto spdlogRotateSeconds { timer.Elapsed_In(time_mode::sec) };
	auto spdlogRotateMillSec { timer.Elapsed_In(time_mode::ms) };
	auto spdlogRotateMicroSec { timer.Elapsed_In(time_mode::us) };
	auto spdlogRotateNanoSec { timer.Elapsed_In(time_mode::ns) };
	auto SpdlogRotatingThrouput { (testStrInMB * iterations) / spdlogRotateSeconds };
	std::cout << "\nSpdlog Rotating File Sink Bench Finished.\n";

	// #################### Below is just the instrumentation print out section ####################
	auto percentConsole = ((consoleMicroSec - spdlogConsoleMicroSec) / spdlogConsoleMicroSec) * 100;
	std::string consolePercent;
	if( percentConsole > 0 ) {
			consolePercent = "- " + std::to_string(std::abs(percentConsole));
	} else {
			consolePercent = "+ " + std::to_string(std::abs(percentConsole));
		}

	auto percentFile = ((fileMicroSec - spdlogFileMicroSec) / spdlogFileMicroSec) * 100;
	std::string filePercent;
	if( percentFile > 0 ) {
			filePercent = "-" + std::to_string(std::abs(percentFile));
	} else {
			filePercent = "+" + std::to_string(std::abs(percentFile));
		}

	auto percentRotating = ((rotateMicroSec - spdlogRotateMicroSec) / spdlogRotateMicroSec) * 100;
	std::string rotatePercent;
	if( percentRotating > 0 ) {
			rotatePercent = "-" + std::to_string(std::abs(percentRotating));
	} else {
			rotatePercent = "+" + std::to_string(std::abs(percentRotating));
		}

	// clang-format off
	std::cout << Tag::Yellow("\n\n***************************************************************************************\n"
                                                                "*************** Instrumentation Data (Averaged Over ") << Tag::Yellow(std::to_string(iterations) + " Iterations): **************\n")
		                        << Tag::Yellow("***************************************************************************************\n");
	// clang-format on
	std::cout << Tag::Bright_Yellow("Color Console Target (ST)\n") << Tag::Bright_Cyan("\t- In Nanoseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(consoleNanoSec / iterations) + " s\n") << Tag::Bright_Cyan("\t- In Microseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(consoleMicroSec / iterations) + " us\n") << Tag::Bright_Cyan("\t- In Milliseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(consoleMillSec / iterations) + " ms\n") << Tag::Bright_Cyan("\t- In Seconds:\t\t\t\t")
			  << Tag::Bright_Green(std::to_string(consoleSeconds / iterations) + " s\n");

	std::cout << Tag::Bright_Yellow("Spdlog Color Console Sink (ST)\n") << Tag::Bright_Cyan("\t- In Nanoseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(spdlogConsoleNanoSec / iterations) + " s\n") << Tag::Bright_Cyan("\t- In Microseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(spdlogConsoleMicroSec / iterations) + " us\n") << Tag::Bright_Cyan("\t- In Milliseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(spdlogConsoleMillSec / iterations) + " ms\n") << Tag::Bright_Cyan("\t- In Seconds:\t\t\t\t")
			  << Tag::Bright_Green(std::to_string(spdlogConsoleSeconds / iterations) + " s\n");

	std::cout << Tag::Bright_Magenta("Color Console Target Is " + consolePercent + " Percent Of Spdlog's Color Console Sink Speed\n");

	std::cout << Tag::Bright_Yellow("File Target (ST)\n") << Tag::Bright_Cyan("\t- In Nanoseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(fileNanoSec / iterations) + " s\n") << Tag::Bright_Cyan("\t- In Microseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(fileMicroSec / iterations) + " us\n") << Tag::Bright_Cyan("\t- In Milliseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(fileMillSec / iterations) + " ms\n") << Tag::Bright_Cyan("\t- In Seconds:\t\t\t\t")
			  << Tag::Bright_Green(std::to_string(fileSeconds / iterations) + " s\n");

	std::cout << Tag::Bright_Yellow("Spdlog Basic File Sink (ST)\n") << Tag::Bright_Cyan("\t- In Nanoseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(spdlogFileNanoSec / iterations) + " s\n") << Tag::Bright_Cyan("\t- In Microseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(spdlogFileMicroSec / iterations) + " us\n") << Tag::Bright_Cyan("\t- In Milliseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(spdlogFileMillSec / iterations) + " ms\n") << Tag::Bright_Cyan("\t- In Seconds:\t\t\t\t")
			  << Tag::Bright_Green(std::to_string(spdlogFileSeconds / iterations) + " s\n");

	std::cout << Tag::Bright_Magenta("File Target Is " + filePercent + " Percent Of Spdlog's File Sink Speed\n");

	std::cout << Tag::Bright_Yellow("Roating Target (ST)\n") << Tag::Bright_Cyan("\t- In Nanoseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(rotateNanoSec / iterations) + " s\n") << Tag::Bright_Cyan("\t- In Microseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(rotateMicroSec / iterations) + " us\n") << Tag::Bright_Cyan("\t- In Milliseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(rotateMillSec / iterations) + " ms\n") << Tag::Bright_Cyan("\t- In Seconds:\t\t\t\t")
			  << Tag::Bright_Green(std::to_string(rotateSeconds / iterations) + " s\n");

	std::cout << Tag::Bright_Yellow("Spdlog Rotating File Sink (ST)\n") << Tag::Bright_Cyan("\t- In Nanoseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(spdlogRotateNanoSec / iterations) + " s\n") << Tag::Bright_Cyan("\t- In Microseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(spdlogRotateMicroSec / iterations) + " us\n") << Tag::Bright_Cyan("\t- In Milliseconds:\t\t\t")
			  << Tag::Bright_Green(std::to_string(spdlogRotateMillSec / iterations) + " ms\n") << Tag::Bright_Cyan("\t- In Seconds:\t\t\t\t")
			  << Tag::Bright_Green(std::to_string(spdlogRotateSeconds / iterations) + " s\n");

	std::cout << Tag::Bright_Magenta("Rotating Target Is " + rotatePercent + " Percent Of Spdlog's Rotating File Sink Speed\n");

	std::cout << "\n";
	std::cout << Tag::Bright_Yellow("Program Throughput :\n");
	std::cout << Tag::Bright_Cyan("Color Console Target Throughput:") << "\n  " << Tag::Bright_Green(SetPrecision(ConsoleThroughput) + " MB/s\n");
	std::cout << Tag::Bright_Cyan("spdlog Color Sink Throughput:") << "\n  " << Tag::Bright_Green(SetPrecision(SpdlogConsoleThroughput) + " MB/s\n");
	std::cout << Tag::Bright_Cyan("File Target Throughput:") << "\n  " << Tag::Bright_Green(SetPrecision(FileThroughput) + " MB/s\n");
	std::cout << Tag::Bright_Cyan("spdlog File Sink Throughput:") << "\n  " << Tag::Bright_Green(SetPrecision(SpdlogFileThroughput) + " MB/s\n");
	std::cout << Tag::Bright_Cyan("Rotating Target Throughput:") << "\n  " << Tag::Bright_Green(SetPrecision(rotatingThroughput) + " MB/s\n");
	std::cout << Tag::Bright_Cyan("spdlog Rotating File Sink Throughput:") << "\n  " << Tag::Bright_Green(SetPrecision(SpdlogRotatingThrouput) + " MB/s\n");

	std::cout << "\n";
	std::cout << Tag::Yellow("***************************************************************************************\n");
	std::cout << Tag::Yellow("***************************************************************************************\n");
	std::cout << Tag::Yellow("***************************************************************************************\n\n");
}    // main