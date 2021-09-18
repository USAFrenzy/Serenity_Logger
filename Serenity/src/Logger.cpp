
#include <serenity/Logger.h>
#include <serenity/Utilities/Utilities.h>

#include <map>
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


	namespace map_helper
	{
		LoggerLevel MapToLogLevel( MappedLevel level )
		{
			std::map<MappedLevel, LoggerLevel> levelMap = {
			  { MappedLevel::trace, LoggerLevel::trace }, { MappedLevel::info, LoggerLevel::info },
			  { MappedLevel::debug, LoggerLevel::debug }, { MappedLevel::warn, LoggerLevel::warning },
			  { MappedLevel::err, LoggerLevel::error },   { MappedLevel::critical, LoggerLevel::fatal } };
			LoggerLevel result   = LoggerLevel::off;
			auto        iterator = levelMap.find( level );
			if( iterator != levelMap.end( ) ) {
				result = iterator->second;
			}
			return result;
		}
		MappedLevel MapToMappedLevel( LoggerLevel level )
		{
			std::map<LoggerLevel, MappedLevel> levelMap = {
			  { LoggerLevel::trace, MappedLevel::trace }, { LoggerLevel::info, MappedLevel::info },
			  { LoggerLevel::debug, MappedLevel::debug }, { LoggerLevel::warning, MappedLevel::warn },
			  { LoggerLevel::error, MappedLevel::err },   { LoggerLevel::fatal, MappedLevel::critical } };
			MappedLevel result   = MappedLevel::off;
			auto        iterator = levelMap.find( level );
			if( iterator != levelMap.end( ) ) {
				result = iterator->second;
			}
			return result;
		}
	}  // namespace map_helper

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
		  - The ability to enable internal logging support independantly of client logger
		  - Ties in with the above point, but also have the ability to turn off internal logging when in release
		  - The ability to customize the internal logger? I mean, I'm using stdout sink here, but hey, who knows?
		*/
		//##########################################################################################################
		// Creating Internal Logger
		internalLoggerInfo.level      = LoggerLevel::trace;  // after testing -> LoggerLevel::warning
		internalLoggerInfo.loggerName = "SERENITY";
		file_helper::directory_entry internalDir { logDirPath };
		internalLoggerInfo.logDir  = internalDir;
		internalLoggerInfo.logName = "Internal_Log.txt";
		internalLoggerInfo.sink_info.sinks.clear( );
		internalLoggerInfo.sink_info.sinks.emplace_back( SinkType::stdout_color_mt );
		m_sinks->clear_sinks( );
		m_sinks->set_sinks( internalLoggerInfo.sink_info.sinks );
		m_internalLogger = std::move( CreateLogger( internalLoggerInfo, true ) );
		SE_INTERNAL_INFO( "Logger [{}] Successfully Initialized", internalLoggerInfo.loggerName );
		// Set The Paths
		FileHelperHandle( )->SetLogDirPath( logDirPath );
		SE_INTERNAL_TRACE( "Set Log Directory Path To [{}]", logDirPath );
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

	void Logger::CloseLog( file_helper::path filePath )
	{
		if( file_helper::exists( filePath ) ) {
			try {
				file_utils::CloseFile( filePath );
				SE_INTERNAL_TRACE( "Closed File [{}]", filePath.filename());
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
				file_utils::OpenFile( filePath );
				SE_INTERNAL_TRACE( "Opened File [{}]", filePath.filename() );
			}
			catch( const std::exception &e ) {
				SE_INTERNAL_ERROR( "Exception Thrown In OpenLog():\n%s\n", e.what( ) );
			}
		}
	}


	void Logger::StopLogger( )
	{
		CloseLog( FileHelperHandle( )->LogFilePath( ) );
		DropLogger( );  // Drop client/internal loggers w/o shutting spdlog down
		SE_INTERNAL_TRACE( "Logger Has Been Stopped" );		
	}

	void Logger::StartLogger( )
	{
		// DropLogger() resets m_sinks ptr -> have to recreate CreateLogger()
		if( m_sinks == nullptr ) {
			m_sinks = std::make_unique<Sink>( );
			m_sinks->clear_sinks( );
			SE_INTERNAL_TRACE( "Sinks Handle Has Been Successfully Recreated" );		
		}
		// Recreates spdlog loggers And registers them with spdlog's registry if they were previously unregistered/destroyed
		if( m_internalLogger == nullptr ) {
			m_sinks->set_sinks( internalLoggerInfo.sink_info.sinks );
			m_internalLogger = CreateLogger( internalLoggerInfo, true );
			SE_INTERNAL_TRACE( "Internal Logger Has Been Successfully Recreated" );		
		}
		if( m_clientLogger == nullptr ) {
			m_sinks->set_sinks( initInfo.sink_info.sinks );
			m_clientLogger = CreateLogger( initInfo );
			SE_INTERNAL_TRACE( "Client Logger Has Been Successfully Recreated" );
		}
		OpenLog( FileHelperHandle( )->LogFilePath( ) );
		SE_INTERNAL_TRACE( "Logger Has Been Started" );
	}

	void Logger::Shutdown( )
	{
		StopLogger( );
		SE_INTERNAL_TRACE( "Now Shutting Down And Resetting Handles" );		
		spdlog::drop_all( );
		spdlog::shutdown( );
		m_sinks.reset( );
		m_internalLogger.reset( );
		m_clientLogger.reset( );
	}

	void Logger::DropLogger( )
	{
		// spdlog::drop( m_internalLogger->name( ) );
		spdlog::drop( m_clientLogger->name( ) );
		SE_INTERNAL_TRACE( "Logger [{}] Has Been Dropped", m_clientLogger->name() );		
		// m_sinks->clear_sinks( );
		m_sinks.reset( );
		SE_INTERNAL_TRACE( "Sinks Handle For Logger Has Been Reset" );		
		// m_internalLogger.reset( );
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
		auto mappedLevel = map_helper::MapToMappedLevel( infoStruct.level );
		if( internalLogger ) {
			auto internalLogger = std::make_shared<spdlog::logger>( infoStruct.loggerName, begin( m_sinks->sinkVector ),
										end( m_sinks->sinkVector ) );
			spdlog::register_logger( internalLogger );
			SE_INTERNAL_TRACE( "Logger [{}] Has Been Successfully Created" , infoStruct.loggerName);		
			SE_INTERNAL_TRACE( "Logger [{}] Has Been Registered", infoStruct.loggerName );
			internalLogger->set_level( mappedLevel );
			SE_INTERNAL_TRACE( "Logger [{}] Level Has Been Set To: {}", infoStruct.loggerName, infoStruct.level );		
			internalLogger->flush_on( mappedLevel );
			SE_INTERNAL_TRACE( "Logger [{}] Flush Level Has Been Set To: {}", infoStruct.loggerName, infoStruct.level );		
			return internalLogger;
		}
		else {
			std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>(
			  infoStruct.loggerName, begin( m_sinks->sinkVector ), end( m_sinks->sinkVector ) );
			SE_INTERNAL_TRACE( "Logger [{}] Has Been Successfully Created", infoStruct.loggerName );		
			spdlog::register_logger( logger );
			SE_INTERNAL_TRACE( "Logger [{}] Has Been Registered", infoStruct.loggerName );
			logger->set_level( mappedLevel );
			SE_INTERNAL_TRACE( "Logger [{}] Level Has Been Set To: {}", infoStruct.loggerName, infoStruct.level );		
			logger->flush_on( mappedLevel );
			SE_INTERNAL_TRACE( "Logger [{}] Flush Level Has Been Set To: {}", infoStruct.loggerName, infoStruct.level );		
			return logger;
		}
	}

	bool Logger::RenameLog( std::string newName, bool replaceIfExists )
	{
		// In Case Path Is Passed In
		file_helper::path newFilePath = newName;
		// Caching To Local Vars Before Deletion Of Loggers
		const auto &      tmpPath = initInfo.logDir.path( );
		file_helper::path oldPath = tmpPath.string( ).append( "\\" + initInfo.logName );
		file_helper::path newPath = tmpPath.string( ).append( "\\" + newFilePath.filename( ).string( ) );
		try {
			// Flush and close logs, Drop Loggers From spdlog's registry so sinks ref to files are dropped, and reset the
			// logger pointers
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
					SE_INTERNAL_TRACE(
					  "Parameter \"replaceIfExists\" = false.");
					file_utils::CopyContents( oldPath, newPath );
					SE_INTERNAL_TRACE( "Copying Contents Finished. Removing File [{}]", oldPath.filename( ) );
					file_utils::RemoveEntry( oldPath );
					SE_INTERNAL_TRACE( "Removed File Link From [{}]", oldPath.filename( ) );
				}
			}
			SE_INTERNAL_TRACE( "Renaming File [{}] To [{}]", oldPath.filename( ), newPath.filename( ) );
			file_utils::RenameFile( oldPath, newPath );
			SE_INTERNAL_INFO( "Renaming [{}] To [{}] Was Successful.\n", oldPath.filename( ), newPath.filename( ) );
			// Effectively Updating Variables
			initInfo.logName = newPath.filename( ).string( );
			SE_INTERNAL_TRACE( "Log File Name Has Been Updated To Reflect Rename To [{}]", newPath.filename());		
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
		if( FileHelperHandle( )->fileInfoChanged ) {
			SE_INTERNAL_TRACE( "File Info Has Changed. Updating File Info Handles" );		
			// Sometimes is called already by caller, however, ensures the cache variables are up to date
			FileHelperHandle( )->UpdateFileInfo( FileHelperHandle( )->LogFilePath( ) );
			SE_INTERNAL_TRACE( "File Info Handles Have Been Updated" );		
		}
		FileHelperHandle( )->fileInfoChanged = false;
	}

	std::string const Logger::LoggerName( )
	{
		return initInfo.loggerName;
	}


	void Logger::SetLoggerLevel( LoggerLevel level, LoggerInterface logInterface )
	{
		auto m_level = map_helper::MapToMappedLevel( level );
		if( !m_level ) {
			SE_INTERNAL_FATAL( "Log Level Was Not A Valid Value" );
		}
		switch( logInterface ) {
			case LoggerInterface::internal:
				{
					m_internalLogger->set_level( m_level );
				}
				break;
			case LoggerInterface::client:
				{
					m_clientLogger->set_level( m_level );
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


}  // namespace serenity