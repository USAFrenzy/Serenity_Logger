
#include "serenity/initialTest.hpp"

#include <map>
#include <string>
// Just A test function
void PrintHello( )
{
	const char* str = "Library!";
	printf("Hello From The %s\n", str);
}

std::shared_ptr<spdlog::logger> Logger::m_internalLogger;
std::shared_ptr<spdlog::logger> Logger::m_clientLogger;
Logger::Logger( ) { }
Logger::~Logger( ) { }

void Logger::Init( ) { }

void Logger::SetLoggerLevel(LoggerLevel level, LoggerInterface logInterface)
{
	m_level = MapLogLevel(level);
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
				m_internalLogger->set_level(spdlog::level::level_enum::off);
				m_clientLogger->set_level(spdlog::level::level_enum::off);
				throw std::runtime_error("Log Level Was Not A Valid Value - Log Level Set To "
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
