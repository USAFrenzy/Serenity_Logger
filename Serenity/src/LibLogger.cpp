
#include <serenity/Helpers/LibLogger.h>
#include <serenity/Sinks/Sinks.h>
#include <serenity/Logger.h>

namespace serenity
{
	std::shared_ptr<spdlog::logger> InternalLibLogger::m_internalLogger;
	bool                            InternalLibLogger::loggingEnabled { false };


	InternalLibLogger::InternalLibLogger( sinks::internal_logger_info infoStruct )
	{
		m_sinks = std::make_unique<sinks::Sink>( );
		// If Default, Should Just Populate With Defaults, Otherwise, Move The Struct Parameter To The internalLoggerInfo
		// Variable
		CustomizeInternalLogger( infoStruct );
	}
	InternalLibLogger::InternalLibLogger( const InternalLibLogger &copy )
	{
		internalLoggerInfo = copy.internalLoggerInfo;
		loggingEnabled     = copy.loggingEnabled;
		internalCustomized = copy.internalCustomized;
		m_internalLogger   = copy.m_internalLogger;
		m_sinks            = copy.m_sinks;
	}

	const std::shared_ptr<sinks::Sink> InternalLibLogger::SinkInfo( )
	{
		return m_sinks;
	}

	const std::string InternalLibLogger::GetName( )
	{
		return internalLoggerInfo.loggerName;
	}
	const sinks::internal_logger_info InternalLibLogger::InternalInfo( )
	{
		return internalLoggerInfo;
	}


	void InternalLibLogger::SetLogLevel( LoggerLevel logLevel )
	{
		trace( "Setting Logger Level..." );
		if( se_utils::ToMappedLevel( logLevel ) == MappedLevel::n_levels ) {
			internalLoggerInfo.level = LoggerLevel::off;
			m_internalLogger->set_level( se_utils::ToMappedLevel( internalLoggerInfo.level ) );
			warn( "Log Level Was Not A Valid Value - Log Level Set To {}", LogLevelToStr( internalLoggerInfo.level ) );
		}
		internalLoggerInfo.level = logLevel;
		m_internalLogger->set_level( se_utils::ToMappedLevel( internalLoggerInfo.level ) );
		trace( "Log Level Successfully Set To: {}", LogLevelToStr( logLevel ) );
	}

	void InternalLibLogger::SetFlushLevel( LoggerLevel flushLevel )
	{
		trace( "Setting Logger Flush Level..." );
		if( se_utils::ToMappedLevel( flushLevel ) == MappedLevel::n_levels ) {
			internalLoggerInfo.flushLevel = LoggerLevel::trace;
			m_internalLogger->flush_on( se_utils::ToMappedLevel( internalLoggerInfo.level ) );
			warn( "Log Level Was Not A Valid Value - Log Flush Level Set To {}", LogLevelToStr( internalLoggerInfo.level ) );
		}
		else {
			internalLoggerInfo.flushLevel = flushLevel;
			m_internalLogger->flush_on( se_utils::ToMappedLevel( internalLoggerInfo.flushLevel ) );
			trace( "Log Flush Level Successfully Set To: {}", LogLevelToStr( flushLevel ) );
		}
	}


	void InternalLibLogger::UpdateInfo( )
	{
		if( internalCustomized ) {
			if( m_internalLogger != nullptr ) {
				m_internalLogger->trace( "Replacing Old Logger With New Options..." );
				spdlog::drop( m_internalLogger->name( ) );
			}
			if( m_sinks != nullptr ) {
				m_sinks->ClearSinks( );
			}
			if( m_internalLogger != nullptr ) {
				m_internalLogger.reset( );
			}
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

	void InternalLibLogger::CustomizeInternalLogger( sinks::internal_logger_info &infoStruct )
	{
		if( ShouldLog( ) ) {
			m_internalLogger->trace( "Clearing Sinks..." );
		}
		m_sinks->GetSinkHandles( ).clear( );
		if( ShouldLog( ) ) {
			m_internalLogger->info( "Sinks Successfully Cleared" );
		}
		if( m_internalLogger != nullptr ) {
			m_internalLogger->trace( "Setting New Options For Internal Logger..." );
		}
		// override formatStr with internalFormatStr For CreateSink() called later on
		infoStruct.sink_info.formatStr = infoStruct.internalFormatStr;
		internalLoggerInfo             = std::move( infoStruct );
		if( ShouldLog( ) ) {
			m_internalLogger->info( "New Options Successfully Set" );
		}
		internalCustomized = true;
		UpdateInfo( );
	}

	void InternalLibLogger::CreateInternalLogger( )
	{
		m_sinks->ClearSinks( );
		if( internalLoggerInfo.sink_info.sinks.empty( ) ) {
			internalLoggerInfo.sink_info.sinks.emplace_back( sinks::SinkType::stdout_color_mt );
		}
		m_sinks->SetSinks( internalLoggerInfo.sink_info.sinks );
		m_sinks->CreateSink( internalLoggerInfo.sink_info );
		auto sinks       = m_sinks->GetSinkHandles( );
		m_internalLogger = std::make_shared<spdlog::logger>( internalLoggerInfo.loggerName, begin( sinks ), end( sinks ) );
		spdlog::register_logger( m_internalLogger );
		SetLogLevel( internalLoggerInfo.level );
		trace( "Logger [{}] Has Been Registered", internalLoggerInfo.loggerName );
		SetFlushLevel( internalLoggerInfo.flushLevel );
		info( "Logger [{}] Successfully Initialized", internalLoggerInfo.loggerName );
	}

	bool InternalLibLogger::ShouldLog( )
	{
		if( m_internalLogger != nullptr ) {
			return ( ( se_globals::GetGlobalLevel( ) <= se_utils::ToLogLevel( InternalLogger( )->level( ) ) ) &&
				 ( loggingEnabled ) )
			       ? true
			       : false;
		}
		else {
			return false;
		}
	}

	sinks::internal_logger_info::internal_logger_info( )
	{
		sink_info.base_info.flushLevel = flushLevel;
		sink_info.base_info.level      = level;
		sink_info.base_info.logDir     = logDir;
		sink_info.base_info.loggerName = loggerName;
		sink_info.base_info.logName    = logName;
	}

}  // namespace serenity
