

#define ENABLE_MEMORY_LEAK_DETECTION 0
#define GENERAL_SANDBOX              0
#define ROTATING_TESTING             0
#define ParseFormatString_TESTING    1

#if ENABLE_MEMORY_LEAK_DETECTION
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#include <stdlib.h>
#endif

#if ParseFormatString_TESTING
	#define ENABLE_PARSE_SECTION
	#include <serenity/MessageDetails/ArgFormatter.h>
#endif

#include <serenity/Targets/ColorConsoleTarget.h>
#include <serenity/Targets/FileTarget.h>
#include <serenity/Targets/RotatingTarget.h>

#include <serenity/Utilities/Utilities.h>

#include <iostream>

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

	using namespace serenity::arg_formatter;
	constexpr std::string_view ParseFormatStringString { "\n{0:*^#{5}X}\n{1:*^#{5}x}\n{2:*^#{5}a}\n{3:*^#{5}E}\n{4:*^#{5}b}\n{5:*^#{5}B}\n{6:s}" };
	constexpr int a { 123456789 };
	constexpr int b { 5 };
	constexpr float c { 32.5f };
	constexpr double d { 54453765675.65675 };
	constexpr int e { 6 };
	constexpr int f { 50 };
	constexpr std::string_view tmp { "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse sed porttitor orci. Nullam "
		                             "aliquet ultrices nisl, porta eleifend tortor. Sed commodo tellus at lorem tincidunt feugiat. Nam "
		                             "porta elit vitae eros dapibus, quis aliquet ante commodo. Pellentesque tempor a purus nec porta."
		                             " Quisque vitae ullamcorper ante. Fusce ac mauris magna. In vulputate at leo vel dapibus. Ut ornare"
		                             " mi non odio." };

	ArgFormatter parseString;
	Instrumentator timer;
	std::string result;
	std::string finalStr;

	serenity::targets::ColorConsole console("", "%+");
	console.SetMsgColor(LoggerLevel::debug, bright_colors::foreground::cyan);

	for( int i { 0 }; i < 3; ++i ) {
			// serenity's format loop using back_insert_iterator
			timer.StopWatch_Reset();
			for( size_t i { 0 }; i < 10'000'000; ++i ) {
					parseString.se_format_to(std::back_inserter(finalStr), ParseFormatStringString, a, b, c, d, e, f, tmp);
					finalStr.clear();
				}
			timer.StopWatch_Stop();
			finalStr.clear();

			auto serenityTime1 { timer.Elapsed_In(time_mode::us) / 10'000'000.0f };
			console.Debug("ArgFormatter se_format_to()  Elapsed Time Over 10,000,000 iterations: [{} us]", std::to_string(serenityTime1));
			parseString.se_format_to(std::back_inserter(finalStr), ParseFormatStringString, a, b, c, d, e, f, tmp);
			console.Info("With Result: {}", finalStr);
			finalStr.clear();

			// serenity's format loop by returning a string
			timer.StopWatch_Reset();
			for( size_t i { 0 }; i < 10'000'000; ++i ) {
					finalStr = parseString.se_format(ParseFormatStringString, a, b, c, d, e, f, tmp);
				}
			timer.StopWatch_Stop();
			finalStr.clear();

			auto serenityTime2 { timer.Elapsed_In(time_mode::us) / 10'000'000.0f };
			console.Debug("ArgFormatter  se_format() Elapsed Time Over 10,000,000 iterations: [{} us]", std::to_string(serenityTime2));
			finalStr = parseString.se_format(ParseFormatStringString, a, b, c, d, e, f, tmp);
			console.Info("With Result: {}", finalStr);
			finalStr.clear();

			// Standard's std::vformat_to() loop
			timer.StopWatch_Reset();
			auto locale { std::locale("") };
			for( size_t i { 0 }; i < 10'000'000; ++i ) {
					L_VFORMAT_TO(finalStr, locale, ParseFormatStringString, a, b, c, d, e, f, tmp);
					finalStr.clear();
				}
			timer.StopWatch_Stop();
			finalStr.clear();

			auto standardTime { timer.Elapsed_In(time_mode::us) / 10'000'000.0f };
			L_VFORMAT_TO(finalStr, locale, ParseFormatStringString, a, b, c, d, e, f, tmp);
			console.Debug("std::vformat_to() Elapsed Time Over 10,000,000 iterations: [{} us]", std::to_string(standardTime));
			console.Info("With Result: {}", finalStr);

			// Just realized why the percentages were so freaking out there - I had broken the formula (WOOPS!)
			auto percentValue { ((serenityTime1 - standardTime) / standardTime) * 100 };
			auto percentValue2 { ((serenityTime2 - standardTime) / standardTime) * 100 };
			auto percentValue3 { ((serenityTime1 - serenityTime2) / serenityTime2) * 100 };

			if( percentValue > 0 ) {
					std::string percentStr { "[" };
					percentStr.append(SetPrecision(percentValue, 2)).append("%]");
					console.Trace("Serenity's se_format_to() is {} Slower Than The Standard's Formatting Function std::vformat_to()", Tag::Red(percentStr));
			} else {
					std::string percentStr { "[" };
					percentStr.append(SetPrecision(std::abs(percentValue), 2)).append("%]");
					console.Trace("Serenity's se_format_to() is {} Faster Than The Standard's Formatting Function std::vformat_to()", Tag::Green(percentStr));
				}

			if( percentValue2 > 0 ) {
					std::string percentStr { "[" };
					percentStr.append(SetPrecision(percentValue2, 2)).append("%]");
					console.Trace("Serenity's se_format() is {} Slower Than The Standard's Formatting Function std::vformat_to()", Tag::Red(percentStr));
			} else {
					std::string percentStr { "[" };
					percentStr.append(SetPrecision(std::abs(percentValue2), 2)).append("%]");
					console.Trace("Serenity's se_format() is {} Faster Than The Standard's Formatting Function std::vformat_to()", Tag::Green(percentStr));
				}

			if( percentValue3 > 0 ) {
					std::string percentStr { "[" };
					percentStr.append(SetPrecision(percentValue3, 2)).append("%]");
					console.Trace("Serenity's se_format_to() Is {} Slower Than Serenity's  se_format()\n", Tag::Red(percentStr));
			} else {
					std::string percentStr { "[" };
					percentStr.append(SetPrecision(std::abs(percentValue3), 2)).append("%]");
					console.Trace("Serenity's se_format_to() Is {} Faster Than Serenity's  se_format()\n", Tag::Green(percentStr));
				}
			printf("\n");
		}

#endif    // ENABLE_PARSE_SECTION

#if ENABLE_MEMORY_LEAK_DETECTION
	_CrtDumpMemoryLeaks();
#endif    //
}
