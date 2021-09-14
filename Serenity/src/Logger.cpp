
#include <serenity/Logger.h>
#include <serenity/Utilities/Utilities.h>

#include <fstream>

#pragma warning( push, 0 )
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning( pop )

namespace serenity
{
	std::shared_ptr<spdlog::logger> Logger::m_clientLogger;
	// : m_loggerName( infoStruct.loggerName ), m_logName( infoStruct.logName )
	Logger::Logger( logger_info &infoStruct )
	{
		initInfo                      = infoStruct;
		auto              defaultPath = file_helper::current_path( );
		file_helper::path logDirPath  = infoStruct.logDir.path( );
		auto              filePath    = logDirPath.string( ) + "\\" + infoStruct.logName;
		logFileHandle                 = std::make_unique<LogFileHelper>( infoStruct.logDir, infoStruct.logName );
		m_sinks                       = *std::make_unique<Sink>( );  // probably dont need
		FileHelperHandle( )->UpdateFileInfo( filePath );
		UpdateFileInfo( );
		FileHelperHandle( )->SetLogDirPath( logDirPath );
		//##########################################################################################################
		// Not Entirely Sure Why, But Initializing Client Logger After Internal Logger Leads To Client Logger Fields
		// Not Fully Populating In The Logger Creation -> Should Probably Look Into That (Works If Internal Logger
		// Is Initialized After Client Logger Though). Really Only Needed, As Of Now, Since I Would Like To Initalize
		// The Internal Logger First So I Can Spit Out Logical Log Messages When Each Logger Has Been Initialized
		// Versus What I'm Doing Now - Waiting For Both To Be Initialized To Log That Each Creation Was Successful
		//##########################################################################################################
		// Client Logger
		m_sinks.get_sinks( ).clear( );
		m_sinks.set_sinks( infoStruct.sink_info.sinks );
		m_clientLogger  = std::move( CreateLogger( infoStruct ) );
		libLoggerhandle = std::make_unique<LibLogger>( "INTERNAL_LOGGER" );
		// Look At LibLogger Class -> Possibly Need Buffers?... Might Have Just Over Complicated All This..
		GetLibLogger( )->se_info( " Logger [{}]\tSuccessfully Initialized", GetLibLogger( )->name( ) );
		GetLibLogger( )->se_info( " Logger [{}]\tSuccessfully Initialized\n", infoStruct.loggerName );
	}

	Logger::~Logger( )
	{
		Shutdown( );
	}


	void Logger::CloseLog( std::string filePath )
	{
		if( file_helper::exists( filePath ) ) {
			try {
				FileHelperHandle( )->CloseFile( filePath );
			}
			catch( const std::exception &e ) {
				GetLibLogger( )->se_error( "Error in CloseLog():\n%s\n", e.what( ) );
			}
		}
	}

	void Logger::OpenLog( std::string filePath )
	{
		if( file_helper::exists( filePath ) ) {
			try {
				FileHelperHandle( )->OpenFile( filePath );
			}
			catch( const std::exception &e ) {
				GetLibLogger( )->se_error( "Exception Thrown In OpenLog():\n%s\n", e.what( ) );
			}
		}
	}


	void Logger::StopLogger( )
	{
		CloseLog( FileHelperHandle( )->LogFilePath( ).string( ) );
	}

	void Logger::StartLogger( )
	{
		OpenLog( FileHelperHandle( )->LogFilePath( ).string( ) );
	}

	void Logger::Shutdown( )
	{
		StopLogger( );
		spdlog::drop_all( );
		spdlog::shutdown( );
		libLoggerhandle->Shutdown( );
		m_clientLogger.reset( );
		libLoggerhandle.reset( );
	}

	const std::unique_ptr<LogFileHelper> &Logger::FileHelperHandle( )
	{
		return logFileHandle;
	}
	const std::unique_ptr<LibLogger> &Logger::GetLibLogger( )
	{
		return libLoggerhandle;
	}

	std::shared_ptr<spdlog::logger> Logger::CreateLogger( /*Sink::SinkType sink,*/ logger_info &infoStruct )
	{
		m_sinks.sinkVector.clear( );
		m_sinks.CreateSink( infoStruct );
		auto                            mappedLevel = map_helper::MapToMappedLevel( infoStruct.level );
		std::shared_ptr<spdlog::logger> logger =
		  std::make_shared<spdlog::logger>( infoStruct.loggerName, begin( m_sinks.sinkVector ), end( m_sinks.sinkVector ) );
		spdlog::register_logger( logger );
		logger->set_level( mappedLevel );
		logger->flush_on( mappedLevel );
		return logger;
	}

	bool Logger::RenameLog( std::string newName )
	{
		std::lock_guard<std::mutex> lock( m_mutex );

		// In Case Path Is Passed In
		file_helper::path newFilePath = newName;
		// Caching To Local Vars Before Deletion Of Loggers
		const auto &      tmpPath = initInfo.logDir.path( );
		file_helper::path oldPath = tmpPath.string( ).append( "\\" + FileHelperHandle( )->FileName( ).string( ) );
		file_helper::path newPath = tmpPath.string( ).append( "\\" + newFilePath.filename( ).string( ) );
		try {
			StopLogger( );  // Flush and close logs
			/*
			 * Needed To Release Spdlog Handle To File And shared_ptrs Referencing Files (had issues with spdlog's rename
			 * for some reason) If I can figure out what the issue was before, I'll drop the Shutdown() and recreation of
			 * sinks and simplify this function more.
			 * In theory, should just be able to check if file exists, copy contents from old to new target, remove the old
			 * target, rename old target to new target, and update file paths
			 *  - spdlog's method of renaming removes the new target if it exists and renames the old target to the new one
			 *    though, so if re-implemented, will have to take that into consideration
			 *  - I wanted to copy contents so as not to lose any log info, but in certain cases, I guess this could lead
			 *    to mixed logs from other loggers resulting in wierd and confusing logs...
			 */
			Shutdown( );  // Drops spdlog Loggers, shuts down spdlog, and resets pointers
			if( file_helper::exists( newPath ) ) {
				std::string msg = fmt::format( "File [{}] Already Exists\n", newPath.filename( ) );
				fmt::print( msg );
				file_utils::CopyContents( oldPath, newPath );
				file_utils::RemoveEntry( oldPath );
			}
			file_utils::RenameFile( oldPath, newPath );

			// Effectively Updating Variables
			initInfo.logName = newPath.filename( ).string( );
			FileHelperHandle( )->UpdateFileInfo( newPath );
			UpdateFileInfo( );

			// Recreates spdlog loggers And registers them with spdlog's registry
			if( m_clientLogger == nullptr ) {
				m_clientLogger = CreateLogger( initInfo );
			}
			if( GetLibLogger( ) == nullptr ) {
				libLoggerhandle = std::make_unique<LibLogger>( "INTERNAL_LOGGER" );
			}
			StartLogger( );
			return true;
		}
		catch( const std::exception &e ) {
			GetLibLogger( )->se_fatal( "EXCEPTION CAUGHT IN RenameLog():\n{}", e.what( ) );
			return false;
		}
	}
	// "Use of the comma operator in a tested expression causes the left argument to be ignored when it has no side effects"
#pragma warning( push )
#pragma warning( disable : 6319 )
	void Logger::UpdateFileInfo( )
	{
		if( FileHelperHandle( )->fileInfoChanged ) {
			// Sometimes is called already by caller, however, ensures the variables are up to date
			FileHelperHandle( )->UpdateFileInfo( FileHelperHandle( )->LogFilePath( ) );
		}
		FileHelperHandle( )->fileInfoChanged = false;
	}
#pragma warning( pop )

	std::string const Logger::LoggerName( )
	{
		return initInfo.loggerName;
	}


	void Logger::SetLoggerLevel( LoggerLevel level )
	{
		auto tempLvl = map_helper::MapToMappedLevel( level );
		if( !tempLvl ) {
			GetLibLogger( )->se_fatal( "Log Level Was Not A Valid Value" );
		}
		initInfo.level = level;
	}


}  // namespace serenity