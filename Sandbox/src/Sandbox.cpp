#include <iostream>

#include <serenity/Targets/ColorConsoleTarget.h>
#include <serenity/Targets/FileTarget.h>
#include <serenity/Targets/RotatingTarget.h>

// clang-format off
// TODO: ####################################################################################################################################
// TODO: Remove Internal Dependancy On Utilities File - I'll work on that as a separate project from this (as it honestly should have been)
// TODO: ####################################################################################################################################
// clang-format on

#define INSTRUMENT 1

#if INSTRUMENT
	#define INSTRUMENTATION_ENABLED
#endif

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
// Want to keep this utility file separate from actual project as it is itself a
// WIP project anyways
#include <serenity/Utilities/Utilities.h>

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
	auto rotatingSink { std::make_shared<spdlog::sinks::rotating_file_sink_st>(SpdlogPath(isRotateFilePath), fileSize, numberOfFiles,
		                                                                   rotateOnOpen) };
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
#ifndef INSTRUMENTATION_ENABLED
	std::filesystem::path dailyFilePath = LogDirPath() /= "Daily/DailyLog.txt";
#else
	std::filesystem::path dailyFilePath = LogDirPath() /= "Rotating_Log.txt";
#endif    // !INSTRUMENTATION_ENABLED

	serenity::experimental::targets::RotatingTarget rotatingFile("Rotating_Logger", dailyFilePath.string(), true);

	// ****************************** TEMPORARY TESTING ********************************************
	/* PeriodicSettings testFileFlushSettings = {};
	 testFileFlushSettings.flushEvery       = std::chrono::seconds(1);
	 Flush_Policy testFIleFlushPolicy(FlushSetting::periodically, PeriodicOptions::timeBased, testFileFlushSettings);
	 testFile.SetFlushPolicy(testFIleFlushPolicy);
	 rotatingFile.SetFlushPolicy(testFIleFlushPolicy);
	 C.SetFlushPolicy(testFIleFlushPolicy);*/
	// testFile.EnableMultiThreadingSupport();
	// rotatingFile.EnableMultiThreadingSupport();
	// C.EnableMultiThreadingSupport();
	//    *********************************************************************************************
	/*	spdlog::flush_every(std::chrono::seconds(1));*/
	// ****************************** TEMPORARY TESTING ********************************************

#ifndef INSTRUMENTATION_ENABLED
	std::cout << "###############################################################"
		     "#####\n";
	std::cout << "# This Will Be The Default Pattern Format And Message Level "
		     "Colors #\n";
	std::cout << "###############################################################"
		     "#####\n";
	// Trace Is Default Color
	C.Trace("Trace");
	// Info Is Light Green
	C.Info("Info");
	// Debug Is Light Cyan
	C.Debug("Debug");
	// Warning Is Light Yellow
	C.Warn("Warning");
	// Error Is Dark Red
	C.Error("Error");
	// Fatal Is Light Yellow On Dark Red
	C.Fatal("Fatal");
	std::cout << "###############################################################"
		     "#####\n\n";
	std::cout << "###############################################################"
		     "#########\n";
	std::cout << "# Testing Some Basic Functions To Make Sure All Is Working As "
		     "I Expect #\n";
	std::cout << "###############################################################"
		     "#########\n";
	C.SetPattern("%T [%N]: %+");
	C.Info("Pattern String Has Been Changed To \"%T [%N]: %+\"");
	C.ColorizeOutput(false);
	C.Info("Colorized Output Set To False, Therefore, This Output Should Be The "
	       "Default Color");
	C.Error("This Line Should Also Still Lack Color");
	C.ColorizeOutput(true);
	C.Info("Colorized Output Re-enabled, therefore this line should be colored");
	C.Info("Log Level Will Be Set To \"warning\"");
	C.SetLogLevel(LoggerLevel::warning);
	C.Info("Log Level Has Been Set To Warning, So This Line Should Not Print");
	C.Warn("However, This Line Should Print");
	C.Fatal("And This Line Should Also Print");
	C.SetLogLevel(LoggerLevel::trace);
	C.Info("Log Level Set Back To \"trace\"");
	C.ResetPatternToDefault();
	C.Info("Pattern String Should Have Been Changed Back To Default Pattern");
	C.SetMsgColor(LoggerLevel::warning, se_colors::bright_colors::combos::cyan::on_grey);
	C.Trace("Warning Level Color Has Been Changed To Cyan On Grey");
	C.Warn("So This Line Should Now Be Cyan On Grey");
	C.SetOriginalColors();
	C.Warn("Colors Should Have Been Reset, So This Should Be Back To Bright Yellow");

	testFile.Trace("This Is A Trace Message To The File");
	testFile.Info("This Is An Info Message To The File");
	testFile.Debug("This Is A Debug Message To The File");
	testFile.Warn("This Is A Warning Message To The File");
	testFile.Error("This Is An Error Message To The File");
	testFile.Fatal("This Is A Fatal Message To The File");
	testFile.RenameFile("Renamed_File.txt");
	testFile.Trace("File Should Have Been Renamed To \"Renamed_File.txt\"");
	testFile.StopBackgroundThread();
	testFile.Flush();

	RotateSettings settings;
	settings.fileSizeLimit         = 512 * KB;
	settings.maxNumberOfFiles      = 10;
	settings.monthModeSetting      = 3;
	settings.weekModeSetting       = 6;
	settings.dayModeSettingHour    = 21;
	settings.dayModeSettingMinute  = 45;

	PeriodicSettings flushSettings = {};
	flushSettings.flushEvery       = std::chrono::seconds(60);
	Flush_Policy flushPolicy(FlushSetting::periodically, PeriodicOptions::timeBased, flushSettings);

	rotatingFile.SetRotateSettings(settings);
	rotatingFile.SetFlushPolicy(flushPolicy);
	rotatingFile.SetRotationMode(RotateSettings::IntervalMode::daily);

	auto onSizeFilePath = LogDirPath() /= "FileSize/OnSizeRotation.txt";
	experimental::targets::RotatingTarget rotatingLoggerOnSize("RotateOnSize_Logger", onSizeFilePath.string(), true);
	rotatingLoggerOnSize.SetRotateSettings(settings);
	rotatingLoggerOnSize.SetFlushPolicy(flushPolicy);
	// should be the default anyways
	rotatingLoggerOnSize.SetRotationMode(RotateSettings::IntervalMode::file_size);

	auto onHourFilePath = LogDirPath() /= "Hourly/OnHourRotation.txt";
	experimental::targets::RotatingTarget rotatingLoggerHourly("RotateHourly_Logger", onHourFilePath.string(), true);
	rotatingLoggerHourly.SetRotateSettings(settings);
	rotatingLoggerHourly.SetFlushPolicy(flushPolicy);
	rotatingLoggerHourly.SetRotationMode(RotateSettings::IntervalMode::hourly);

	// Testing (probably won't work first time around)
	rotatingLoggerOnSize.EnableMultiThreadingSupport();
	rotatingFile.EnableMultiThreadingSupport();
	rotatingLoggerHourly.EnableMultiThreadingSupport();

	size_t rotationIterations = 1'000'000;
	std::mutex consoleMutex;

	auto NotifyConsole = [ & ](std::string message) {
		std::unique_lock<std::mutex> lock(consoleMutex);
		std::cout << message;
	};

	std::cout << "\n\nLogging messages to test rotation on hour mark, daily "
		     "mark, and file size\n\n";
	auto LogHourly = [ & ]() {
		std::mutex hourlyMutex;
		for( int i = 1; i <= rotationIterations; ++i ) {
				std::unique_lock<std::mutex> lock(hourlyMutex);
				rotatingLoggerHourly.Info("Logging message {} to rotating file based on hour mode", i);
				std::string message = "Message ";
				message.append(std::to_string(i)).append(" Logged To File For Rotate On Hourly\n");
				NotifyConsole(message);
				std::this_thread::sleep_for(std::chrono::minutes(1));
			}
	};

	auto LogOnDaily = [ & ]() {
		std::mutex dailyMutex;
		for( int i = 1; i <= rotationIterations; ++i ) {
				std::unique_lock<std::mutex> lock(dailyMutex);
				rotatingFile.Info("Logging message {} to rotating file based on daily mode", i);
				std::string message = "Message ";
				message.append(std::to_string(i)).append(" Logged To File For Rotate On Daily\n");
				NotifyConsole(message);
				std::this_thread::sleep_for(std::chrono::minutes(2));
			}
	};

	// auto LogOnSize = [ & ]() {
	//	std::mutex fileSizeMutex;
	//	for( int i = 1; i <= rotationIterations; ++i ) {
	//			std::unique_lock<std::mutex> lock(fileSizeMutex);
	//			rotatingLoggerOnSize.Info("Logging message {} to rotating file based on file size mode", i);
	//			std::string message = "Message ";
	//			message.append(std::to_string(i)).append(" Logged To File For Rotate On Size\n");
	//			NotifyConsole(message);
	//			std::this_thread::sleep_for(std::chrono::milliseconds(400));
	//		}
	// };

	// std::thread t1 { LogOnSize };
	std::thread t2 { LogHourly };
	std::thread t3 { LogOnDaily };
	while( /* !(t1.joinable()) ||*/ !(t2.joinable()) || !(t3.joinable()) ) {
			std::this_thread::sleep_for(std::chrono::nanoseconds(50));
		}
	// t1.join();
	t2.join();
	t3.join();

#endif    // !INSTRUMENTATION_ENABLED
	// clang-format off
	// NOTE: precision fields for strings not yet implemented
	// Forcing a throw to prevent usage at the moment as the fallback
	// has yet to be implemented
	// TODO: Add Progress trackers here to make sure I don't miss anything
	/***************************** Current Progress *****************************/ /*
	 * [X] Fill and Alignment check:
	 * - {:*<12d} // and any other additional specs in place of 'd'
	 * - {:*<6}
	 * - {:*>6}
	 * - {:*^6}
	 * - {:6d}
	 * - {:6}
	 * Just need to map it over to how the arguments are formatted now [ ]
	 *****************************************************************************/
	// clang-format on

	constexpr const char* msg { "{1}:Test with other characters present: [{0:.2A}]" };

#ifdef INSTRUMENTATION_ENABLED

	// Currently the fallback isn't implemented
	C.EnableFallbackToStd(true);
	testFile.EnableFallbackToStd(true);
	rotatingFile.EnableFallbackToStd(true);

	Instrumentator timer;

	// const char* test = nullptr;
	//  test string
	static_assert(testView.size() == 400);
	std::string temp { testView.data(), testView.size() };
	// test = temp.c_str();
	auto test         = 9875.76;
	std::string test2 = "[Placement Test]";

	auto testStrInMB { (temp.length()) / static_cast<float>(MB) };

	unsigned long int i { 0 };
	const unsigned long int iterations { 2'000'000 };
	C.SetLogLevel(LoggerLevel::off);
	spdlogConsoleLogger->set_level(spdlog::level::off);

	std::cout << "Benching Color Console Target...\n";

	for( i; i < iterations; i++ ) {
			C.Info(msg, test, test2);
		}
	timer.StopWatch_Stop();
	auto consoleSeconds { timer.Elapsed_In(time_mode::sec) };
	auto consoleMillSec { timer.Elapsed_In(time_mode::ms) };
	auto consoleMicroSec { timer.Elapsed_In(time_mode::us) };
	auto ConsoleThroughput { (testStrInMB * iterations) / consoleSeconds };
	std::cout << "\nColor Console Target Bench Finished. Benching Spdlog Color "
		     "Console Sink...\n";

	i = 0;    // reset
	timer.StopWatch_Reset();
	for( i; i < iterations; i++ ) {
			spdlogConsoleLogger->info(msg, test, test2);
		}
	timer.StopWatch_Stop();
	auto spdlogConsoleSeconds { timer.Elapsed_In(time_mode::sec) };
	auto spdlogConsoleMillSec { timer.Elapsed_In(time_mode::ms) };
	auto spdlogConsoleMicroSec { timer.Elapsed_In(time_mode::us) };
	auto SpdlogConsoleThroughput { (testStrInMB * iterations) / spdlogConsoleSeconds };
	std::cout << "\nSpdlog Color Console Sink Bench Finished.  Benching File "
		     "Target...\n";

	i = 0;    // reset
	timer.StopWatch_Reset();
	for( i; i < iterations; i++ ) {
			testFile.Info(msg, test, test2);
		}
	timer.StopWatch_Stop();
	testFile.Flush();
	auto fileSeconds { timer.Elapsed_In(time_mode::sec) };
	auto fileMillSec { timer.Elapsed_In(time_mode::ms) };
	auto fileMicroSec { timer.Elapsed_In(time_mode::us) };
	auto FileThroughput { (testStrInMB * iterations) / fileSeconds };

	std::cout << "\nFile Target Bench Finished. Benching Spdlog Basic File Sink...\n";

	i = 0;    // reset
	timer.StopWatch_Reset();
	for( i; i < iterations; i++ ) {
			spdlogFileLogger->info(msg, test, test2);
		}
	timer.StopWatch_Stop();
	spdlogFileLogger->flush();
	auto spdlogFileSeconds { timer.Elapsed_In(time_mode::sec) };
	auto spdlogFileMillSec { timer.Elapsed_In(time_mode::ms) };
	auto spdlogFileMicroSec { timer.Elapsed_In(time_mode::us) };
	auto SpdlogFileThroughput { (testStrInMB * iterations) / spdlogFileSeconds };
	std::cout << "\nSpdlog Basic File Sink Bench Finished. Benching Rotating "
		     "Target...\n";

	i = 0;    // reset
	timer.StopWatch_Reset();
	for( i; i < iterations; i++ ) {
			rotatingFile.Info(msg, test, test2);
		}
	timer.StopWatch_Stop();
	rotatingFile.Flush();
	auto rotateSeconds { timer.Elapsed_In(time_mode::sec) };
	auto rotateMillSec { timer.Elapsed_In(time_mode::ms) };
	auto rotateMicroSec { timer.Elapsed_In(time_mode::us) };
	auto rotatingThroughput { (testStrInMB * iterations) / rotateSeconds };
	std::cout << "\nRotating Target Bench Finished. Benching Spdlog Rotating Sink...\n";

	i = 0;    // reset
	timer.StopWatch_Reset();
	for( i; i < iterations; i++ ) {
			spdlogRotatingLogger->info(msg, test, test2);
		}
	timer.StopWatch_Stop();
	spdlogRotatingLogger->flush();
	auto spdlogRotateSeconds { timer.Elapsed_In(time_mode::sec) };
	auto spdlogRotateMillSec { timer.Elapsed_In(time_mode::ms) };
	auto spdlogRotateMicroSec { timer.Elapsed_In(time_mode::us) };
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
                                                                "****************** Instrumentation Data (Averaged Over ") << Tag::Yellow(std::to_string(iterations) + " Iterations: ******************\n")
		                        << Tag::Yellow("***************************************************************************************\n");
	// clang-format on
	std::cout << Tag::Bright_Yellow("Color Console Target (ST)\n") << Tag::Bright_Cyan("\t- In Microseconds:\t\t")
		  << Tag::Bright_Green(std::to_string(consoleMicroSec / iterations) + " us\n") << Tag::Bright_Cyan("\t- In Milliseconds:\t\t")
		  << Tag::Bright_Green(std::to_string(consoleMillSec / iterations) + " ms\n") << Tag::Bright_Cyan("\t- In Seconds:\t\t\t")
		  << Tag::Bright_Green(std::to_string(consoleSeconds / iterations) + " s\n");

	std::cout << Tag::Bright_Yellow("Spdlog Color Console Sink (ST)\n") << Tag::Bright_Cyan("\t- In Microseconds:\t\t")
		  << Tag::Bright_Green(std::to_string(spdlogConsoleMicroSec / iterations) + " us\n")
		  << Tag::Bright_Cyan("\t- In Milliseconds:\t\t")
		  << Tag::Bright_Green(std::to_string(spdlogConsoleMillSec / iterations) + " ms\n") << Tag::Bright_Cyan("\t- In Seconds:\t\t\t")
		  << Tag::Bright_Green(std::to_string(spdlogConsoleSeconds / iterations) + " s\n");

	std::cout << Tag::Bright_Magenta("Color Console Target Is " + consolePercent + " Percent Of Spdlog's Color Console Sink Speed\n");

	std::cout << Tag::Bright_Yellow("File Target (ST)\n") << Tag::Bright_Cyan("\t- In Microseconds:\t\t")
		  << Tag::Bright_Green(std::to_string(fileMicroSec / iterations) + " us\n") << Tag::Bright_Cyan("\t- In Milliseconds:\t\t")
		  << Tag::Bright_Green(std::to_string(fileMillSec / iterations) + " ms\n") << Tag::Bright_Cyan("\t- In Seconds:\t\t\t")
		  << Tag::Bright_Green(std::to_string(fileSeconds / iterations) + " s\n");

	std::cout << Tag::Bright_Yellow("Spdlog Basic File Sink (ST)\n") << Tag::Bright_Cyan("\t- In Microseconds:\t\t")
		  << Tag::Bright_Green(std::to_string(spdlogFileMicroSec / iterations) + " us\n") << Tag::Bright_Cyan("\t- In Milliseconds:\t\t")
		  << Tag::Bright_Green(std::to_string(spdlogFileMillSec / iterations) + " ms\n") << Tag::Bright_Cyan("\t- In Seconds:\t\t\t")
		  << Tag::Bright_Green(std::to_string(spdlogFileSeconds / iterations) + " s\n");

	std::cout << Tag::Bright_Magenta("File Target Is " + filePercent + " Percent Of Spdlog's File Sink Speed\n");

	std::cout << Tag::Bright_Yellow("Roating Target (ST)\n") << Tag::Bright_Cyan("\t- In Microseconds:\t\t")
		  << Tag::Bright_Green(std::to_string(rotateMicroSec / iterations) + " us\n") << Tag::Bright_Cyan("\t- In Milliseconds:\t\t")
		  << Tag::Bright_Green(std::to_string(rotateMillSec / iterations) + " ms\n") << Tag::Bright_Cyan("\t- In Seconds:\t\t\t")
		  << Tag::Bright_Green(std::to_string(rotateSeconds / iterations) + " s\n");

	std::cout << Tag::Bright_Yellow("Spdlog Rotating File Sink (ST)\n") << Tag::Bright_Cyan("\t- In Microseconds:\t\t")
		  << Tag::Bright_Green(std::to_string(spdlogRotateMicroSec / iterations) + " us\n")
		  << Tag::Bright_Cyan("\t- In Milliseconds:\t\t") << Tag::Bright_Green(std::to_string(spdlogRotateMillSec / iterations) + " ms\n")
		  << Tag::Bright_Cyan("\t- In Seconds:\t\t\t") << Tag::Bright_Green(std::to_string(spdlogRotateSeconds / iterations) + " s\n");

	std::cout << Tag::Bright_Magenta("Rotating Target Is " + rotatePercent + " Percent Of Spdlog's Rotating File Sink Speed\n");

	std::cout << "\n";
	std::cout << Tag::Bright_Yellow("Program Throughput :\n");
	std::cout << Tag::Bright_Cyan("Color Console Target Throughput:") << "\n  "
		  << Tag::Bright_Green(SetPrecision(ConsoleThroughput) + " MB/s\n");
	std::cout << Tag::Bright_Cyan("spdlog Color Sink Throughput:") << "\n  "
		  << Tag::Bright_Green(SetPrecision(SpdlogConsoleThroughput) + " MB/s\n");
	std::cout << Tag::Bright_Cyan("File Target Throughput:") << "\n  " << Tag::Bright_Green(SetPrecision(FileThroughput) + " MB/s\n");
	std::cout << Tag::Bright_Cyan("spdlog File Sink Throughput:") << "\n  "
		  << Tag::Bright_Green(SetPrecision(SpdlogFileThroughput) + " MB/s\n");
	std::cout << Tag::Bright_Cyan("Rotating Target Throughput:") << "\n  " << Tag::Bright_Green(SetPrecision(rotatingThroughput) + " MB/s\n");
	std::cout << Tag::Bright_Cyan("spdlog Rotating File Sink Throughput:") << "\n  "
		  << Tag::Bright_Green(SetPrecision(SpdlogRotatingThrouput) + " MB/s\n");

	std::cout << "\n";
	std::cout << Tag::Yellow("***************************************************************************************\n");
	std::cout << Tag::Yellow("***************************************************************************************\n");
	std::cout << Tag::Yellow("***************************************************************************************\n\n");

#endif    // INSTRUMENTATION_ENABLED
}
