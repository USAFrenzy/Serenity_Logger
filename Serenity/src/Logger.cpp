
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
		serenity::MappedLevel MapToMappedLevel( LoggerLevel level )
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

	Logger::Logger( logger_info &infoStruct ) : m_loggerName( infoStruct.loggerName ), m_logName( infoStruct.logName )
	{
		initInfo                      = infoStruct;
		auto              defaultPath = file_helper::current_path( );
		file_helper::path logDirPath  = infoStruct.logDir.path( );
		auto              filePath    = logDirPath.string( ) + "\\" + infoStruct.logName;

		logFileHandle = std::make_unique<LogFileHelper>( infoStruct.logDir, infoStruct.logName );
		m_sinks       = *std::make_unique<Sink>( );  // probably dont need
		FileHelperHandle( )->UpdateFileInfo( filePath );
		UpdateFileInfo( );
		FileHelperHandle( )->SetLogDirPath( logDirPath );
		//##########################################################################################################
		// Client Logger
		m_sinks.get_sinks( ).clear( );
		m_sinks.set_sinks( infoStruct.sink_info.sinks );

		m_clientLogger = std::move( CreateLogger( infoStruct ) );
		SE_INTERNAL_INFO( "Logger [{}] Successfully Initialized", infoStruct.loggerName);
		// Creating Internal Logger
		internalLoggerInfo.level      = LoggerLevel::trace;  // after testing -> LoggerLevel::warning
		internalLoggerInfo.loggerName = "INTERNAL_LOGGER";
		internalLoggerInfo.logDir     = infoStruct.logDir;
		internalLoggerInfo.logName    = "Internal_Log.txt";
		internalLoggerInfo.sink_info.sinks.clear( );
		internalLoggerInfo.sink_info.sinks.emplace_back( SinkType::stdout_color_mt );
		m_internalLogger = std::move( CreateLogger( internalLoggerInfo, true ) );
		SE_INTERNAL_INFO( "Logger [{}] Successfully Initialized", internalLoggerInfo.loggerName);
	}

	Logger::~Logger( )
	{
		Shutdown( );
	}

	void Logger::CloseLog( std::string loggerName )
	{
		// Explicit Checking For File Status
		std::fstream log( logFileHandle->LogFilePath( ).string( ) );
		if( log.is_open( ) ) {
			try {
				log.close( );
			}
			catch( const std::exception &e ) {
				SE_INTERNAL_ERROR( "Error in CloseLog():\n%s\n", e.what( ) );
			}
		}
	}

	void Logger::OpenLog( file_helper::path filePath )
	{
		SE_INTERNAL_DEBUG( "OpenLog(): LogFilePath() = {}", logFileHandle->LogFilePath( ) );
		try {
			// TODO: Implement A Close/Open File In file_utils Namespace And Wrap In LogFileHelper
			// GetFileHelperHandle( )->OpenFile( filePath.string( ) );
		}
		catch( const std::exception &e ) {
			SE_INTERNAL_ERROR( "Exception Thrown In OpenLog():\n%s\n", e.what( ) );
		}
	}


	void Logger::StopLogger( )
	{
		CloseLog( m_clientLogger.get( )->name( ) );
		CloseLog( m_internalLogger.get( )->name( ) );
	}

	void Logger::StartLogger( )
	{
		OpenLog( FileHelperHandle( )->LogFilePath( ) );
	}

	void Logger::Shutdown( )
	{
		spdlog::drop_all( );
		spdlog::shutdown( );
		m_internalLogger.reset( );
		m_clientLogger.reset( );
	}

	std::shared_ptr<spdlog::logger> Logger::CreateLogger( /*Sink::SinkType sink,*/ logger_info &infoStruct, bool internalLogger )
	{
		m_sinks.sinkVector.clear( );
		m_sinks.CreateSink( infoStruct );
		auto                            mappedLevel = map_helper::MapToMappedLevel( infoStruct.level );
		if( internalLogger ) {
			auto internalLogger =
			  std::make_shared<spdlog::logger>( infoStruct.loggerName, begin( m_sinks.sinkVector ), end( m_sinks.sinkVector ) );
			spdlog::register_logger( internalLogger );
			internalLogger->set_level( mappedLevel );
			internalLogger->flush_on( mappedLevel );
			return internalLogger;
		}
		else {
			std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>(
			  infoStruct.loggerName, begin( m_sinks.sinkVector ), end( m_sinks.sinkVector ) );
			spdlog::register_logger( logger );
			logger->set_level( mappedLevel );
			logger->flush_on( mappedLevel );
			return logger;
		}
	}

	bool Logger::RenameLog( std::string newName )
	{
		std::lock_guard<std::mutex> lock( m_mutex );

		// In Case Path Is Passed In
		file_helper::path newFilePath = newName;
		// Caching To Local Vars Before Deletion Of Loggers
		const auto &      tmpPath = initInfo.logDir.path( );
		file_helper::path oldPath = tmpPath.string( ).append( "\\" + m_logName );
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
			if( m_clientLogger = nullptr ) {
				m_clientLogger = CreateLogger( initInfo );
			}
			if( m_internalLogger = nullptr ) {
				m_internalLogger = CreateLogger( internalLoggerInfo, true );
			}

			StartLogger( );
			return true;
		}
		catch( const std::exception &e ) {
			SE_INTERNAL_ERROR( "EXCEPTION CAUGHT IN RenameLog():\n{}", e.what( ) );
			return false;
		}
	}


	void Logger::UpdateFileInfo( )
	{
		if( FileHelperHandle( )->fileInfoChanged ) {
			// Sometimes is called already by caller, however, ensures the cache variables are up to date
			FileHelperHandle( )->UpdateFileInfo( FileHelperHandle( )->LogFilePath( ) );
		}
		FileHelperHandle( )->fileInfoChanged = false;
	}

	std::string const Logger::LoggerName( )
	{
		return m_loggerName;
	}


	void Logger::SetLoggerLevel( LoggerLevel level, LoggerInterface logInterface )
	{
		m_level = map_helper::MapToMappedLevel( level );
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