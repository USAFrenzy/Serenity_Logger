#include "LibLogger.h"

namespace serenity
{
	using namespace se_internal;

	// Just So It Can Compile For Now.. -----------------------------------------------------------------------------------------
	static LoggerLevel GetGlobalLevel( )
	{
		return LoggerLevel::warning;
	}
	// Delete Above When Fleshed Out    -----------------------------------------------------------------------------------------

	std::shared_ptr<spdlog::logger> InternalLibLogger::m_internalLogger;
	internal_logger_info     InternalLibLogger::internalLoggerInfo = { };

	InternalLibLogger::InternalLibLogger( se_internal::internal_logger_info infoStruct )
	{
		m_sinks = std::make_unique<Sink>( );
		// If Default, Should Just Populate With Defaults, Otherwise, Move The Struct Parameter To The internalLoggerInfo
		// Variable
		CustomizeInternalLogger( infoStruct );
		CreateInternalLogger( );
	}

	void InternalLibLogger::SetLogLevel( LoggerLevel logLevel )
	{
		trace( "Setting Logger Level..." );
		auto m_level = ToMappedLevel( logLevel );
		if( m_level == MappedLevel::n_levels ) {
			warn( "Log Level Was Not A Valid Value - Log Level Set To [off]" );
			m_internalLogger->set_level( MappedLevel::off );
		}
		else {
			m_internalLogger->set_level( m_level );
			trace( "Log Level Successfully Set To: [{}]", LogLevelToStr( logLevel ) );
		}
	}

		void InternalLibLogger::UpdateInfo( ) 
		{ 
			if( internalCustomized ) {
				if( m_internalLogger != nullptr ) {
					spdlog::drop( m_internalLogger->name( ) );
				}
				m_sinks->clear_sinks( );
				m_internalLogger.reset( );
				CreateInternalLogger( );
			}
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
		internalLoggerInfo = infoStruct;
		internalCustomized = true;
	}

	void InternalLibLogger::CreateInternalLogger( )
	{
		m_sinks->clear_sinks( );
		m_sinks->set_sinks( internalLoggerInfo.sink_info.sinks );
		auto tmp = toLoggerInfo( internalLoggerInfo );
		m_sinks->CreateSink( tmp);

		if( m_internalLogger != nullptr ) {
			trace( "Sinks For Logger [{}] Have Been Succesfully Created", internalLoggerInfo.loggerName );
		}
		auto mappedLevel = ToMappedLevel( internalLoggerInfo.level );
		if( GetGlobalLevel( ) != internalLoggerInfo.level ) {
			mappedLevel = ToMappedLevel( GetGlobalLevel( ) );
		}

		m_internalLogger = std::make_shared<spdlog::logger>( internalLoggerInfo.loggerName, begin( m_sinks->sinkVector ),
								     end( m_sinks->sinkVector ) );
		spdlog::register_logger( m_internalLogger );
		trace( "Logger [{}] Has Been Successfully Created", internalLoggerInfo.loggerName );
		trace( "Logger [{}] Has Been Registered", internalLoggerInfo.loggerName );

		m_internalLogger->set_level( mappedLevel );
		trace( "Logger [{}] Level Has Been Set To: {}", internalLoggerInfo.loggerName,
				   LogLevelToStr( internalLoggerInfo.level ) );

		m_internalLogger->flush_on( mappedLevel );
		trace( "Logger [{}] Flush Level Has Been Set To: {}", internalLoggerInfo.loggerName,

				   LogLevelToStr( internalLoggerInfo.level ) );
		info( "Logger [{}] Successfully Initialized", internalLoggerInfo.loggerName );
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
		return ( ToLogLevel( InternalLogger( )->level( ) ) <= GetGlobalLevel( ) && ( loggingEnabled ) ) ? true : false;
	}

}  // namespace serenity
