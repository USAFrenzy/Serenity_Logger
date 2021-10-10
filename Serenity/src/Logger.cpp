
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
	LoggerLevel                        Logger::global_level { LoggerLevel::trace };

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


	Logger::Logger( sinks::base_sink_info infoStruct ) : initInfo( std::move( infoStruct ) )
	{
		file_helper::path logDirPath = initInfo.base_info.logDir.path( );
		file_helper::path filePath   = logDirPath.string( ) + "/" + initInfo.base_info.logName;
		filePath.make_preferred( );

		if( internalLogger == nullptr ) {
		internalLogger = std::make_shared<InternalLibLogger>( internalLoggerInfo );
		}
		if( logFileHandle == nullptr ) {
		logFileHandle  = std::make_unique<LogFileHelper>( filePath );
		}
		m_sinks        = std::make_unique<sinks::Sink>( );
		// Creating Client Logger
		m_sinks->ClearSinks( );
		m_sinks->SetSinks( initInfo.sinks );
		CreateLogger( initInfo );
		internalLogger->trace( "Logger [{}] Successfully Initialized", initInfo.base_info.loggerName );
	}

	Logger::~Logger( )
	{
		if( m_clientLogger != nullptr ) {
			Shutdown( );
		}
	}

	void Logger::CloseLog( const file_helper::path filePath )
	{
		auto prefPath = filePath;
		prefPath.make_preferred( );

		internalLogger->trace( "Closing File [{}]...", prefPath.filename( ) );
		FileHelperHandle( )->CloseFile( prefPath );
		internalLogger->trace( "File [{}] Successfully Closed", prefPath.filename( ) );
	}

	void Logger::OpenLog( const file_helper::path filePath, bool truncate )
	{
		auto prefPath = filePath;
		prefPath.make_preferred( );

		internalLogger->trace( "Opening File [{}]...", prefPath.filename( ) );
		FileHelperHandle( )->OpenFile( prefPath, truncate );
		internalLogger->trace( "File [{}] Successfully Opened", prefPath.filename( ) );
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
			m_sinks = std::make_unique<sinks::Sink>( );
			m_sinks->ClearSinks( );
			internalLogger->trace( "Sinks Handle Has Been Successfully Re-Initialized" );
		}
		if( internalLogger->InternalLogger( ) == nullptr ) {
			// CustomizeInternalLogger() sets up the struct fields, resets the pointer, creates the new sink(s), creates
			// the new logger, and registers that logger with the pointer pointing to that logger
			internalLogger->CustomizeInternalLogger( internalLoggerInfo );
			internalLogger->trace( "Internal Logger Has Been Successfully Re-Initialized" );
		}
		if( m_clientLogger == nullptr ) {
			m_sinks->SetSinks( initInfo.sinks );
			CreateLogger( initInfo );
			internalLogger->trace( "Client Logger Has Been Successfully Re-Initialized" );
		}
		OpenLog( FileHelperHandle( )->LogFilePath( ), false );
		internalLogger->info( "Logger Has Been Successfully Restarted" );
	}

	void Logger::Shutdown( )
	{
		StopLogger( );
		if( InternalLogger != nullptr ) {
			internalLogger->trace( "Shutting Down..." );
		}
		spdlog::drop_all( );
		spdlog::shutdown( );
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

	void Logger::CreateLogger( sinks::base_sink_info &infoStruct )
	{
		m_sinks->GetSinkHandles( ).clear( );
		m_sinks->CreateSink( infoStruct );
		internalLogger->trace( "Sinks For Logger [{}] Have Been Succesfully Created", infoStruct.base_info.loggerName );
		auto sinks     = m_sinks->GetSinkHandles( );
		m_clientLogger = std::make_shared<spdlog::logger>( infoStruct.base_info.loggerName, begin( sinks ), end( sinks ) );
		internalLogger->trace( "Logger [{}] Has Been Successfully Created", infoStruct.base_info.loggerName );
		spdlog::register_logger( m_clientLogger );
		internalLogger->trace( "Logger [{}] Has Been Registered", infoStruct.base_info.loggerName );
		SetLogLevel( initInfo.base_info.level );
		SetFlushLevel( initInfo.base_info.flushLevel );
	}

	const std::shared_ptr<spdlog::logger> &Logger::ClientSideLogger( )
	{
		return m_clientLogger;
	}

	const std::unique_ptr<LogFileHelper> &Logger::FileHelperHandle( )
	{
		return logFileHandle;
	}

	const std::shared_ptr<InternalLibLogger> &Logger::InternalLogger( )
	{
		return internalLogger;
	}

	const LoggerLevel &Logger::GetGlobalLevel( )
	{
		return global_level;
	}

	void Logger::SetGlobalLevel( LoggerLevel level )
	{
		global_level = level;
		if( Logger::ClientSideLogger( ) != nullptr ) {
			Logger::ClientSideLogger( )->set_level( se_utils::ToMappedLevel( level ) );
		}
		if( InternalLibLogger::InternalLogger( ) != nullptr ) {
			InternalLibLogger::InternalLogger( )->set_level( se_utils::ToMappedLevel( level ) );
		}
	}

	bool Logger::RenameLog( const std::string newName, bool overwriteIfExists )
	{
		// In Case Path Is Passed In
		file_helper::path paramPath = newName;
		const auto &      tmpPath   = initInfo.base_info.logDir.path( );
		file_helper::path oldPath   = tmpPath.string( ).append( "/" + initInfo.base_info.logName );
		oldPath.make_preferred( );
		file_helper::path newPath = tmpPath.string( ).append( "/" + paramPath.filename( ).string( ) );
		newPath.make_preferred( );

		internalLogger->trace( "Renaming Log File [{}] To [{}]", oldPath.filename( ), newPath.filename( ) );
		try {
			// Flush and close logs, Drop Loggers From spdlog's registry so sinks
			// ref to files are dropped, and reset the logger pointers
			StopLogger( );
			if( file_helper::exists( newPath ) ) {
				internalLogger->warn( "File [{}] Already Exists", newPath.filename( ) );
				if( overwriteIfExists ) {
					internalLogger->trace( "Parameter \"overwriteIfExists\" = true." );
					internalLogger->trace( "Removing [{}] Before Renaming [{}] To [{}].", newPath.filename( ),
							       oldPath.filename( ), newPath.filename( ) );
					file_utils::RemoveEntry( newPath );
					internalLogger->trace( "Removed File [{}]", newPath.filename( ) );
				}
				else {
					internalLogger->trace( "Parameter \"overwriteIfExists\" = false." );
					internalLogger->trace( "Copying File Contents: \n- From:\t[{}] \n- To:\t[{}]", oldPath, newPath );
					file_utils::CopyContents( oldPath, newPath );
					internalLogger->trace( "Copying Contents Finished. Removing File [{}]", oldPath.filename( ) );
					file_utils::RemoveEntry( oldPath );
					internalLogger->trace( "Removed File Link From [{}]", oldPath.filename( ) );
				}
			}
			file_utils::RenameFile( oldPath, newPath );
			if( overwriteIfExists ) {
				internalLogger->info( "Renaming [{0}] To [{1}], By First Removing [{0}], Was Successful",
						      oldPath.filename( ), newPath.filename( ) );
			}
			else {
				internalLogger->info(
				  "Renaming [{0}] To [{1}], By First Copying Contents From [{0}] To [{1}] And Then Removing [{0}], "
				  "Was Successful",
				  oldPath.filename( ), newPath.filename( ) );
			}
			// Effectively Updating Variables
			initInfo.base_info.logName = newPath.filename( ).string( );
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

	bool Logger::WriteToNewLog( std::string newLogName, bool truncateIfExists )
	{
		StopLogger( );
		// if path with leading dir separators passed in, remove them and standardize path
		if( ( newLogName.front( ) == ( '/' ) ) || ( newLogName.front( ) == ( '\\' ) ) ) {
			newLogName.erase( newLogName.begin( ), newLogName.begin( ) + 1 );
		}
		const auto &      tmpPath     = initInfo.base_info.logDir.path( );
		file_helper::path newPath     = tmpPath.string( ).append( "/" + newLogName );
		auto              newFilePath = newPath.make_preferred( );
		FileHelperHandle( )->CloseFile( newFilePath ); // ensure that new file is closed
		try {
			internalLogger->trace( "Setting New Log Path To Point To [{}]", newFilePath );
			FileHelperHandle( )->StorePathComponents( newFilePath );
			UpdateInfo( );
			if( std::filesystem::exists( newFilePath ) && truncateIfExists ) {
				FileHelperHandle( )->OpenFile( newFilePath, truncateIfExists );
				FileHelperHandle( )->CloseFile( newFilePath );
			}
			StartLogger( );
			internalLogger->info( "Successfully Changed To New Log [{}]", initInfo.base_info.logName );
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
			initInfo.base_info.logDir = FileHelperHandle( )->LogDir( );
			internalLogger->trace( "Logger's Log Directory Set To [{}]", initInfo.base_info.logDir.path( ).filename( ) );
			initInfo.base_info.logName = FileHelperHandle( )->LogName( );
			internalLogger->trace( "Logger's Log Name Set To [{}]", initInfo.base_info.logName );
			internalLogger->info( "File Info Has Been Updated" );
			FileHelperHandle( )->fileInfoChanged = false;
		}
	}

	std::string const Logger::LoggerName( )
	{
		return initInfo.base_info.loggerName;
	}

	const std::string Logger::LogName( )
	{
		return initInfo.base_info.logName;
	}

	const LoggerLevel Logger::GetLogLevel( )
	{
		return se_utils::ToLogLevel( m_clientLogger->level( ) );
	}

	void Logger::SetLogLevel( LoggerLevel level )
	{
		internalLogger->trace( "Setting Logger Level..." );
		if( se_utils::ToMappedLevel( level ) == MappedLevel::n_levels ) {
			initInfo.base_info.level = LoggerLevel::off;
			internalLogger->warn( "Log Level Was Not A Valid Value - Log Level Set To {}",
					      LogLevelToStr( initInfo.base_info.level ) );
			m_clientLogger->set_level( se_utils::ToMappedLevel( initInfo.base_info.level ) );
		}
		initInfo.base_info.level = level;
		m_clientLogger->set_level( se_utils::ToMappedLevel( level ) );
		internalLogger->trace( "Log Level Successfully Set To: {}", LogLevelToStr( initInfo.base_info.level ) );
	}

	void Logger::SetFlushLevel( LoggerLevel flushLevel )
	{
		internalLogger->trace( "Setting Logger Flush Level..." );
		if( se_utils::ToMappedLevel( flushLevel ) == MappedLevel::n_levels ) {
			initInfo.base_info.flushLevel = LoggerLevel::trace;
			m_clientLogger->flush_on( se_utils::ToMappedLevel( initInfo.base_info.flushLevel ) );
			internalLogger->warn( "Log Level Was Not A Valid Value - Logger Flush Level Set To {}",
					      LogLevelToStr( initInfo.base_info.flushLevel ) );
		}
		else {
			initInfo.base_info.flushLevel = flushLevel;
			m_clientLogger->flush_on( se_utils::ToMappedLevel( initInfo.base_info.flushLevel ) );
			internalLogger->trace( "Log Flush Level Successfully Set To: {}", LogLevelToStr( flushLevel ) );
		}
	}

	const LoggerLevel Logger::GetFlushLevel( )
	{
		return se_utils::ToLogLevel( m_clientLogger->flush_level( ) );
	}

	bool Logger::ShouldLog( )
	{
		if( m_clientLogger != nullptr ) {
			return ( GetLogLevel( ) <= se_globals::GetGlobalLevel( ) ) ? true : false;
		}
		else {
			return false;
		}
	}

	void Logger::ChangeInternalLoggerOptions( sinks::internal_logger_info &options )
	{
		internalLogger->trace( "Updating Internal Logger Options" );
		internalLogger->CustomizeInternalLogger( options );
		internalLogger->info( "Successfully Updated Internal Logger Options" );
	}
}  // namespace serenity
