#pragma once

#include "serenity/Common.hpp"

namespace serenity
{
	/*
		Will honestly need to think about this particular class a lot. Need to be sure that it's
	   functionality won't be broken by adding the variability I want to Logger class as well as correctly
	   update the Logger class for any changes made under these fields
	*/
	class LoggerInfo
	{
	      public:
		LoggerInfo( std::string name, std::string pattern, LoggerLevel logLevel, LoggerLevel flushLevel );
		~LoggerInfo( );

		// General Setter/Getter Helper Functions
		void        SetLoggerName( std::string loggerName );
		std::string GetLoggerName( );
		void        SetLoggerPattern( std::string logPattern );
		void        SetLoggerLevel( LoggerLevel logLevel );
		void        LoggerFlushOn( LoggerLevel flushLevel );

		/*std::optional<LoggerInterface>
		  SetLoggerName(std::string loggerName, std::optional<LoggerInterface> interface);*/

	      private:
		std::string m_loggerName;
		std::string m_pattern;
		LoggerLevel m_logLevel;
		LoggerLevel m_flushLevel;

	      private:
		LoggerLevel GetLoggerFlushLevel( );
		LoggerLevel GetLoggerLevel( );
		std::string GetLoggerPattern( );

	};  // class LoggerInfo

	LoggerInfo::LoggerInfo( std::string name, std::string pattern, LoggerLevel logLevel, LoggerLevel flushLevel )
	  : m_loggerName( name ), m_pattern( pattern ), m_logLevel( logLevel ), m_flushLevel( flushLevel )
	{
	}
	LoggerInfo::~LoggerInfo( ) { }

	inline void LoggerInfo::SetLoggerName( std::string loggerName )
	{
		m_loggerName = loggerName;
	}
	std::string LoggerInfo::GetLoggerName( )
	{
		return m_loggerName;
	}

	inline void LoggerInfo::SetLoggerPattern( std::string logPattern )
	{
		m_pattern = logPattern;
	}

	std::string LoggerInfo::GetLoggerPattern( )
	{
		return m_pattern;
	}

	inline void LoggerInfo::SetLoggerLevel( LoggerLevel logLevel )
	{
		m_logLevel = logLevel;
	}

	LoggerLevel LoggerInfo::GetLoggerLevel( )
	{
		return m_logLevel;
	}

	inline void LoggerInfo::LoggerFlushOn( LoggerLevel flushLevel )
	{
		m_flushLevel = flushLevel;
	}

	LoggerLevel LoggerInfo::GetLoggerFlushLevel( )
	{
		return m_flushLevel;
	}


	// ########################################################################################################
	/*
		Default Behavior Without Optional Interface Arg Is To Set The Client Logger Name.
		Optional Interface Arg Can Be Either: 'internal' Or 'client'
	*/
	/*std::optional<LoggerInterface>
	  LoggerInfo::SetLoggerName(std::string loggerName, std::optional<LoggerInterface> interface)
	{
		if(!interface.has_value( )) {
			m_clientName = loggerName;
		} else {
			switch(interface.value( )) {
				case LoggerInterface::internal:
					{
						m_internalName = loggerName;
					}
					break;
				case LoggerInterface::client:
					{
						m_clientName = loggerName;
					}
					break;
			}
		}
		return SE_NULL_OPTION;
	}*/
	// ########################################################################################################


	struct LogSinkInfo
	{
	};

}  // namespace serenity

// clang-format off
/*
	just had an idea: 
	- make the LogFileInfo struct a class instead and in the constructor take in the path to the log dir and the log file name [X]
	- in the initial setup in the constructor, separate the log dir path into its component paths
		- this way i can set a relative path that will be easier to reference throughout
	- set the member variables in respect to those component path values and set the log name to its respective variable
	- implement the setters/getters needed as helper functions in a helper namespace so as not to cloud the LogFileInfo class
	- Add Basic Functions for what would be expected in a normal file system handler

*/
// clang-format on
