#pragma once

#include <memory>

#pragma warning(push)
#pragma warning(disable : 26812)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)


// Just an Initial Test function
void PrintHello( );
// currently in global namespace
using MappedLevel = spdlog::level::level_enum;
enum class LoggerLevel { trace, debug, info, warning, error, fatal };
enum class LoggerInterface { internal, client };

class Logger
{
      public:
	Logger( );
	~Logger( );
	static void Init( );
	void SetLoggerLevel(LoggerLevel logLevel, LoggerInterface logInterface);

	static std::shared_ptr<spdlog::logger>& GetInternalLogger( )
	{
		return m_internalLogger;
	}
	static std::shared_ptr<spdlog::logger>& GetClientSideLogger( )
	{
		return m_clientLogger;
	}

      private:
	inline static MappedLevel MapLogLevel(LoggerLevel level);
	static std::shared_ptr<spdlog::logger> m_internalLogger;
	static std::shared_ptr<spdlog::logger> m_clientLogger;
	MappedLevel m_level {MappedLevel::off};
};
