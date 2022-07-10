

#define ENABLE_MEMORY_LEAK_DETECTION 0
#define GENERAL_SANDBOX              0
#define ROTATING_TESTING             0
#define PARSE_TESTING                1
#define ENABLE_PARSE_BENCHING        0
#define ENABLE_PARSE_SANDBOX         0
#define ENABLE_CTIME_SANDBOX         1

#if ENABLE_MEMORY_LEAK_DETECTION
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#include <stdlib.h>
#endif

#include <serenity/Targets/ColorConsoleTarget.h>
#include <serenity/Targets/FileTarget.h>
#include <serenity/Targets/RotatingTarget.h>

#include <serenity/Utilities/Utilities.h>

#include <iostream>

#if PARSE_TESTING
	#include <serenity/MessageDetails/ArgFormatter.h>
	#include <format>
	#define ENABLE_PARSE_SECTION
	// Since the inclusion of USE_STD_FORMAT and USE_FMTLIB , need to force this to be MSVC for testing purposes atm
	#ifdef VFORMAT_TO
		#undef VFORMAT_TO
		#if _MSC_VER >= 1930 && (_MSVC_LANG >= 202002L)
			#define CONTEXT                         std::back_insert_iterator<std::basic_string<char>>
			#define VFORMAT_TO(cont, loc, msg, ...) std::vformat_to<CONTEXT>(std::back_inserter(cont), loc, msg, std::make_format_args(__VA_ARGS__))
		#elif(_MSC_VER >= 1929) && (_MSVC_LANG >= 202002L)
			#if _MSC_FULL_VER >= 192930145    // MSVC build that backported fixes for <format> under C++20 switch instead of C++ latest
				#define VFORMAT_TO(cont, loc, msg, ...) std::vformat_to(std::back_inserter(cont), loc, msg, std::make_format_args(__VA_ARGS__))
			#else
				#define CONTEXT                         std::basic_format_context<std::back_insert_iterator<std::basic_string<char>>, char>
				#define VFORMAT_TO(cont, loc, msg, ...) std::vformat_to(std::back_inserter(cont), loc, msg, std::make_format_args<CONTEXT>(__VA_ARGS__))
			#endif
		#endif
	#endif    // VFORMAT_TO
#endif        // PARSE_TESTING

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

struct TestPoint
{
	int x, y;
};

// Test for specialization
// The requirement mine has is that both parse and format must be at least defined
template<> struct serenity::CustomFormatter<TestPoint>
{
	constexpr void Parse(std::string_view parse) {
		return;
	}

	template<typename ContainerCtx> constexpr auto Format(const TestPoint& p, ContainerCtx& ctx) const {
		serenity::format_to(std::back_inserter(ctx), "({},{})", p.x, p.y);
	}
};

template<> struct std::formatter<TestPoint>
{
	auto parse(std::format_parse_context& pc) {
		return pc.end();
	}
	auto format(TestPoint const& p, std::format_context& ctx) {
		std::format_to(ctx.out(), "({},{})", p.x, p.y);
		return ctx.out();
	}
};

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

	#if ENABLE_PARSE_BENCHING
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

		// Now std::vector<char> and string containers are more or less on par with one another, with strings being only ~20ns faster
		#define TEST_STRING_CASE 1

		#if TEST_STRING_CASE
	std::string finalStr;
		#else
	std::vector<char> finalStr {};
		#endif

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

			auto serenityTime1 { timer.Elapsed_In(time_mode::ns) / 10'000'000.0f };
			console.Debug("ArgFormatter se_format_to()  Elapsed Time Over 10,000,000 iterations: [{} ns]", std::to_string(serenityTime1));
			parseString.se_format_to(std::back_inserter(finalStr), ParseFormatStringString, a, b, c, d, e, f, tmp);
			console.Info("With Result: {}", std::string_view(finalStr.data(), finalStr.size()));
			finalStr.clear();

			// serenity's format loop by returning a string
			timer.StopWatch_Reset();
			for( size_t i { 0 }; i < 10'000'000; ++i ) {
		#if TEST_STRING_CASE
					finalStr = parseString.se_format(ParseFormatStringString, a, b, c, d, e, f, tmp);
		#endif
				}
			timer.StopWatch_Stop();
			finalStr.clear();

			auto serenityTime2 { timer.Elapsed_In(time_mode::ns) / 10'000'000.0f };
			console.Debug("ArgFormatter  se_format() Elapsed Time Over 10,000,000 iterations: [{} ns]", std::to_string(serenityTime2));
		#if TEST_STRING_CASE
			finalStr = parseString.se_format(ParseFormatStringString, a, b, c, d, e, f, tmp);
		#endif
			console.Info("With Result: {}", std::string_view(finalStr.data(), finalStr.size()));
			finalStr.clear();

			// Standard's std::vformat_to() loop
			timer.StopWatch_Reset();
			auto locale { std::locale("") };
			for( size_t i { 0 }; i < 10'000'000; ++i ) {
					VFORMAT_TO(finalStr, locale, ParseFormatStringString, a, b, c, d, e, f, tmp);
					finalStr.clear();
				}
			timer.StopWatch_Stop();
			finalStr.clear();

			auto standardTime { timer.Elapsed_In(time_mode::ns) / 10'000'000.0f };
			VFORMAT_TO(finalStr, locale, ParseFormatStringString, a, b, c, d, e, f, tmp);
			console.Debug("std::vformat_to() Elapsed Time Over 10,000,000 iterations: [{} ns]", std::to_string(standardTime));
			console.Info("With Result: {}", std::string_view(finalStr.data(), finalStr.size()));

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
	#endif

	#if ENABLE_PARSE_SANDBOX

	ArgFormatter formatter {};
	Instrumentator cTimer;
	constexpr TestPoint test { .x { 5 }, .y { 8 } };

	constexpr std::string_view sv { "[Loop Averages]" };
	std::string loopStr { "[Loop " };
	constexpr int repeatTest = 5;
	constexpr size_t iterations { 100'000'000 };
	float seTotal {}, stdTotal {};

	for( int i { 0 }; i < repeatTest; ++i ) {
			std::cout << serenity::format("{:*^55}\n", (loopStr + std::to_string(i + 1) + "]"));
			cTimer.StopWatch_Reset();
			for( size_t i { 0 }; i < iterations; ++i ) {
					auto _ { formatter.se_format("{}", test) };
				}
			cTimer.StopWatch_Stop();
			auto serenityElapsed { cTimer.Elapsed_In(time_mode::ns) / static_cast<float>(iterations) };
			seTotal += serenityElapsed;
			std::cout << serenity::format("Serenity Took An Average Of [{} ns]\nWith Result: {}\n", serenityElapsed, test);
			cTimer.StopWatch_Reset();
			for( size_t i { 0 }; i < iterations; ++i ) {
					auto _ { std::format("{}", test) };
				}
			cTimer.StopWatch_Stop();
			auto standardElapsed { cTimer.Elapsed_In(time_mode::ns) / static_cast<float>(iterations) };
			stdTotal += standardElapsed;
			std::cout << std::format("Standard Took An Average Of [{} ns]\nWith Result: {}\n", standardElapsed, test);
			std::cout << serenity::format("{:*55}\n\n");
		}

	/*************************** Current Stats Ran As Of  08Jul22 ***************************/
	// ********************************** [Loop Averages] **********************************
	// Serenity Total Average Among Loops [48.059757 ns]
	// Standard Total Average Among Loops [83.25582 ns]
	// Serenity Is 42.275 % Faster Than The Standard
	// *************************************************************************************
	auto seAvg { seTotal / repeatTest };
	auto stdAvg { stdTotal / repeatTest };
	std::cout << serenity::format("{:*^55}\n", sv);
	std::cout << serenity::format("Serenity Total Average Among Loops [{} ns]\n", seAvg);
	std::cout << serenity::format("Standard Total Average Among Loops [{} ns]\n", stdAvg);
	std::cout << serenity::format("Serenity Is {}% Faster Than The Standard\n", SetPrecision((std::abs(stdAvg - seAvg) / stdAvg) * 100));
	std::cout << serenity::format("{:*55}\n");

	#endif
#endif    // ENABLE_PARSE_SECTION

#if ENABLE_CTIME_SANDBOX
	auto tz { std::chrono::current_zone() };
	auto localTime { tz->to_local(std::chrono::system_clock::now()) };
	std::cout << std::format("{:*^65%T}\n", std::chrono::floor<std::chrono::seconds>(localTime));
	/*************************************** NOTES ABOUT ABOVE ***************************************/
	// The spec states: [fill-align] [width] [precision] [locale] [chrono spec]
	//
	// I need to test the precision more and see what's allowed/disallowed because I couldn't get the result
	// I wanted without using std::chrono::floor() here . Also need to test how locale affects this.
	// For the time being anyways, I think I'll imlement c-time's tm struct instead as that's what I'm currently
	// using in the logger right now.

	std::tm cTime {};
	auto now { std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) };
	LOCAL_TIME(cTime, now);
	serenity::msg_details::Message_Formatter::Formatters formatter {};
	serenity::msg_details::Message_Info info("", LoggerLevel::trace, serenity::message_time_mode::local);
	formatter.Emplace_Back(std::make_unique<Format_Arg_T>(info));
	formatter.Emplace_Back(std::make_unique<Format_Arg_Char>(std::string_view(".", 1)));
	formatter.Emplace_Back(std::make_unique<Format_Arg_e>(3));
	std::string timeStr;
	Instrumentator cTimeTimer;

	// just benching strictly the formatting -> not the time updates

	// takes ~28ns as %T
	// adding subsecond precision to the mix drops this to ~72ns
	cTimeTimer.StopWatch_Reset();
	for( size_t i { 0 }; i < 100'000'000; ++i ) {
			timeStr.clear();
			serenity::format_to(std::back_inserter(timeStr), "{:.3%T}", cTime);
		}
	cTimeTimer.StopWatch_Stop();
	std::cout << serenity::format("Serenity Formatter For Time Spec Took: [{} ns] \nWith Result: {}\n", cTimeTimer.Elapsed_In(time_mode::ns) / 100'000'000.f, timeStr);

	// takes ~14ns as %T
	// adding subsecond precision to the mix drops this down to ~68ns
	cTimeTimer.StopWatch_Reset();
	for( size_t i { 0 }; i < 100'000'000; ++i ) {
			timeStr.clear();
			timeStr.append(formatter.FormatUserPattern());
		}
	cTimeTimer.StopWatch_Stop();
	std::cout << serenity::format("FormatterArgs Struct For Time Spec Took: [{} ns] \nWith Result: {}\n", cTimeTimer.Elapsed_In(time_mode::ns) / 100'000'000.f, timeStr);
	// NOTE: due to the struct formatter never dealing with fill-align-width-precision-or spec verification before formatting the result, it will probably always be
	// faster
	//               than the ArgFormatter version, however, a ~14ns delta isn't a terrible start at all for plain %T, especially when subsecond precision's delta is
	//               only ~4ns. Not to mention the ability to drop the numberStr array in the near future here as the whole finding the digit as a char with the
	//               offset is coming in at roughly the same timings as indexing into that array.

	// Commenting out MSVC standard as it's incredibly slow (~98% slower than the above 2 methods) taking ~1360ns
	// -> stepping through the calls and I presume a lot of time is spent checking  the chrono specific bits before it ever
	//       gets to filling the tm struct they call. Therefore, instead of benching against MSVC, I'll be using it to make sure
	//      the formatted results match up.

	// cTimeTimer.StopWatch_Reset();
	// for (size_t i{ 0 }; i < 100'000; ++i) {
	//	timeStr.clear();
	//	std::format_to(std::back_inserter(timeStr), "{:%T}", std::chrono::floor<std::chrono::seconds>(localTime));
	// }
	// cTimeTimer.StopWatch_Stop();
	// std::cout << serenity::format("Standard Formatter For Time Spec Took: [{} ns] \nWith Result: {}\n", cTimeTimer.Elapsed_In(time_mode::ns) / 100'000.f,
	// timeStr); serenity::format_to(std::back_inserter(timeStr), "{:%T}", cTime);

#endif    // ENABLE_CTIME_SANDBOX

#if ENABLE_MEMORY_LEAK_DETECTION
	_CrtDumpMemoryLeaks();
#endif    //
}
