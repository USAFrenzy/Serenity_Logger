

#define ENABLE_MEMORY_LEAK_DETECTION 0
#define GENERAL_SANDBOX              0
#define ROTATING_TESTING             0
#define PARSE_TESTING                1

#if ENABLE_MEMORY_LEAK_DETECTION
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#include <stdlib.h>
#endif

#if PARSE_TESTING
	#define ENABLE_PARSE_SECTION
	#include <serenity/MessageDetails/LazyParser.h>
#endif

#include <iostream>

#include <serenity/Targets/ColorConsoleTarget.h>
#include <serenity/Targets/FileTarget.h>
#include <serenity/Targets/RotatingTarget.h>

#include <serenity/Utilities/Utilities.h>

std::filesystem::path LogDirPath() {
	return (std::filesystem::current_path() /= "Logs");
}

int main() {
	using namespace serenity;
	using namespace se_utils;
	using namespace se_colors;
	using namespace experimental;

#if GENERAL_SANDBOX

	serenity::targets::ColorConsole C;
	serenity::targets::FileTarget testFile;

	#ifdef ENABLE_ROTATION_SECTION
	std::filesystem::path dailyFilePath = LogDirPath() /= "Daily/DailyLog.txt";
	#else
	std::filesystem::path dailyFilePath = LogDirPath() /= "Rotating_Log.txt";
	#endif    // ENABLE_ROTATION_SECTION
	serenity::experimental::targets::RotatingTarget rotatingFile("Rotating_Logger", dailyFilePath.string(), true);

#endif    //  GENERAL_SANDBOX

#ifdef ENABLE_ROTATION_SECTION
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

#endif    // !ENABLE_ROTATION_SECTION

#ifdef ENABLE_PARSE_SECTION
	using namespace lazy_parser;

	std::string parseString { "This is a parse string with brackets 1: {1}, 2: {2:+}, and 3: {3:0{4}.{5}}" };

	LazyParser parser;
	Instrumentator timer;

	ParseResult result {};
	result.remainder = parseString;
	timer.StopWatch_Reset();
	for( size_t i { 0 }; i < 10'000'000; ++i ) {
			// simulating a workload of taking the result and continuing to parse
			for( ;; ) {
					if( result.remainder.size() == 0 ) {
							break;
					} else {
							result = parser.Parse(result.remainder);
							continue;
						}
				}
			result.remainder = parseString;
		}
	timer.StopWatch_Stop();

	std::cout << "LazyParser Parsing Elapsed Time Over 10,000,000 iterations: " << timer.Elapsed_In(time_mode::us) / 10'000'000.0f << " us\n";

#endif    // ENABLE_PARSE_SECTION

#if ENABLE_MEMORY_LEAK_DETECTION
	_CrtDumpMemoryLeaks();
#endif    //
}
/************************************************* Paser Notes *************************************************/
// [06May] Note 1: Parsing Time On Average Takes ~0.47-0.48 us, which is just too damn slow for my use case.
//                 Most of the time is spent in clearing the token storage vector and creating/storing the
//                 unique pointers of the tokens. There's a chance that I can bypass this by making these
//                 token classes apart of a std::variant and access them this way. At the very least, I may
//                 be able to shift parsing over to compile time which would eliminate most of the runtime
//                 checks as well as bypass runtime parsing; this also has the added benefit of being closer
//                 to what libfmt/<format> provides and would allow the only cost of this be in the formatting
//                 of the token itself (which isn't even factored in here - hence why the current timings are
//                 horrid; Adding ~0.5 us to a logger whose latency is ~0.8 us at its slowest is unacceptable).
//
// [06May] Note 2: Changing how the Parse() function is used dropped the times dramatically to ~0.18-0.20 us, so this
//                 is clearly the right direction to go in. Now the Parser() function will return a ParseResults
//                 struct containing the formatted token value (when the Format'x'Token() functions are in place),
//                 the portion of the string up to the substitution brackets, and the portion of the string after the
//                 substitution brackets. Given that no formatting was done in the original version AND in this version
//                 of the Parse() function, this is a more apples-to-apples comparison. Eliminated the usage of
//                 LazyParserHelper class due to bloat of unused variables and functions other than the bracket related
//                 variables and functions.
// [06May] Note 3: Some more micro-optimizations on bracket searching and using string_views vs strings sped this up
//                 to only take ~0.13 us consistently. This is definitely on the right track now.