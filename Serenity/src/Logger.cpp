
#include "serenity/Logger.hpp"

#include <map>
#include <string>

#pragma warning(push, 0)
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

// Just A test function
void PrintHello( )
{
	const char* str = "Library!";
	printf("Hello From The %s\n", str);
}


std::shared_ptr<spdlog::logger> Logger::m_internalLogger;
std::shared_ptr<spdlog::logger> Logger::m_clientLogger;

Logger::Logger(std::string loggerName) 
	:m_loggerName(loggerName)
{ 
}

Logger::~Logger( ) {
	
	spdlog::shutdown();
 }

namespace serenity {
	std::string GetSerenityVerStr( ) 
	{
		auto version = VERSION_NUMBER(SERENITY_MAJOR, SERENITY_MINOR, SERENITY_REV);
		return version;
	}
} // namespace serenity


// Heavily Influenced by Chernos Hazel
void Logger::Init(std::string fileName, LoggerLevel setLevel)
{
	// Keeping this pretty simple before deviating too hard core - end goal would be to abstract some of the
	// setup in some structs and then just pass in the desired struct into the init function
	auto mappedLevel = MapLogLevel(setLevel);
	std::vector<spdlog::sink_ptr> sinks;

	sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>( ));
	sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName, true));
	// Would Like To Format this in a more personalized and absstracted manner
	sinks [0]->set_pattern("%^[%T] %n: %v%$");
	sinks [1]->set_pattern("[%T] [%l] %n: %v");

	// For both logger types, would like to abstract away the mappedLevel in a way that a flush level doesnt
	// always have to be the same as what is set
	m_internalLogger = std::make_shared<spdlog::logger>("INTERNAL", begin(sinks), end(sinks));
	spdlog::register_logger(m_internalLogger);
	m_internalLogger->set_level(mappedLevel);
	m_internalLogger->flush_on(mappedLevel);

	m_clientLogger = std::make_shared<spdlog::logger>("CLIENT", begin(sinks), end(sinks));
	spdlog::register_logger(m_clientLogger);
	m_clientLogger->set_level(mappedLevel);
	m_clientLogger->flush_on(mappedLevel);
}

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

MappedLevel Logger::MapLogLevel(LoggerLevel level)
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
