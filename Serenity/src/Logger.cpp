
#include <serenity/Logger.h>
#include <serenity/Utilities/Utilities.h>

#include <fstream>

#pragma warning( push, 0 )
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning( pop )

namespace serenity
{
	std::shared_ptr<spdlog::logger> Logger::m_internalLogger;
	std::shared_ptr<spdlog::logger> Logger::m_clientLogger;
	std::unique_ptr<LogFileHelper>  Logger::logFileHandle;


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
		auto              defaultPath = file_helper::current_path( );
		file_helper::path logDirPath  = initInfo.logDir.path( );
		auto              filePath    = logDirPath.string( ) + "\\" + initInfo.logName;

		logFileHandle = std::make_unique<LogFileHelper>( initInfo.logDir, initInfo.logName );
		m_sinks       = std::make_unique<Sink>( );
		//##########################################################################################################
		/*
		  Some possible features to add:
		  - The ability to enable internal logging support independantly of client logger [X]
		  - Ties in with the above point, but also have the ability to turn off internal logging when in release [.5X]
		  - The ability to customize the internal logger? I mean, I'm using stdout sink here, but hey, who knows? [ ]
		*/
		//##########################################################################################################
		CreateInternalLogger( internalLoggerInfo );
		// Set The Paths
		FileHelperHandle( )->SetLogDirPath( logDirPath );
		SE_INTERNAL_TRACE( "Setting Log Directory Path To [{}]", logDirPath );
		FileHelperHandle( )->UpdateFileInfo( filePath );
		SE_INTERNAL_TRACE( "Updated File Info For Path [{}]", filePath );
		this->UpdateFileInfo( );
		// Creating Client Logger
		m_sinks->clear_sinks( );
		m_sinks->set_sinks( initInfo.sink_info.sinks );
		m_clientLogger = std::move( CreateLogger( initInfo ) );
		SE_INTERNAL_INFO( "Logger [{}] Successfully Initialized", initInfo.loggerName );
	}

	Logger::~Logger( )
	{
		Shutdown( );
	}
	void Logger::CreateInternalLogger( logger_info &infoStruct )
	{
		// Creating Internal Logger
		internalLoggerInfo.level      = LoggerLevel::trace;  // after testing -> LoggerLevel::warning
		internalLoggerInfo.loggerName = "SERENITY";
		file_helper::directory_entry internalDir { initInfo.logDir.path( ) };
		internalLoggerInfo.logDir  = internalDir;
		internalLoggerInfo.logName = "Internal_Log.txt";
		internalLoggerInfo.sink_info.sinks.clear( );
		internalLoggerInfo.sink_info.sinks.emplace_back( SinkType::stdout_color_mt );
		m_sinks->clear_sinks( );
		m_sinks->set_sinks( internalLoggerInfo.sink_info.sinks );
		m_internalLogger = std::move( CreateLogger( internalLoggerInfo, true ) );
		SE_INTERNAL_INFO( "Logger [{}] Successfully Initialized", internalLoggerInfo.loggerName );
	}

	void Logger::CloseLog( file_helper::path filePath )
	{
		if( file_helper::exists( filePath ) ) {
			try {
				SE_INTERNAL_TRACE( "Closing File [{}]...", filePath.filename( ) );
				file_utils::CloseFile( filePath );
				SE_INTERNAL_TRACE( "File [{}] Successfully Closed", filePath.filename( ) );
			}
			catch( const std::exception &e ) {
				SE_INTERNAL_ERROR( "Exception Thrown In CloseLog():\n%s\n", e.what( ) );
			}
		}
	}

	void Logger::OpenLog( file_helper::path filePath )
	{
		if( file_helper::exists( filePath ) ) {
			try {
				SE_INTERNAL_TRACE( "Opening File [{}]...", filePath.filename( ) );
				file_utils::OpenFile( filePath );
				SE_INTERNAL_TRACE( "File [{}] Successfully Opened", filePath.filename( ) );
			}
			catch( const std::exception &e ) {
				SE_INTERNAL_ERROR( "Exception Thrown In OpenLog():\n%s\n", e.what( ) );
			}
		}
	}


	void Logger::StopLogger( )
	{
		SE_INTERNAL_TRACE( "Stopping Logger..." );
		CloseLog( FileHelperHandle( )->LogFilePath( ) );
		DropLogger( );  // Drop client/internal loggers w/o shutting spdlog down
		SE_INTERNAL_TRACE( "Logger Has Been Stopped" );
	}

	void Logger::StartLogger( )
	{
		SE_INTERNAL_TRACE( "Starting Logger(s)..." );
		// DropLogger() resets m_sinks ptr -> have to recreate sink handle for CreateLogger()
		if( m_sinks == nullptr ) {
			m_sinks = std::make_unique<Sink>( );
			m_sinks->clear_sinks( );
			SE_INTERNAL_TRACE( "Sinks Handle Has Been Successfully Re-Initialized" );
		}
		// Recreates spdlog loggers And registers them with spdlog's registry if they were previously unregistered/destroyed
		if( m_internalLogger == nullptr ) {
			m_sinks->set_sinks( internalLoggerInfo.sink_info.sinks );
			m_internalLogger = CreateLogger( internalLoggerInfo, true );
			SE_INTERNAL_TRACE( "Internal Logger Has Been Successfully Re-Initialized" );
		}
		if( m_clientLogger == nullptr ) {
			m_sinks->set_sinks( initInfo.sink_info.sinks );
			m_clientLogger = CreateLogger( initInfo );
			SE_INTERNAL_TRACE( "Client Logger Has Been Successfully Re-Initialized" );
		}
		OpenLog( FileHelperHandle( )->LogFilePath( ) );
		SE_INTERNAL_TRACE( "Logger Has Been Successfully Restarted" );
	}

	void Logger::Shutdown( )
	{
		StopLogger( );
		SE_INTERNAL_TRACE( "Shutting Down..." );
		spdlog::drop_all( );
		spdlog::shutdown( );
		m_sinks.reset( );
		m_internalLogger.reset( );
		m_clientLogger.reset( );
	}

	void Logger::DropLogger( )
	{
		SE_INTERNAL_TRACE( "Dropping Logger And Resetting Handles...", m_clientLogger->name( ) );
		spdlog::drop( m_clientLogger->name( ) );
		SE_INTERNAL_TRACE( "Logger [{}] Has Been Dropped", m_clientLogger->name( ) );
		m_sinks.reset( );
		SE_INTERNAL_TRACE( "Sinks Handle For Logger Has Been Reset" );
		m_clientLogger.reset( );
		SE_INTERNAL_TRACE( "Logger Handle Has Been Reset" );
	}

	std::shared_ptr<spdlog::logger> Logger::CreateLogger( logger_info &infoStruct, bool internalLogger )
	{
		m_sinks->sinkVector.clear( );
		m_sinks->CreateSink( infoStruct );
		if( m_internalLogger != nullptr ) {
			SE_INTERNAL_TRACE( "Sinks For Logger [{}] Have Been Succesfully Created", infoStruct.loggerName );
		}
		auto mappedLevel = ToMappedLevel( infoStruct.level );

		if( internalLogger ) {
			if( GetGlobalLevel( ) != internalLoggerInfo.level ) {
				mappedLevel = ToMappedLevel( GetGlobalLevel( ) );
			}
			auto internalLogger = std::make_shared<spdlog::logger>( infoStruct.loggerName, begin( m_sinks->sinkVector ),
										end( m_sinks->sinkVector ) );
			spdlog::register_logger( internalLogger );
			SE_INTERNAL_TRACE( "Logger [{}] Has Been Successfully Created", infoStruct.loggerName );
			SE_INTERNAL_TRACE( "Logger [{}] Has Been Registered", infoStruct.loggerName );
			internalLogger->set_level( mappedLevel );
			SE_INTERNAL_TRACE( "Logger [{}] Level Has Been Set To: {}", infoStruct.loggerName,
					   LogLevelToStr( infoStruct.level ) );
			internalLogger->flush_on( mappedLevel );
			SE_INTERNAL_TRACE( "Logger [{}] Flush Level Has Been Set To: {}", infoStruct.loggerName,
					   LogLevelToStr( infoStruct.level ) );
			return internalLogger;
		}
		else {
			if( GetGlobalLevel( ) != infoStruct.level ) {
				mappedLevel = ToMappedLevel( GetGlobalLevel( ) );
			}
			std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>(
			  infoStruct.loggerName, begin( m_sinks->sinkVector ), end( m_sinks->sinkVector ) );
			SE_INTERNAL_TRACE( "Logger [{}] Has Been Successfully Created", infoStruct.loggerName );
			spdlog::register_logger( logger );
			SE_INTERNAL_TRACE( "Logger [{}] Has Been Registered", infoStruct.loggerName );
			logger->set_level( mappedLevel );
			SE_INTERNAL_TRACE( "Logger [{}] Level Has Been Set To: {}", infoStruct.loggerName,
					   LogLevelToStr( infoStruct.level ) );
			logger->flush_on( mappedLevel );
			SE_INTERNAL_TRACE( "Logger [{}] Flush Level Has Been Set To: {}", infoStruct.loggerName,
					   LogLevelToStr( infoStruct.level ) );
			return logger;
		}
	}

	bool Logger::RenameLog( std::string newName, bool replaceIfExists )
	{
		// In Case Path Is Passed In
		file_helper::path newFilePath = newName;
		const auto &      tmpPath     = initInfo.logDir.path( );
		file_helper::path oldPath     = tmpPath.string( ).append( "\\" + initInfo.logName );
		file_helper::path newPath     = tmpPath.string( ).append( "\\" + newFilePath.filename( ).string( ) );
		SE_INTERNAL_TRACE( "Renaming Log File [{}] To [{}]", oldPath.filename( ), newPath.filename( ) );
		try {
			// Flush and close logs, Drop Loggers From spdlog's registry so sinks
			// ref to files are dropped, and reset the logger pointers
			StopLogger( );
			if( file_helper::exists( newPath ) ) {
				SE_INTERNAL_WARN( "File [{}] Already Exists", newPath.filename( ) );
				if( replaceIfExists ) {
					SE_INTERNAL_TRACE( "Parameter \"replaceIfExists\" = true." );
					SE_INTERNAL_TRACE( "Removing [{}] Before Renaming [{}] To [{}].", newPath.filename( ),
							   oldPath.filename( ), newPath.filename( ) );
					file_utils::RemoveEntry( newPath );
					SE_INTERNAL_TRACE( "Removed File [{}]", newPath.filename( ) );
				}
				else {
					SE_INTERNAL_TRACE( "Parameter \"replaceIfExists\" = false." );
					SE_INTERNAL_TRACE( "Copying File Contents: \n- From:\t[{}] \n- To:\t[{}]", oldPath, newPath );
					file_utils::CopyContents( oldPath, newPath );
					SE_INTERNAL_TRACE( "Copying Contents Finished. Removing File [{}]", oldPath.filename( ) );
					file_utils::RemoveEntry( oldPath );
					SE_INTERNAL_TRACE( "Removed File Link From [{}]", oldPath.filename( ) );
				}
			}
			file_utils::RenameFile( oldPath, newPath );
			SE_INTERNAL_INFO( "Renaming [{}] To [{}] Was Successful.\n", oldPath.filename( ), newPath.filename( ) );
			// Effectively Updating Variables
			initInfo.logName = newPath.filename( ).string( );
			SE_INTERNAL_TRACE( "Log File Name Has Been Updated To Reflect Rename To [{}]", newPath.filename( ) );
			FileHelperHandle( )->UpdateFileInfo( newPath );
			SE_INTERNAL_TRACE( "File Info Has Been Updated Based On Path:\n{}", newPath );
			UpdateFileInfo( );
			SE_INTERNAL_TRACE( "Logger Has Been Updated For File Path Changes" );
			StartLogger( );
			return true;
		}
		catch( const std::exception &e ) {
			SE_INTERNAL_ERROR( "Renaming Failed." );
			SE_INTERNAL_ERROR( "EXCEPTION CAUGHT IN RenameLog():\n{}\n", e.what( ) );
			return false;
		}
	}

	void Logger::UpdateFileInfo( )
	{
		SE_INTERNAL_TRACE( "Checking For File Info Changes..." );
		if( FileHelperHandle( )->fileInfoChanged ) {
			SE_INTERNAL_TRACE( "File Info Has Changed. Updating File Info Handles" );
			// Sometimes is called already by caller, however, ensures everything is up to date
			FileHelperHandle( )->UpdateFileInfo( FileHelperHandle( )->LogFilePath( ) );
			SE_INTERNAL_TRACE( "File Info Handles Have Been Updated" );
		}
		else {
			SE_INTERNAL_TRACE( "File Info Already Up-To-Date" );
		}
		FileHelperHandle( )->fileInfoChanged = false;
	}

	std::string const Logger::LoggerName( )
	{
		return initInfo.loggerName;
	}

	const LoggerLevel Logger::GetLogLevel( )
	{
		return ToLogLevel( m_clientLogger->level( ) );
	}

	void Logger::SetLogLevel( LoggerLevel level, LoggerInterface logInterface )
	{
		SE_INTERNAL_TRACE( "Setting Logger Level..." );
		auto m_level = ToMappedLevel( level );
		if( !m_level ) {
			SE_INTERNAL_FATAL( "Log Level Was Not A Valid Value" );
		}
		switch( logInterface ) {
			case LoggerInterface::internal:
				{
					m_internalLogger->set_level( m_level );
					SE_INTERNAL_TRACE( "Log Level Successfully Set To: [{}]", LogLevelToStr( level ) );
				}
				break;
			case LoggerInterface::client:
				{
					m_clientLogger->set_level( m_level );
					SE_INTERNAL_TRACE( "Log Level Successfully Set To: [{}]", LogLevelToStr( level ) );
				}
				break;
			default:
				{
					m_internalLogger->set_level( MappedLevel::off );
					m_clientLogger->set_level( MappedLevel::off );
					SE_INTERNAL_WARN( "Log Interface Was Not A Valid Value - Log Level Set To 'OFF'\n" );
				}
				break;
		}
	}

	bool Logger::ClientShouldLog( )
	{
		return ( GetLogLevel( ) <= GetGlobalLevel( ) ) ? true : false;
	}

	bool Logger::InternalShouldLog( )
	{
		return ( ToLogLevel( InternalLogger( )->level( ) ) <= GetGlobalLevel( ) ) ? true : false;
	}

}  // namespace serenity

// MISC Functions
namespace serenity
{
	std::string GetSerenityVerStr( )
	{
		auto version = VERSION_NUMBER( SERENITY_MAJOR, SERENITY_MINOR, SERENITY_REV );
		return version;
	}
	void SetGlobalLevel( LoggerLevel level )
	{
		global_level = level;
		if( Logger::ClientSideLogger( ) != nullptr ) {
			Logger::ClientSideLogger( )->set_level( ToMappedLevel( level ) );
		}
		if( Logger::InternalLogger( ) != nullptr ) {
			Logger::InternalLogger( )->set_level( ToMappedLevel( level ) );
		}
	}
}  // namespace serenity