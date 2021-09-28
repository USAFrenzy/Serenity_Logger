
#include <serenity/Logger.h>
#include <serenity/Utilities/Utilities.h>

#include <fstream>

#pragma warning( push, 0 )
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning( pop )

namespace serenity
{
	std::shared_ptr<spdlog::logger>    Logger::m_clientLogger;
	std::unique_ptr<LogFileHelper>     Logger::logFileHandle;
	std::shared_ptr<InternalLibLogger> Logger::internalLogger;

	std::string Logger::LogLevelToStr( LoggerLevel level )
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


	Logger::Logger( logger_info &infoStruct ) : initInfo( std::move( infoStruct ) )
	{
		file_helper::path logDirPath = initInfo.logDir.path( );
		auto              filePath   = logDirPath.string( ) + "\\" + initInfo.logName;

		internalLogger = std::make_shared<InternalLibLogger>( internalLoggerInfo );
		logFileHandle  = std::make_unique<LogFileHelper>( filePath );
		m_sinks        = std::make_unique<Sink>( );
		//##########################################################################################################
		/*
		  Some possible features to add:
		  - The ability to enable internal logging support independantly of client logger [X]
		  - Ties in with the above point, but also have the ability to turn off internal logging when in release [X]
		  - The ability to customize the internal logger? I mean, I'm using stdout sink here, but hey, who knows? [ ]
		*/
		//##########################################################################################################
		// Creating Client Logger
		m_sinks->clear_sinks( );
		m_sinks->set_sinks( initInfo.sink_info.sinks );
		CreateLogger( initInfo );
		internalLogger->trace( "Logger [{}] Successfully Initialized", initInfo.loggerName );
	}

	Logger::~Logger( )
	{
		Shutdown( );
	}

	void Logger::CloseLog( const file_helper::path filePath )
	{
		internalLogger->trace( "Closing File [{}]...", filePath.filename( ) );
		FileHelperHandle( )->CloseFile( filePath );
		internalLogger->trace( "File [{}] Successfully Closed", filePath.filename( ) );
	}

	void Logger::OpenLog( const file_helper::path filePath )
	{
		internalLogger->trace( "Opening File [{}]...", filePath.filename( ) );
		FileHelperHandle( )->OpenFile( filePath );
		internalLogger->trace( "File [{}] Successfully Opened", filePath.filename( ) );
	}


	void Logger::StopLogger( )
	{
		internalLogger->trace( "Stopping Logger..." );
		CloseLog( FileHelperHandle( )->LogFilePath( ) );
		DropLogger( );  // Drop client/internal loggers w/o shutting spdlog down
		internalLogger->info( "Logger Has Been Successfully Stopped" );
	}

	void Logger::StartLogger( )
	{
		internalLogger->trace( "Starting Logger(s)..." );
		// DropLogger() resets m_sinks ptr -> have to recreate sink handle for CreateLogger()
		if( m_sinks == nullptr ) {
			m_sinks = std::make_unique<Sink>( );
			m_sinks->clear_sinks( );
			internalLogger->trace( "Sinks Handle Has Been Successfully Re-Initialized" );
		}
		if( internalLogger->InternalLogger( ) == nullptr ) {
			// CustomizeInternalLogger() sets up the struct fields, resets the pointer, creates the new sink(s), creates
			// the new logger, and registers that logger with the pointer pointing to that logger
			internalLogger->CustomizeInternalLogger( internalLoggerInfo );
			internalLogger->trace( "Internal Logger Has Been Successfully Re-Initialized" );
		}
		if( m_clientLogger == nullptr ) {
			m_sinks->set_sinks( initInfo.sink_info.sinks );
			CreateLogger( initInfo );
			internalLogger->trace( "Client Logger Has Been Successfully Re-Initialized" );
		}
		OpenLog( FileHelperHandle( )->LogFilePath( ) );
		internalLogger->info( "Logger Has Been Successfully Restarted" );
	}

	void Logger::Shutdown( )
	{
		StopLogger( );

		internalLogger->trace( "Shutting Down..." );
		spdlog::drop_all( );
		spdlog::shutdown( );
		m_sinks.reset( );
		m_clientLogger.reset( );
		internalLogger.reset( );
	}

	void Logger::DropLogger( )
	{

		internalLogger->trace( "Dropping Logger And Resetting Handles...", m_clientLogger->name( ) );
		spdlog::drop( m_clientLogger->name( ) );
		internalLogger->trace( "Logger [{}] Has Been Dropped", m_clientLogger->name( ) );
		m_sinks.reset( );
		internalLogger->trace( "Sinks Handle For Logger Has Been Reset" );
		m_clientLogger.reset( );
		internalLogger->trace( "Logger Handle Has Been Reset" );
	}

	void Logger::CreateLogger( logger_info &infoStruct )
	{
		m_sinks->sinkVector.clear( );
		m_sinks->CreateSink( infoStruct );
		internalLogger->trace( "Sinks For Logger [{}] Have Been Succesfully Created", infoStruct.loggerName );
		m_clientLogger =
		  std::make_shared<spdlog::logger>( infoStruct.loggerName, begin( m_sinks->sinkVector ), end( m_sinks->sinkVector ) );
		internalLogger->trace( "Logger [{}] Has Been Successfully Created", infoStruct.loggerName );
		spdlog::register_logger( m_clientLogger );
		internalLogger->trace( "Logger [{}] Has Been Registered", infoStruct.loggerName );
		SetLogLevel( initInfo.level );
		SetFlushLevel( initInfo.flushLevel );
	}

	bool Logger::RenameLog( const std::string newName, bool replaceIfExists )
	{
		// In Case Path Is Passed In
		file_helper::path newFilePath = newName;
		const auto &      tmpPath     = initInfo.logDir.path( );
		file_helper::path oldPath     = tmpPath.string( ).append( "\\" + initInfo.logName );
		file_helper::path newPath     = tmpPath.string( ).append( "\\" + newFilePath.filename( ).string( ) );
		internalLogger->trace( "Renaming Log File [{}] To [{}]", oldPath.filename( ), newPath.filename( ) );
		try {
			// Flush and close logs, Drop Loggers From spdlog's registry so sinks
			// ref to files are dropped, and reset the logger pointers
			StopLogger( );
			if( file_helper::exists( newPath ) ) {
				internalLogger->warn( "File [{}] Already Exists", newPath.filename( ) );
				if( replaceIfExists ) {
					internalLogger->trace( "Parameter \"replaceIfExists\" = true." );
					internalLogger->trace( "Removing [{}] Before Renaming [{}] To [{}].", newPath.filename( ),
							       oldPath.filename( ), newPath.filename( ) );
					file_utils::RemoveEntry( newPath );
					internalLogger->trace( "Removed File [{}]", newPath.filename( ) );
				}
				else {
					internalLogger->trace( "Parameter \"replaceIfExists\" = false." );
					internalLogger->trace( "Copying File Contents: \n- From:\t[{}] \n- To:\t[{}]", oldPath, newPath );
					file_utils::CopyContents( oldPath, newPath );
					internalLogger->trace( "Copying Contents Finished. Removing File [{}]", oldPath.filename( ) );
					file_utils::RemoveEntry( oldPath );
					internalLogger->trace( "Removed File Link From [{}]", oldPath.filename( ) );
				}
			}
			file_utils::RenameFile( oldPath, newPath );
			internalLogger->info( "Renaming [{}] To [{}] Was Successful.", oldPath.filename( ), newPath.filename( ) );
			// Effectively Updating Variables
			initInfo.logName = newPath.filename( ).string( );
			internalLogger->trace( "Log File Name Has Been Updated To Reflect Rename To [{}]", newPath.filename( ) );
			internalLogger->trace( "File Info Updating Based On Path:{}", newPath );
			FileHelperHandle( )->UpdateFileInfo( newPath );
			UpdateInfo( );
			internalLogger->info( "Logger Has Been Updated For File Path Changes" );
			StartLogger( );
			return true;
		}
		catch( const std::exception &e ) {
			internalLogger->error( "Renaming Failed." );
			internalLogger->error( "EXCEPTION CAUGHT IN RenameLog():\n{}\n", e.what( ) );
			return false;
		}
	}

	bool Logger::WriteToNewLog( const std::string newLogName )
	{
		const auto &            tmpPath     = initInfo.logDir.path( );
		file_helper::path       oldPath     = tmpPath.string( ).append( "\\" + initInfo.logName );
		const file_helper::path newFilePath = tmpPath.string( ).append( "\\" + newLogName );
		try {
			StopLogger( );
			internalLogger->trace( "Setting New Log Path To Point To [{}]", newFilePath );
			FileHelperHandle( )->StorePathComponents( newFilePath );
			UpdateInfo( );
			StartLogger( );
			internalLogger->info( "Successfully Changed To New Log [{}]", initInfo.logName );
		}
		catch( const std::exception &e ) {
			internalLogger->fatal( "Failed Change To New Log [{}] Due To:\n{}", newLogName, e.what( ) );
			return false;
		}
		return true;
	}

	void Logger::UpdateInfo( )
	{
		internalLogger->trace( "Checking For File Info Changes..." );
		if( !FileHelperHandle( )->fileInfoChanged ) {
			internalLogger->info( "File Info Already Up-To-Date" );
		}
		else {
			internalLogger->trace( "File Info Has Changed. Updating File Info" );
			initInfo.logDir = FileHelperHandle( )->LogDir( );
			internalLogger->trace( "Logger's Log Directory Set To [{}]", initInfo.logDir.path( ).filename( ) );
			initInfo.logName = FileHelperHandle( )->LogName( );
			internalLogger->trace( "Logger's Log Name Set To [{}]", initInfo.logName );
			internalLogger->info( "File Info Has Been Updated" );
			FileHelperHandle( )->fileInfoChanged = false;
		}
	}

	std::string const Logger::LoggerName( )
	{
		return initInfo.loggerName;
	}

	const std::string Logger::LogName( )
	{
		return initInfo.logName;
	}

	const LoggerLevel Logger::GetLogLevel( )
	{
		return ToLogLevel( m_clientLogger->level( ) );
	}

	void Logger::SetLogLevel( LoggerLevel level )
	{
		internalLogger->trace( "Setting Logger Level..." );
		if( ToMappedLevel( level ) == MappedLevel::n_levels ) {
			initInfo.level = LoggerLevel::off;
			internalLogger->warn( "Log Level Was Not A Valid Value - Log Level Set To {}", LogLevelToStr( initInfo.level ) );
			m_clientLogger->set_level( ToMappedLevel( initInfo.level ) );
		}
		initInfo.level = level;
		m_clientLogger->set_level( ToMappedLevel( level ) );
		internalLogger->trace( "Log Level Successfully Set To: {}", LogLevelToStr( initInfo.level ) );
	}

	void Logger::SetFlushLevel( LoggerLevel flushLevel )
	{
		internalLogger->trace( "Setting Logger Flush Level..." );
		if( ToMappedLevel( flushLevel ) == MappedLevel::n_levels ) {
			initInfo.flushLevel = LoggerLevel::trace;
			m_clientLogger->flush_on( ToMappedLevel( initInfo.flushLevel ) );
			internalLogger->warn( "Log Level Was Not A Valid Value - Logger Flush Level Set To {}",
					      LogLevelToStr( initInfo.flushLevel ) );
		}
		else {
			initInfo.flushLevel = flushLevel;
			m_clientLogger->flush_on( ToMappedLevel( initInfo.flushLevel ) );
			internalLogger->trace( "Log Flush Level Successfully Set To: {}", LogLevelToStr( flushLevel ) );
		}
	}

	const LoggerLevel Logger::GetFlushLevel( )
	{
		return ToLogLevel( m_clientLogger->flush_level( ) );
	}

	bool Logger::ShouldLog( )
	{
		if( m_clientLogger != nullptr ) {
			return ( GetGlobalLevel( ) <= GetLogLevel( ) ) ? true : false;
		}
		else {
			return false;
		}
	}

	void Logger::ChangeInternalLoggerOptions( se_internal::internal_logger_info options )
	{
		internalLogger->trace( "Updating Internal Logger Options" );
		internalLogger->CustomizeInternalLogger( options );
		internalLogger->info( "Successfully Updated Internal Logger Options" );
	}

	// MISC Functions
	void SetGlobalLevel( LoggerLevel level )
	{
		global_level = level;
		if( Logger::ClientSideLogger( ) != nullptr ) {
			Logger::ClientSideLogger( )->set_level( ToMappedLevel( level ) );
		}
		if( InternalLibLogger::InternalLogger( ) != nullptr ) {
			InternalLibLogger::InternalLogger( )->set_level( ToMappedLevel( level ) );
		}
	}
}  // namespace serenity
