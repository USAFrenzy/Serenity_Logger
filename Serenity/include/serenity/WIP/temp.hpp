#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/android_sink.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/wincolor_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/win_eventlog_sink.h>

#include <string>
#include <vector>
#include <iostream>  // used for ostream &os = std::cout as default

// clang-format off

//             It's A Little Hacky At The Moment, But The Order Should Be As Follows:
/************************************************************************************************************
# 1) Main Goal Is To Fill Out LoggerProperties Struct
#	- For Whatever Sink Shold Be Added, Call AddSinkForLogger(LoggerSinks::SinkType sink)
#		- This Will Populate The std::vector<LoggerSinks::SinkType> sinkTypeList variable
# 2) Next Goal Would Be To Default Initialize The LogFileInfo struct (i.e. LogFileInfo filePropInfo = {})
#	- Then Alter Any Fields Corresponding To Sinks That Were Added
#		- Next, set LogSinkInfo* logSinkInfo = &LogFileInfo;
#		- This Will Ensure That LoggerProperties Recieves The Correct Information To Pass On
# 3) Now Call The void InitializeSinks( )
#	- This will now populate the std::vector<spdlog::sink_ptr> logSinks variable with the sink signatures
#	required and the altered fields from step 2 
# 4) Now We Can Call InitializeLoggers(LoggerLevel setLevel) to initialize the Logger class member 
#    variables as spdlog::logger types using the loggerName variables and the sinks from step 3
***************************************************************************************************************
* Ideally, This Would Be A Little More Straight-Forward, But Once A Way To Dynamically Set The Log Levels For
* set_level() and flush_on() are in place, as well as a way to dynamically set the pattern (Can Set Defaults
* Here And Implement Targeted Functions For This), Then All Of This Can Be Wrapped In The Init() Function
* And It Should In Theory Work? Library Works As Is Right Now, So I May Just Hold Off On All These Changes For
* A Bit Until I Can Actually Think Of A Better Way To Approach This
*/

// clang-format on

struct LogSinkInfo
{
	// Should Probably look into using std::filesystem here?
	// (NOTE): Due to the way I Have The Functions Working, I Need To Add Separate Name Fields And Other Fields
	// That Have Yet To Be Added For Individual Sinks Here
#if defined( WIN32 ) || ( _WIN32 )
	const char *separator = "/";
#else
	const char *separator = "\\";
#endif  // Separator definition
	std::string fileName = "Log.txt";
	std::string fileDir  = "Logs";
	// basic file sink specific
	bool isTruncated = false;
	// Rotating file sink specific
	std::size_t maxFileSize;
	std::size_t maxNumFiles;
	bool        rotateWhenOpened = false;
	// ansicolor sink specific
	FILE *             file;
	spdlog::color_mode colorMode = spdlog::color_mode::automatic;
	// daily sink specific
	int      rotateHour;
	int      rotateMin;
	uint16_t maxDailySinkFiles = 0;
	// ostream sink specific
	std::ostream &os         = std::cout;
	bool          forceFlush = false;
	// wincolor sink specific
	void *outputHandle;
} LogFileInfo;

// Not An Exhaustive List Of LoggerSinks::SinkType (spdlog Has Many More), Just Ones I Want To Support
struct LoggerSinks
{
	enum class SinkType
	{
		androidSink,
		ansiColorSink,
		basicFileSink,
		dailyFileSink,
		ostreamSink,
		rotatingFileSink,
		stdoutColorSink,
		stdoutSink,
		winDebugSink,
		winEventSink,
		winColorSink
	};
	std::vector<spdlog::sink_ptr> logSinks;
} SE_sinks;

struct LoggerProperties
{
	void AddSinkForLogger( LoggerSinks::SinkType sink )
	{
		sinkTypeList.emplace_back( sink );
	}
	LogSinkInfo *                      logSinkInfo = &LogFileInfo;
	std::vector<LoggerSinks::SinkType> sinkTypeList;
	std::vector<spdlog::sink_ptr> *    loggerSinks = &SE_sinks.logSinks;
};

namespace serenity
{
	spdlog::sink_ptr CreateSinks( std::vector<LoggerSinks::SinkType> sinkTypeList )
	{
		for( auto const &sink : sinkTypeList ) {
			using Sink = LoggerSinks::SinkType;
			switch( sink ) {
				case Sink::androidSink:
					{
#ifdef __ANDROID__
						SE_sinks.logSinks.emplace_back( std::make_shared<spdlog::sinks::android_sink_mt>( ) );
#endif
					}
					break;
				case Sink::ansiColorSink:
					{
						SE_sinks.logSinks.emplace_back( std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>(
						  LogFileInfo.file, LogFileInfo.colorMode ) );
					}
					break;
				case Sink::basicFileSink:
					{
						SE_sinks.logSinks.emplace_back( std::make_shared<spdlog::sinks::basic_file_sink_mt>(
						  ( LogFileInfo.fileDir + LogFileInfo.separator + LogFileInfo.fileName ),
						  LogFileInfo.isTruncated ) );
					}
					break;
				case Sink::dailyFileSink:
					{
						SE_sinks.logSinks.emplace_back( std::make_shared<spdlog::sinks::daily_file_sink_mt>(
						  LogFileInfo.fileName, LogFileInfo.rotateHour, LogFileInfo.rotateMin,
						  LogFileInfo.isTruncated, LogFileInfo.maxDailySinkFiles ) );
					}
					break;
				case Sink::ostreamSink:
					{
						SE_sinks.logSinks.emplace_back( std::make_shared<spdlog::sinks::ostream_sink_mt>(
						  LogFileInfo.os, LogFileInfo.forceFlush ) );
					}
					break;
				case Sink::rotatingFileSink:
					{
						SE_sinks.logSinks.emplace_back( std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
						  LogFileInfo.fileName, LogFileInfo.maxFileSize, LogFileInfo.maxNumFiles,
						  LogFileInfo.rotateWhenOpened ) );
					}
					break;
				case Sink::stdoutColorSink:
					{
						SE_sinks.logSinks.emplace_back( std::make_shared<spdlog::sinks::stdout_color_sink_mt>( ) );
					}
					break;
				case Sink::stdoutSink:
					{
						SE_sinks.logSinks.emplace_back( std::make_shared<spdlog::sinks::stdout_sink_mt>( ) );
					}
					break;
				case Sink::winColorSink:
					{
						SE_sinks.logSinks.emplace_back( std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>(
						  LogFileInfo.outputHandle, LogFileInfo.colorMode ) );
					}
					break;
				case Sink::winDebugSink:
					{
						SE_sinks.logSinks.emplace_back( std::make_shared<spdlog::sinks::msvc_sink_mt>( ) );
					}
					break;
				case Sink::winEventSink:
					{
						SE_sinks.logSinks.emplace_back( std::make_shared<spdlog::sinks::win_eventlog_sink_mt>( ) );
					}
					break;
				default:
					{
						throw std::runtime_error( "Sink Undefined" );
					}
					break;
			}
		}
	}

}  // namespace serenity

// this include will go away when I can test things properly and slowly integrate them/change them to work with the
// Logger class
#include "serenity/Logger.hpp"


struct LoggerInfo
{
	LoggerInfo( Logger logger )
	{
		AddLoggerToList( logger );
	}
	void AddLoggerToList( Logger logger )
	{
		this->loggerName == logger.GetLoggerName( );
		loggers.emplace_back( logger );
	}
	const std::string           loggerName;
	std::vector<Logger>         loggers;
	std::vector<spdlog::logger> loggersList;
	LoggerProperties            loggerProps = { };

} SE_Logger;

namespace serenity
{
	void InitializeSinks( )
	{
		// This Now Populates logSinks vector via what was added into sinkTypeList
		// Now we pass the logsinks vector into a function that will have the begin(sinks), end(sinks)
		// parameters;
		serenity::CreateSinks( SE_Logger.loggerProps.sinkTypeList );
	}

	void InitializeLoggers( LoggerLevel setLevel )
	{
		int  i           = 0;
		auto mappedLevel = Logger::MapLogLevel( setLevel );
		for( i; i < SE_Logger.loggers.size( ); i++ ) {
			auto loggerName     = SE_Logger.loggers[ i ].GetLoggerName( );
			auto clientLogger   = SE_Logger.loggers[ i ].GetClientSideLogger( );
			auto internalLogger = SE_Logger.loggers[ i ].GetClientSideLogger( );
			clientLogger =
			  std::make_shared<spdlog::logger>( loggerName, begin( SE_sinks.logSinks ), end( SE_sinks.logSinks ) );
			spdlog::register_logger( clientLogger );
			clientLogger->set_level( mappedLevel );
			clientLogger->flush_on( mappedLevel );
			internalLogger =
			  std::make_shared<spdlog::logger>( loggerName, begin( SE_sinks.logSinks ), end( SE_sinks.logSinks ) );
			spdlog::register_logger( internalLogger );
			internalLogger->set_level( mappedLevel );
			internalLogger->flush_on( mappedLevel );
		}
	}
}  // namespace serenity
