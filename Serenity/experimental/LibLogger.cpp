#include "LibLogger.h"

#include <serenity/Logger.h>  // For The GetGlobal()

namespace serenity
{
	using namespace se_internal;

	std::shared_ptr<spdlog::logger> InternalLibLogger::m_internalLogger;
	bool                            InternalLibLogger::loggingEnabled { false };


	InternalLibLogger::InternalLibLogger( se_internal::internal_logger_info infoStruct )
	{
		m_sinks = std::make_unique<Sink>( );
		// If Default, Should Just Populate With Defaults, Otherwise, Move The Struct Parameter To The internalLoggerInfo
		// Variable
		CustomizeInternalLogger( infoStruct );
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
		m_sinks->sinkVector.clear( );
		internalLoggerInfo = std::move( infoStruct );
		internalCustomized = true;
		UpdateInfo( );
	}

	void InternalLibLogger::CreateInternalLogger( )
	{
		m_sinks->clear_sinks( );
		if( internalLoggerInfo.sink_info.sinks.empty( ) ) {
			internalLoggerInfo.sink_info.sinks.emplace_back( SinkType::stdout_color_mt );
		}
		m_sinks->set_sinks( internalLoggerInfo.sink_info.sinks );
		auto tmp = toLoggerInfo( internalLoggerInfo );
		m_sinks->CreateSink( tmp );

		auto mappedLevel = ToMappedLevel( internalLoggerInfo.level );
		if( GetGlobalLevel( ) != internalLoggerInfo.level ) {
			mappedLevel = ToMappedLevel( GetGlobalLevel( ) );
		}
		m_internalLogger = std::make_shared<spdlog::logger>( internalLoggerInfo.loggerName, begin( m_sinks->sinkVector ),
								     end( m_sinks->sinkVector ) );
		spdlog::register_logger( m_internalLogger );
		info( "Logger [{}] Has Been Successfully Created", internalLoggerInfo.loggerName );

		m_internalLogger->set_level( mappedLevel );
		trace( "Logger [{}] Level Has Been Set To: {}", internalLoggerInfo.loggerName, LogLevelToStr( internalLoggerInfo.level ) );

		trace( "Logger [{}] Has Been Registered", internalLoggerInfo.loggerName );


		m_internalLogger->flush_on( mappedLevel );
		trace( "Logger [{}] Flush Level Has Been Set To: {}", internalLoggerInfo.loggerName,

		       LogLevelToStr( internalLoggerInfo.level ) );
		info( "Logger [{}] Successfully Initialized", internalLoggerInfo.loggerName );
	}

	bool InternalLibLogger::ShouldLog( )
	{
		return ( ( GetGlobalLevel( ) <= ToLogLevel( InternalLogger( )->level( ) ) ) && ( loggingEnabled ) ) ? true : false;
	}

}  // namespace serenity
