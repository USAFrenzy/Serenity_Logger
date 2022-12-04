

#define ENABLE_MEMORY_LEAK_DETECTION 0
#define GENERAL_SANDBOX              0
#define ROTATING_TESTING             0
#define PARSE_TESTING                1
#define ENABLE_PARSE_BENCHING        0
#define ENABLE_PARSE_SANDBOX         0
#define ENABLE_CTIME_SANDBOX         0

#if ENABLE_ONLY_MOCK_LOG_IN_CUSTOMFMT_SANDBOX
	#define ONLY_MOCK_LOG_MESSAGE_ENABLED
#endif

#if ENABLE_MEMORY_LEAK_DETECTION
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#include <stdlib.h>
#endif

#include <serenity/Targets/ConsoleTarget.h>
#include <serenity/Targets/FileTarget.h>
#include <serenity/Targets/RotatingTarget.h>

#include <serenity/Utilities/Utilities.h>

#include <iostream>

#if PARSE_TESTING
	#include <ArgFormatter/ArgFormatter.h>
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

#ifdef USE_FMT_LIB
	#define FMT_VFORMAT_TO(cont, loc, msg, ...) fmt::vformat_to(std::back_inserter(cont), loc, msg, fmt::make_format_args(__VA_ARGS__))
#endif

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
template<> struct formatter::CustomFormatter<TestPoint>
{
	constexpr void Parse(std::string_view parse) {
		return;
	}

	template<typename ContainerCtx> constexpr auto Format(const TestPoint& p, ContainerCtx& ctx) const {
		formatter::format_to(std::back_inserter(ctx), "({},{})", p.x, p.y);
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

#ifdef USE_FMT_LIB
template<> struct fmt::formatter<TestPoint>
{
	auto parse(fmt::format_parse_context& pc) {
		return pc.end();
	}
	auto format(TestPoint const& p, fmt::format_context& ctx) {
		fmt::format_to(ctx.out(), "({},{})", p.x, p.y);
		return ctx.out();
	}
};
#endif

int main() {
	using namespace serenity;
	using namespace se_utils;
	using namespace se_colors;

#ifdef WINDOWS_PLATFORM
	SetConsoleOutputCP(CP_UTF8);
#endif    // WINDOWS_PLATFORM

#if GENERAL_SANDBOX

	serenity::targets::ConsoleTarget C;
	serenity::targets::FileTarget testFile;

	#ifdef ENABLE_ROTATION_SECTION
	std::filesystem::path dailyFilePath = LogDirPath() /= "Daily/DailyLog.txt";
	#else
	std::filesystem::path dailyFilePath = LogDirPath() /= "Rotating_Log.txt";
	#endif    // ENABLE_ROTATION_SECTION
	serenity::targets::RotatingTarget rotatingFile("Rotating_Logger", dailyFilePath.string(), true);

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

#endif

#ifdef ENABLE_PARSE_SECTION
	using namespace formatter::arg_formatter;

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

	serenity::targets::ConsoleTarget console("", "%+");
	console.SetMsgColor(LoggerLevel::debug, bright_colors::foreground::cyan);

	for( int i { 0 }; i < 3; ++i ) {
			// serenity's format loop using back_insert_iterator
			timer.StopWatch_Reset();
			for( size_t i { 0 }; i < 10'000'000; ++i ) {
					parseString.format_to(std::back_inserter(finalStr), ParseFormatStringString, a, b, c, d, e, f, tmp);
					finalStr.clear();
				}
			timer.StopWatch_Stop();
			finalStr.clear();

			auto serenityTime1 { timer.Elapsed_In(time_mode::ns) / 10'000'000.0f };
			console.Debug("ArgFormatter se_format_to()  Elapsed Time Over 10,000,000 iterations: [{} ns]", std::to_string(serenityTime1));
			parseString.format_to(std::back_inserter(finalStr), ParseFormatStringString, a, b, c, d, e, f, tmp);
			console.Info("With Result: {}", std::string_view(finalStr.data(), finalStr.size()));
			finalStr.clear();

			// serenity's format loop by returning a string
			timer.StopWatch_Reset();
			for( size_t i { 0 }; i < 10'000'000; ++i ) {
		#if TEST_STRING_CASE
					finalStr = parseString.format(ParseFormatStringString, a, b, c, d, e, f, tmp);
		#endif
				}
			timer.StopWatch_Stop();
			finalStr.clear();

			auto serenityTime2 { timer.Elapsed_In(time_mode::ns) / 10'000'000.0f };
			console.Debug("ArgFormatter  se_format() Elapsed Time Over 10,000,000 iterations: [{} ns]", std::to_string(serenityTime2));
		#if TEST_STRING_CASE
			finalStr = parseString.format(ParseFormatStringString, a, b, c, d, e, f, tmp);
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

	std::string loopStr { "[Loop " };
	constexpr int repeatTest = 5;
	constexpr size_t iterations { 100'000'000 };
	std::string_view sv { std::move(formatter::format("[Loop Averages Among {} Iterations]", iterations)) };
	float seTotal {}, stdTotal {};

		#ifdef USE_FMT_LIB
	float fmtTotal {};
		#endif

	for( int i { 0 }; i < repeatTest; ++i ) {
			std::cout << formatter::format("{:*^55}\n", (loopStr + std::to_string(i + 1) + "]"));
			cTimer.StopWatch_Reset();
			for( size_t i { 0 }; i < iterations; ++i ) {
					auto _ { formatter.format("{}", test) };
				}
			cTimer.StopWatch_Stop();
			auto serenityElapsed { cTimer.Elapsed_In(time_mode::ns) / static_cast<float>(iterations) };
			seTotal += serenityElapsed;

			std::cout << formatter::format("Serenity Took An Average Of [{} ns]\nWith Result: {}\n", serenityElapsed, test);

		#ifdef USE_FMT_LIB
			cTimer.StopWatch_Reset();
			for( size_t i { 0 }; i < iterations; ++i ) {
					auto _ { fmt::format("{}", test) };
				}
			cTimer.StopWatch_Stop();
			auto fmtElapsed { cTimer.Elapsed_In(time_mode::ns) / static_cast<float>(iterations) };
			fmtTotal += fmtElapsed;
			std::cout << std::format("fmtlib Took An Average Of [{} ns]\nWith Result: {}\n", fmtElapsed, test);
		#endif

			cTimer.StopWatch_Reset();
			for( size_t i { 0 }; i < iterations; ++i ) {
					auto _ { std::format("{}", test) };
				}
			cTimer.StopWatch_Stop();
			auto standardElapsed { cTimer.Elapsed_In(time_mode::ns) / static_cast<float>(iterations) };
			stdTotal += standardElapsed;
			std::cout << std::format("Standard Took An Average Of [{} ns]\nWith Result: {}\n", standardElapsed, test);
			std::cout << formatter::format("{:*55}\n\n");
		}

	/*************************** Current Stats Ran As Of  21Aug22 ***************************/
	// ********************************** [Loop Averages] **********************************
	// Serenity Total Average Among Loops [51.91447 ns]
	// Standard Total Average Among Loops [83.83898 ns]
	// Serenity Is 38.078 % Faster Than The Standard
	// *************************************************************************************

	auto seAvg { seTotal / repeatTest };
	auto stdAvg { stdTotal / repeatTest };

	std::cout << formatter::format("{:*^55}\n", sv);
	std::cout << formatter::format("Serenity Total Average Among Loops [{} ns]\n", seAvg);
	std::cout << formatter::format("Standard Total Average Among Loops [{} ns]\n", stdAvg);

		#ifdef USE_FMT_LIB
	auto fmtAvg { fmtTotal / repeatTest };
	std::cout << formatter::format("fmtlib Total Average Among Loops [{} ns]\n", fmtAvg);
	std::cout << formatter::format("Serenity Is {}% Faster Than fmtlib\n", SetPrecision((std::abs(fmtAvg - seAvg) / fmtAvg) * 100));
		#endif

	std::cout << formatter::format("Serenity Is {}% Faster Than The Standard\n", SetPrecision((std::abs(stdAvg - seAvg) / stdAvg) * 100));
	std::cout << formatter::format("{:*55}\n\n");

	#endif
#endif    // ENABLE_PARSE_SECTION

#if ENABLE_CTIME_SANDBOX

	std::locale loc("zh_CN.UTF-8");

	std::string timeStr;
	Instrumentator cTimeTimer;
	constexpr size_t timeIterations { 10'000'000 };
	constexpr std::string_view formatString { "{:L%Od%b%Oy %T}" };
	constexpr const char* strftimeString { "%d%b%y %T" };
	std::tm cTime {};
	auto now { std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) };
	LOCAL_TIME(cTime, now);
	serenity::msg_details::Message_Info info("", LoggerLevel::trace, serenity::message_time_mode::local);
	info.TimeDetails().UpdateCache(std::chrono::system_clock::now());

	auto tz { std::chrono::current_zone() };
	auto localTime { tz->to_local(std::chrono::system_clock::now()) };
	std::cout << std::format("{:*^95%d%b%y %T}\n", std::chrono::floor<std::chrono::seconds>(localTime));
	std::cout << std::format(std::locale("en_US"), "Note:\tBench Times Are Based On {:L} Iterations For Each Case Being Benched\n", timeIterations);
	std::cout << std::format("\tTime Updates Are Ignored In These Loops And Use The Initial Time.\n\n");
	/*************************************** NOTES ABOUT ABOVE ***************************************/
	// The spec states: [fill-align] [width] [precision] [locale] [chrono spec]
	// Need to test how locale affects this. For the time being anyways, I think I'll imlement c-time's tm struct
	// instead as that's what I'm currently using in the logger right now.
	/*****************************************************************************************************/

	/*************************************************  Just seeing how fast these conversions are *************************************************/

	// Below conversions for u16Str and u32Str from u8view is in order to keep the encoding of the source
	// file in utf-8 (to avoid the /utf-8 compiler switch, this source page is encoded in utf-8 with BOM)
	constexpr std::u8string_view u8view { u8"一個簡單的中文字符串。" };
	std::string u8Str;
	std::u16string u16Str;
	std::u32string u32Str;
	for( auto& ch: u8view ) u8Str += static_cast<char>(ch);    // need this as iIdon't currently support char8_t
	utf_utils::U8ToU16(u8Str, u16Str);
	utf_utils::U8ToU32(u8Str, u32Str);

	std::string u8result;
	std::u16string u16Result;
	std::u32string u32Result;

	/************************************  Validating Conversions are working correctly before testing timings ************************************/
	std::cout << "Verifying Conversion Functions Are All Working As Intended...\n";
	utf_utils::U16ToU8(u16Str, u8result);
	utf_utils::U8ToU16(u8result, u16Result);
	std::cout << ((u16Str == u16Result) ? "\tLossless Conversion From UTF-16 -> UTF-8 -> UTF-16  Complete\n"
	                                    : "\tData Corruption In Conversion From UTF-16 -> UTF-8 -> UTF-16 Detected\n");
	u8result.clear();
	utf_utils::U32ToU8(u32Str, u8result);
	utf_utils::U8ToU32(u8result, u32Result);
	std::cout << ((u32Str == u32Result) ? "\tLossless Conversion  From  UTF-32 -> UTF-8 -> UTF-32 Complete\n"
	                                    : "\tData Corruption In Conversion From UTF-32 -> UTF-8 -> UTF-32 Detected\n");
	u8result.clear();

	u16Result.clear();
	u32Result.clear();
	utf_utils::U32ToU16(u32Str, u16Result);
	utf_utils::U16ToU32(u16Result, u32Result);
	std::cout << ((u32Str == u32Result) ? "\tLossless Conversion  From  UTF-32 -> UTF-16 -> UTF-32 Complete\n"
	                                    : "\tData Corruption In Conversion From UTF-32 -> UTF-16 -> UTF-32 Detected\n");

	u8result.clear();
	u16Result.clear();
	u32Result.clear();
	utf_utils::U32ToU16(u32Str, u16Result);
	utf_utils::U16ToU8(u16Result, u8result);
	u16Result.clear();
	utf_utils::U8ToU16(u8result, u16Result);
	u32Result.clear();
	utf_utils::U16ToU32(u16Result, u32Result);
	std::cout << ((u32Str == u32Result) ? "\tLossless Conversion  From  UTF-32 -> UTF-16 -> UTF-8 -> UTF-16 -> UTF-32 Complete\n"
	                                    : "\tData Corruption In Conversion From UTF-32 -> UTF-16 -> UTF-8 -> UTF-16 -> UTF-32 Detected\n");
	u8result.clear();
	u16Result.clear();
	u32Result.clear();
	std::cout << "All UTF Conversion Function Verifications Have Finished\n\n";
	/**********************************************************************************************************************************************/
	std::cout << std::format("{:*^95}\n\n", " Benching UTF Conversion Functions ");

	cTimeTimer.StopWatch_Reset();
	for( size_t i { 0 }; i < timeIterations; ++i ) {
			u8result.clear();
			utf_utils::U32ToU8(u32Str, u8result);
		}
	cTimeTimer.StopWatch_Stop();
	std::cout << formatter::format("Serenity Formatter Encoding UTF-32 To UTF-8 Took: [{} ns] \nWith Result: {}\n\n",
	                               cTimeTimer.Elapsed_In(time_mode::ns) / static_cast<float>(timeIterations), u8result);

	cTimeTimer.StopWatch_Reset();
	for( size_t i { 0 }; i < timeIterations; ++i ) {
			u16Result.clear();
			utf_utils::U32ToU16(u32Str, u16Result);
		}
	cTimeTimer.StopWatch_Stop();
	u8result.clear();
	utf_utils::U16ToU8(u16Result, u8result);
	std::cout << formatter::format("Serenity Formatter Encoding UTF-32 To UTF-16 Took: [{} ns] \nWith Result: {}\n\n",
	                               cTimeTimer.Elapsed_In(time_mode::ns) / static_cast<float>(timeIterations), u8result);

	cTimeTimer.StopWatch_Reset();
	for( size_t i { 0 }; i < timeIterations; ++i ) {
			u8result.clear();
			utf_utils::U16ToU8(u16Str, u8result);
		}
	cTimeTimer.StopWatch_Stop();
	std::cout << formatter::format("Serenity Formatter Encoding UTF-16 To UTF-8 Took: [{} ns] \nWith Result: {}\n\n",
	                               cTimeTimer.Elapsed_In(time_mode::ns) / static_cast<float>(timeIterations), u8result);

	cTimeTimer.StopWatch_Reset();
	for( size_t i { 0 }; i < timeIterations; ++i ) {
			u32Result.clear();
			utf_utils::U16ToU32(u16Str, u32Result);
		}
	cTimeTimer.StopWatch_Stop();
	u8result.clear();
	utf_utils::U32ToU8(u32Result, u8result);
	std::cout << formatter::format("Serenity Formatter Encoding UTF-16 To UTF-32 Took: [{} ns] \nWith Result: {}\n\n",
	                               cTimeTimer.Elapsed_In(time_mode::ns) / static_cast<float>(timeIterations), u8result);

	cTimeTimer.StopWatch_Reset();
	for( size_t i { 0 }; i < timeIterations; ++i ) {
			u16Result.clear();
			utf_utils::U8ToU16(u8Str, u16Result);
		}
	cTimeTimer.StopWatch_Stop();
	u8result.clear();
	utf_utils::U16ToU8(u16Result, u8result);
	std::cout << formatter::format("Serenity Formatter Encoding UTF-8 To UTF-16 Took: [{} ns] \nWith Result: {}\n\n",
	                               cTimeTimer.Elapsed_In(time_mode::ns) / static_cast<float>(timeIterations), u8result);

	cTimeTimer.StopWatch_Reset();
	for( size_t i { 0 }; i < timeIterations; ++i ) {
			u32Result.clear();
			utf_utils::U8ToU32(u8Str, u32Result);
		}
	cTimeTimer.StopWatch_Stop();
	u8result.clear();
	utf_utils::U32ToU8(u32Result, u8result);
	std::cout << formatter::format("Serenity Formatter Encoding UTF-8 To UTF-32 Took: [{} ns] \nWith Result: {}\n\n",
	                               cTimeTimer.Elapsed_In(time_mode::ns) / static_cast<float>(timeIterations), u8result);
	/**********************************************************************************************************************************************/

	/********************************************  NOTE ********************************************/
	// It may just have to do with the fact that ArgFormatter has to parse the validity of arguments,
	// but I should definitely look into what can be done to speed these up a bit.
	// EDIT: Complex Time Formatting isn't too off the mark though:
	//             * For the format string of "{:%d%b%y %T}"  :
	//                - ArgFormattter took ~53ns
	//                - FormatterArgs struct version took ~32ns
	// EDIT 2: Given the changes made to allow the complex formatting to occur, the below times have
	//                 been hit by ~5ns performance penalty which is basically nothing, but is worth noting.
	/***********************************************************************************************/
	std::cout << std::format("{:*^95}\n\n", " Benching Time Formatting With Specifiers ");

	// %a Short Day takes ~25ns
	// %b Short Month takes ~26ns
	// %c Time-Date takes ~36ns
	// %d Padded Day takes ~28ns -> %e being the more or less the same had same u8result
	// %y Short year takes ~29ns -> %g takes ~30-31ns
	// %j Day Of Year takes ~29ns
	// %m Padded Month takes~27ns
	// %n newline takes ~27ns
	// %p AMPM takes ~26ns
	// %r 12hour time takes ~34ns (on laptop) and ~31ns on desktop
	// %t tab character takes ~26ns(on laptop)
	// %u ISO day of the week number takes ~25ns
	// %w decimal weekday takes ~28ns (on laptop) and ~26ns on desktop
	// %x MM/DD/YY takes ~32ns (on laptop) and ~29ns on desktop
	// %z UtcOffset takes ~32ns -> first time usage latency somewhat addressed with runtime initialization before formatting occurs
	// %A Long Weekday takes ~28ns
	// %B Long Month takes~29ns
	// %C Truncated Year takes ~30ns
	// %D is using %z
	// %F YYYY-MM-DD takes ~35ns
	// %H 24 Hour takes ~30ns
	// %I 12 Hour takes ~31ns
	// %M minute takes ~30ns
	// %R 24HourMin takes ~33ns
	// %S Seconds takes ~31ns
	// %T 24 hour full time takes ~28ns -> adding subsecond precision to the mix drops this to ~72ns
	// %U Week takes ~29ns
	// %V Iso Week Number takes ~28ns
	// %W Iso Week takes ~31ns
	// %X Same as %T unless localized (localization not implemented yet)
	// %Y Long Year takes ~32ns
	// %Z timezone abbreviated takes ~32ns -> first time usage latency somewhat addressed with runtime initialization before formatting occurs

	cTimeTimer.StopWatch_Reset();
	for( size_t i { 0 }; i < timeIterations; ++i ) {
			timeStr.clear();
			formatter::format_to(std::back_inserter(timeStr), loc, formatString, cTime);
		}
	cTimeTimer.StopWatch_Stop();
	std::cout << formatter::format("Serenity Formatter For Time Specs Took: [{} ns] \nWith Result: {}\n\n",
	                               cTimeTimer.Elapsed_In(time_mode::ns) / static_cast<float>(timeIterations), formatter::format(loc, formatString, cTime));

	// standard
	auto flooredTime { std::chrono::floor<std::chrono::seconds>(localTime) };    // taking this out of the loop (should've been outside the loop anyways)
	cTimeTimer.StopWatch_Reset();
	for( size_t i { 0 }; i < timeIterations; ++i ) {
			timeStr.clear();
			std::format_to(std::back_inserter(timeStr), loc, formatString, flooredTime);
		}
	cTimeTimer.StopWatch_Stop();
	std::cout << std::format("Standard Formatter For Time Specs Took: [{} ns] \nWith Result: {}\n\n",
	                         cTimeTimer.Elapsed_In(time_mode::ns) / static_cast<float>(timeIterations), std::format(loc, formatString, flooredTime));

	// ctime standard
	// Note: internal buff used was for arithmetic formatting but since it makes sense to reuse it, doubling its usage case
	// for time formatting as well - hence why the size is larger than it probably needs to be for time related formatting

	//	constexpr size_t buffSize { 66 };    // mirror size of buff used internally
	// char buff[ buffSize ] {};
	// cTimeTimer.StopWatch_Reset();
	// for( size_t i { 0 }; i < timeIterations; ++i ) {
	//		std::memset(&buff, 0, buffSize);
	//		std::strftime(buff, buffSize, strftimeString, &cTime);
	//	}
	// cTimeTimer.StopWatch_Stop();
	// std::cout << formatter::format("Strftime Formatting For Format Specs Took: [{} ns] \nWith Result: {}\n",
	//                              cTimeTimer.Elapsed_In(time_mode::ns) / static_cast<float>(timeIterations), std::string_view(buff, buffSize));

	// localTime = tz->to_local(std::chrono::system_clock::now());
	// std::cout << std::format("{:*^85%d%b%y %T}\n", std::chrono::floor<std::chrono::seconds>(localTime));

	// *****************************************************************************************************************************
	// In order of performance of time related formatting from the results of the above loops (fastest to slowest):
	// FormatterArgs -> ArgFormatter -> strftime -> std::format
	// *****************************************************************************************************************************
	// Honestly unsure why std::format is relatively fast with other types of formatting but horrendously slow with time formatting.
	// To put it in perspective, FormatterArgs is ~1.5x to ~2x faster than ArgFormatter which is ~6x times faster than strftime
	// which is ~8x faster than std::format... I am going to assume it absolutely has to do with the chrono type checking std::format
	// does when given a time point as I couldn't get std::format to format a std::tm struct -> I assume if they allowed tm struct
	// formatting, the times wouldn't be so heavily skewed as they are. When trying "std::format(formatString, cTime);", the compiler
	// complains about an unknown type and that it can't deduce the type of the tm struct. Same thing  happens when taking a pointer
	// or a reference to the tm struct -> obviously meaning that the tm struct isn't natively supported here.
	// *****************************************************************************************************************************

	/**************************  From The Benching Using Localized Time Formats (21Aug22) **************************/
	// Serenity Formatter For Time Specs Took : [1247.8507 ns]
	// With Result : 218月22 14:04:54
	// Standard Formatter For Time Specs Took : [3020.7104 ns]
	// With Result : 218?22 14:04:54
	/*  The above timings note the huge disparity between std::format in regards to specifically time related formatting */
	// Note that even taking into account the utf-8 encoding done on serenity's side instead of the glyph replacement the
	//  standard uses, serenity is still well above 50% faster than the standard in this case.

#endif    // ENABLE_CTIME_SANDBOX

#if ENABLE_MEMORY_LEAK_DETECTION
	_CrtDumpMemoryLeaks();
#endif    //
}
