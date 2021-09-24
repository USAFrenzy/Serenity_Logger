#include "LibLogger.h"

namespace serenity
{
	// Just So It Can Compile For Now.. -----------------------------------------------------------------------------------------
	static LoggerLevel GetGlobalLevel( )
	{
		return LoggerLevel::warning;
	}
	// Delete Above When Fleshed Out    -----------------------------------------------------------------------------------------

	std::shared_ptr<spdlog::logger> InternalLibLogger::m_internalLogger;

	InternalLibLogger::InternalLibLogger( internal_logger_info infoStruct )
	{
		m_sinks = std::make_unique<Sink>( );
		// If Default, Should Just Populate With Defaults, Otherwise, Move The Struct Parameter To The internalLoggerInfo
		// Variable
		CustomizeInternalLogger( infoStruct );
		CreateInternalLogger( );
	}

	// copy-paste from Logger.cpp Implementation
	std::string InternalLibLogger::LogLevelToStr( LoggerLevel level )
	{
		std::map<LoggerLevel, std::string> levelMap = { { LoggerLevel::trace, "[trace]" }, { LoggerLevel::info, "[info]" },
								{ LoggerLevel::debug, "[debug]" }, { LoggerLevel::warning, "[warn]" },
								{ LoggerLevel::error, "[error]" }, { LoggerLevel::fatal, "[fatal]" } };
		std::string                        result   = "[off]";
		auto                               iterator = levelMap.find( level );
		if( iterator != levelMap.end( ) ) {
			result = iterator->second;
		}
		return result;
	}

	void InternalLibLogger::CustomizeInternalLogger( internal_logger_info infoStruct )
	{
		internalLoggerInfo = std::move( infoStruct );
	}

	void InternalLibLogger::CreateInternalLogger( )
	{
		internalLoggerInfo.sink_info.sinks.clear( );
		internalLoggerInfo.sink_info.sinks.emplace_back( SinkType::stdout_color_mt );

		m_sinks->clear_sinks( );
		m_sinks->set_sinks( internalLoggerInfo.sink_info.sinks );
		m_sinks->sinkVector.clear( );

		// Should Probs Just Write A Conversion Function?
		logger_info tmp;
		tmp.loggerName = internalLoggerInfo.loggerName;
		tmp.level      = internalLoggerInfo.level;
		tmp.logDir     = internalLoggerInfo.logDir;
		tmp.logName    = internalLoggerInfo.logName;
		tmp.sink_info  = internalLoggerInfo.sink_info;
		m_sinks->CreateSink( tmp );


		if( m_internalLogger != nullptr ) {
			SE_INTERNAL_TRACE( "Sinks For Logger [{}] Have Been Succesfully Created", internalLoggerInfo.loggerName );
		}
		auto mappedLevel = ToMappedLevel( internalLoggerInfo.level );
		if( GetGlobalLevel( ) != internalLoggerInfo.level ) {
			mappedLevel = ToMappedLevel( GetGlobalLevel( ) );
		}

		m_internalLogger = std::make_shared<spdlog::logger>( internalLoggerInfo.loggerName, begin( m_sinks->sinkVector ),
								     end( m_sinks->sinkVector ) );
		spdlog::register_logger( m_internalLogger );
		SE_INTERNAL_TRACE( "Logger [{}] Has Been Successfully Created", internalLoggerInfo.loggerName );
		SE_INTERNAL_TRACE( "Logger [{}] Has Been Registered", internalLoggerInfo.loggerName );

		m_internalLogger->set_level( mappedLevel );
		SE_INTERNAL_TRACE( "Logger [{}] Level Has Been Set To: {}", internalLoggerInfo.loggerName,
				   LogLevelToStr( internalLoggerInfo.level ) );

		m_internalLogger->flush_on( mappedLevel );
		SE_INTERNAL_TRACE( "Logger [{}] Flush Level Has Been Set To: {}", internalLoggerInfo.loggerName,

				   LogLevelToStr( internalLoggerInfo.level ) );
		SE_INTERNAL_INFO( "Logger [{}] Successfully Initialized", internalLoggerInfo.loggerName );
	}

	void InternalLibLogger::EnableInternalLogging( )
	{
		loggingEnabled = true;
	}

	void InternalLibLogger::DisableInternalLogging( )
	{
		loggingEnabled = false;
	}

	bool InternalLibLogger::ShouldLog( )
	{
		return loggingEnabled;
	}

}  // namespace serenity
