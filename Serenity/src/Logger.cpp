
#include "serenity/Logger.hpp"

#include <map>
#include <string>

#pragma warning(push, 0)
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

/*           NOTE FOR FUTURE, SINCE DOING OBSERVER-LIKE PATTERNS ARE STILL NEW FOR ME RIGHT NOW
###########################################################################################################################
	Used as a psuedo call-back in a mock observer pattern. There is most likely a better way to implement
   something like this, however, this will only do work: calling UpdateFileInfo() if and only if a function has
   changed a file info property (currently only paths). There's a NotifyLogger() function that simply sets
   fileInfoChanged to true which then will allow the poll for updated info to populate. The rest is really
   straight-forward: toggle the bool to "reset" and only allow a function that changes the fields to change this
   value.
   - To Ensure Bool Stays Safe To Use:
		- I believe by always having NotifyLogger() simply set fileInfoChanged = true, then multiple calls
that change file info fields won't run the risk of inadverdantly blocking the UpdateLoggerFileInfo() function
		- In The Same Regard, by always waiting for fileInfoChanged to be true, UpdateLoggeFileInfo() will
never execute code uneccessarily (although it does always perform a condition check).
###########################################################################################################################

*/
void Logger::UpdateLoggerFileInfo( )
{
	if(fileInfoChanged) {
		logFileHandle.UpdateFileInfo(logFileHandle.optCurrentDir,
					     logFileHandle.optLogDir,
					     logFileHandle.optFilePath,
					     logFileHandle.optFileName);
	}
	fileInfoChanged = (!fileInfoChanged);
}

std::shared_ptr<spdlog::logger> Logger::m_internalLogger;
std::shared_ptr<spdlog::logger> Logger::m_clientLogger;
Logger* Logger::loggerInstance;

// clang-format off
/* 
	In The Near Future, Would like to be able to just do 
	- Logger(LoggerInfo loggerInfoStruct, LogFileInfo logInfoStruct, LogSink sinkInfo);
*/
// clang-format on

Logger::Logger(std::string loggerName, LogFileHelper* fileInfo, LoggerLevel level)
{
	m_loggerName = loggerName;
	loggerInstance = this;
	Init(*loggerInstance, *fileInfo, level);
}

Logger::~Logger( )
{
	logFileHandle.~LogFileHelper( );
	spdlog::drop_all( );
	spdlog::shutdown( );
}

namespace serenity {
	std::string GetSerenityVerStr( )
	{
		auto version = VERSION_NUMBER(SERENITY_MAJOR, SERENITY_MINOR, SERENITY_REV);
		return version;
	}
} // namespace serenity


void Logger::Init(Logger& logger, LogFileHelper& file, LoggerLevel setLevel)
{
	// Keeping this pretty simple before deviating too hard core - end goal would be to abstract some of the
	// setup in some structs and then just pass in the desired struct into the init function
	auto mappedLevel = MapLogLevel(setLevel);
	std::vector<spdlog::sink_ptr> sinks;

	sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>( ));
	sinks.emplace_back(
	  std::make_shared<spdlog::sinks::basic_file_sink_mt>(file.GetFileName( ).string( ), true));
	// Would Like To Format this in a more personalized and absstracted manner
	sinks [0]->set_pattern("%^[%T] %n: %v%$");
	sinks [1]->set_pattern("[%T] [%l] %n: %v");

	// For both logger types, would like to abstract away the mappedLevel in a way that a flush level doesnt
	// always have to be the same as what is set
	if(m_internalLogger.get( ) == nullptr) {
		m_internalLogger = std::make_shared<spdlog::logger>("INTERNAL", begin(sinks), end(sinks));
		spdlog::register_logger(m_internalLogger);
		m_internalLogger->set_level(mappedLevel);
		m_internalLogger->flush_on(mappedLevel);
	} else {
		SE_INTERNAL_WARN("Warning: Trying To Initialize A Logger Of Same Name: {}", logger.GetLoggerName());
	}

	if(m_clientLogger.get( ) == nullptr) {
		m_clientLogger =
		  std::make_shared<spdlog::logger>(logger.GetLoggerName( ), begin(sinks), end(sinks));
		spdlog::register_logger(m_clientLogger);
		m_clientLogger->set_level(mappedLevel);
		m_clientLogger->flush_on(mappedLevel);
	} else {
		SE_WARN("Warning: Trying To Initialize A Logger Of Same Name: {}", logger.GetLoggerName( ));
	}
}

using MappedLevel = serenity::MappedLevel;

void Logger::SetLoggerLevel(LoggerLevel level, LoggerInterface logInterface)
{
	m_level = MapLogLevel(level);
	if(!m_level) {
		throw std::runtime_error("Log Level Was Not A Valid Value");
	}
	switch(logInterface) {
		case LoggerInterface::internal:
			{
				m_internalLogger->set_level(m_level);
			}
		case LoggerInterface::client:
			{
				m_clientLogger->set_level(m_level);
			}
		default:
			{
				m_internalLogger->set_level(MappedLevel::off);
				m_clientLogger->set_level(MappedLevel::off);
				throw std::runtime_error("Log Interface Was Not A Valid Value - Log Level Set To "
							 "'OFF'\n");
			}
	}
}

serenity::MappedLevel Logger::MapLogLevel(LoggerLevel level)
{
	std::map<LoggerLevel, MappedLevel> levelMap = {{LoggerLevel::trace, MappedLevel::trace},
						       {LoggerLevel::info, MappedLevel::info},
						       {LoggerLevel::debug, MappedLevel::debug},
						       {LoggerLevel::warning, MappedLevel::warn},
						       {LoggerLevel::error, MappedLevel::err},
						       {LoggerLevel::fatal, MappedLevel::critical}};
	MappedLevel result                          = MappedLevel::off;
	auto iterator                               = levelMap.find(level);
	if(iterator != levelMap.end( )) {
		result = iterator->second;
	}
	return result;
}
