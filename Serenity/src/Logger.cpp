
#include <serenity/Logger.hpp>
#include <serenity/Utilities/Utilities.hpp>
#include <serenity/Cache/LoggerCache.hpp>

#include <map>
#include <string>
#include <fstream>

#pragma warning( push, 0 )
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/details/os.h>
#pragma warning( pop )


/* clang-format off
                  NOTE FOR FUTURE, SINCE DOING OBSERVER-LIKE PATTERNS ARE STILL NEW FOR ME RIGHT NOW
###########################################################################################################################
	Used as a psuedo call-back in a mock observer pattern. There is most likely a better way to implement
   something like this, however, this will only do work: calling UpdateFileInfo() if and only if a function has
   changed a file info property (currently only paths). There's a NotifyLogger() function that simply sets
   fileInfoChanged to true which then will allow the poll for updated info to populate. The rest is really
   straight-forward: toggle the bool to "reset" and only allow a function that changes the fields to change this
   value.
   - To Ensure Bool Stays Safe To Use:
		- I believe by always having NotifyLogger() simply set fileInfoChanged = true, then multiple calls
          that change file info fields won't run the risk of inadverdantly blocking the UpdateLoggerFileInfo() function
		- In The Same Regard, by always waiting for fileInfoChanged to be true, UpdateLoggeFileInfo() will
          never execute code uneccessarily (although it does always perform a condition check).
###########################################################################################################################
clang-format on
*/

/*
	At the moment, I have the cache_logger struct storing a unique_ptr to spdlog::logger for internal and client loggers.
	The Constructor of cache_logger is supposed to create a unique_ptr of that instance, move that pointer to the shared_ptr
	m_instance, to which the Logger class cache_handle can refer to using GetCacheHandle().

*/
namespace serenity
{
	std::shared_ptr<spdlog::logger>               Logger::m_internalLogger;
	std::shared_ptr<spdlog::logger>               Logger::m_clientLogger;
	Logger *                                      Logger::loggerInstance;
	std::shared_ptr<serenity::cache_logger>       Logger::cache_handle;

	// clang-format off
/* 
	In The Near Future, Would like to be able to just do 
	- Logger(LoggerInfo loggerInfoStruct, LogFileInfo logInfoStruct, LogSink sinkInfo);
*/
	// clang-format on


	Logger::Logger( logger_info &infoStruct )
	  : m_loggerName( infoStruct.loggerName ), m_logName( infoStruct.logName )
	{
		// Might Change If I Go The Singleton Route? Otherwise Will Probs Just Get Rid Of loggerInstance var
		loggerInstance = this;
		initInfo       = infoStruct;
		auto                        defaultPath = serenity::file_helper::current_path( );
		serenity::file_helper::path logDirPath  = infoStruct.logDir.path( );
		auto filePath                           = logDirPath.string( ) + "\\" + infoStruct.logName;

		loggerInstance->logFileHandle                           = std::make_unique<LogFileHelper>(infoStruct.logDir, infoStruct.logName);
		loggerInstance->cache_handle                            = std::make_shared<cache_logger>( );
		loggerInstance->m_sinks                                 = *std::make_unique<Sinks>( );  // probably dont need
		loggerInstance->GetFileHelperHandle( )->UpdateFileInfo( filePath );
		loggerInstance->UpdateLoggerFileInfo( );
		loggerInstance->GetFileHelperHandle( )->SetLogDirPath( logDirPath );
		//##########################################################################################################
		loggerInstance->GetCacheHandle( )->cacheInitInfo = { };
		loggerInstance->GetCacheHandle( )->cacheInitInfo = *&initInfo;
		// Will Probably Have This Bit Abstracted Away Into The initInfo Struct
		loggerInstance->m_sinks.set_sink_type( Sinks::SinkType::basic_file_mt );
		loggerInstance->m_clientLogger =
		  std::move( loggerInstance->CreateLogger( loggerInstance->m_sinks.get_sink_type( ), infoStruct ) );
		// Above Is Redundant ATM Just Due To No Data Being Passed In For Realistically Setting/Getting Sink Type
		loggerInstance->m_internalLogger =
		  std::move( loggerInstance->CreateLogger( Sinks::SinkType::stdout_color_mt, infoStruct, true ) );
	}

	Logger::~Logger( )
	{
		loggerInstance->Shutdown( );
	}

	void Logger::CloseLog( std::string loggerName )
	{
		// Explicit Checking For File Status
		std::fstream log( logFileHandle->GetLogFilePath( ).string( ) );
		if( log.is_open( ) ) {
			try {
				log.close( );
			}
			catch( const std::exception &e ) {
				printf( "Error in CloseLog():\n%s\n", e.what( ) );
			}
		}
	}
	// As A Simple Function Wrapper That Does Nothing
	// Else At The Moment, Probably Can Just Get Rid Of This..
	void Logger::RefreshCache( )
	{
		CacheLogger( );
	}

	void Logger::RefreshFromCache( )
	{
		if( !prev_func_called ) {
			loggerInstance->GetCacheHandle( )->cacheLogDirPath = logFileHandle->GetLogDirPath( );
		}

		loggerInstance->m_logName    = loggerInstance->GetCacheHandle( )->instance( )->cacheLogName;
		loggerInstance->m_loggerName = loggerInstance->GetCacheHandle( )->instance( )->cacheLoggerName;
		loggerInstance->m_level      = loggerInstance->GetCacheHandle( )->instance( )->cacheLevel;
		loggerInstance->GetFileHelperHandle( )->SetLogFilePath( loggerInstance->GetCacheHandle( )->instance( )->cacheLogPath );
		auto refreshedPath = loggerInstance->GetFileHelperHandle( )->GetLogFilePath( );
		loggerInstance->GetFileHelperHandle( )->UpdateFileInfo( refreshedPath );
		loggerInstance->UpdateLoggerFileInfo( );
	}

	void Logger::CacheLogger( )
	{
		loggerInstance->m_logName                          = initInfo.logName;
		loggerInstance->GetCacheHandle( )->cacheLoggerName = m_loggerName;
		loggerInstance->GetCacheHandle( )->cacheLogName    = m_logName;
		loggerInstance->GetCacheHandle( )->cacheLevel      = m_level;
		loggerInstance->GetCacheHandle( )->cacheLogPath    = loggerInstance->GetFileHelperHandle( )->GetLogFilePath( );
		loggerInstance->GetFileHelperHandle( )->UpdateFileInfo( loggerInstance->GetCacheHandle( )->cacheLogPath );
	}
	void Logger::OpenLog( file_helper::path filePath )
	{
		loggerInstance->RefreshFromCache( );
		SE_DEBUG( "OpenLog(): LogFilePath() = {}", logFileHandle->GetLogFilePath( ) );
		try {
			// TODO: Implement A Close/Open File In file_utils Namespace And Wrap In LogFileHelper
			//loggerInstance->GetFileHelperHandle( )->OpenFile( filePath.string( ) );
		}
		catch( const std::exception &e ) {
			printf( "Exception Thrown In OpenLog():\n%s\n", e.what( ) );
		}
	}
	
	
	void Logger::StopLogger( )
	{
		loggerInstance->CloseLog( m_clientLogger.get( )->name( ) );
		// I'm Guessing I'll Only have To Do Internal Reset For As Long
		// As The Current Init() Is Called...
		loggerInstance->CloseLog( m_internalLogger.get( )->name( ) );
	}

	// Starting To Restructure In Favor of Passing In A Struct Here That Will Be Called In Init()
	void Logger::StartLogger( )
	{
		loggerInstance->OpenLog( GetCacheHandle( )->cacheLogPath );
	}

	void Logger::Shutdown( )
	{
		spdlog::drop_all( );
		spdlog::shutdown( );
		loggerInstance->m_internalLogger.reset( );
		loggerInstance->m_clientLogger.reset( );
		loggerInstance->GetCacheHandle( )->cacheClientLogger.reset( );
		loggerInstance->GetCacheHandle( )->cacheInternalLogger.reset( );
	}

	std::shared_ptr<spdlog::logger> Logger::CreateLogger( Sinks::SinkType sink, logger_info &infoStruct, bool internalLogger )
	{
		loggerInstance->m_sinks.sinkVector.clear( );
		loggerInstance->m_sinks.CreateSink( sink, infoStruct );
		auto mappedLevel = loggerInstance->MapToMappedLevel( infoStruct.level );

		if( internalLogger ) {
			auto internalLogger =
			  std::make_shared<spdlog::logger>( "INTERNAL", begin( loggerInstance->m_sinks.sinkVector ), end( loggerInstance->m_sinks.sinkVector ) );
			spdlog::register_logger( internalLogger );
			internalLogger->set_level( mappedLevel );
			internalLogger->flush_on( mappedLevel );

			loggerInstance->GetCacheHandle( )->cacheInternalLogger =
			  std::make_unique<spdlog::logger>( "INTERNAL", begin( loggerInstance->m_sinks.sinkVector ), end( loggerInstance->m_sinks.sinkVector ) );
			loggerInstance->GetCacheHandle( )->cacheInternalLogger->set_level( mappedLevel );
			loggerInstance->GetCacheHandle( )->cacheInternalLogger->flush_on( mappedLevel );
			return internalLogger;
		}
		else {
			std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>(
			  loggerInstance->GetCacheHandle( )->cacheLoggerName, begin( loggerInstance->m_sinks.sinkVector ), end( loggerInstance->m_sinks.sinkVector ) );
			spdlog::register_logger( logger );
			logger->set_level( mappedLevel );
			logger->flush_on( mappedLevel );

			loggerInstance->GetCacheHandle( )->cacheClientLogger = std::make_unique<spdlog::logger>(
			  loggerInstance->GetCacheHandle( )->cacheLoggerName, begin( loggerInstance->m_sinks.sinkVector ), end( loggerInstance->m_sinks.sinkVector ) );
			loggerInstance->GetCacheHandle( )->cacheClientLogger->set_level( mappedLevel );
			loggerInstance->GetCacheHandle( )->cacheClientLogger->flush_on( mappedLevel );

			return logger;
		}
	}

	// Finally Got This Working Properly! Now To See What I Can Do To Clean It Up A Little Bit
	bool Logger::RenameLog( std::string newName )
	{
		std::lock_guard<std::mutex> lock( m_mutex );

		// In Case Path Is Passed In
		file_helper::path newFilePath = newName;
		// Caching To Local Vars Before Deletion Of Loggers
		const auto &      tmpPath = loggerInstance->initInfo.logDir.path( );
		file_helper::path oldPath = tmpPath.string( ).append( "\\" + loggerInstance->m_logName );
		file_helper::path newPath = tmpPath.string( ).append( "\\" + newFilePath.filename( ).string( ) );
		try {
			loggerInstance->StopLogger( );  // Flushes Log And Closes them
			loggerInstance->Shutdown( );    // Drops spdlog Loggers, shuts down spdlog, and resets pointers
			if( file_helper::exists( newPath ) ) {
				std::string msg = fmt::format( "File [{}] Already Exists\n", newPath.filename( ) );
				fmt::print(msg);
				file_utils::CopyContents( oldPath, newPath );
				file_utils::RemoveEntry( oldPath );
			}
			file_utils::RenameFile( oldPath, newPath );

			// Effectively Updating Cache Variables
			loggerInstance->GetCacheHandle( )->cacheInitInfo.logName = newFilePath.filename( ).string( );
			loggerInstance->GetFileHelperHandle( )->UpdateFileInfo( newFilePath );
			loggerInstance->UpdateLoggerFileInfo( );
			loggerInstance->RefreshCache( );

			// Recreates spdlog loggers And registers them with spdlog's registry
			loggerInstance->m_clientLogger = CreateLogger( loggerInstance->m_sinks.get_sink_type( ), loggerInstance->GetCacheHandle( )->cacheInitInfo );
			loggerInstance->m_internalLogger =
			  loggerInstance->CreateLogger(Sinks::SinkType::stdout_color_mt, loggerInstance->GetCacheHandle( )->cacheInitInfo, true );

			loggerInstance->StartLogger( );
			return true;
		}
		catch( const std::exception &e ) {
			SE_INTERNAL_ERROR( "EXCEPTION CAUGHT IN RenameLog():\n{}", e.what( ) );
			return false;
		}
	}


	void Logger::UpdateLoggerFileInfo( )
	{
		if( loggerInstance->GetFileHelperHandle()->fileInfoChanged ) {
			// Sometimes is called already by caller, however, ensures the cache variables are up to date
			loggerInstance->GetFileHelperHandle( )->UpdateFileInfo( loggerInstance->GetFileHelperHandle( )->GetLogFilePath( ) );
			loggerInstance->RefreshCache( );
		}
		loggerInstance->GetFileHelperHandle( )->fileInfoChanged = false;
	}

	std::string const Logger::GetLoggerName( )
	{
		return loggerInstance->m_loggerName;
	}

	void Logger::Init( Logger &logger, LoggerLevel setLevel )
	{
		// Keeping this pretty simple before deviating too hard core - end goal would be to abstract some of the
		// setup in some structs and then just pass in the desired struct into the init function
		auto                          mappedLevel = MapToMappedLevel( setLevel );
		std::vector<spdlog::sink_ptr> sinks;
		auto filePath = logger.initInfo.logDir.path( ).string( ).append( "\\" + logger.initInfo.logName );

		bool truncate { false };
		sinks.emplace_back( std::make_shared<spdlog::sinks::stdout_color_sink_mt>( ) );
		sinks.emplace_back( std::make_shared<spdlog::sinks::basic_file_sink_mt>( filePath, truncate ) );
		// Would Like To Format this in a more personalized and absstracted manner
		sinks[ 0 ]->set_pattern( "%^[%T] %n: %v%$" );
		sinks[ 1 ]->set_pattern( "[%T] [%l] %n: %v" );
		logger.GetCacheHandle( )->cacheSinks = sinks;
		// For both logger types, would like to abstract away the mappedLevel in a way that a flush level doesnt
		// always have to be the same as what is set
		int       i { 1 };
		const int retries { 5 };
		try {
			if( ( m_internalLogger.get( ) == nullptr ) && ( m_clientLogger.get( ) == nullptr ) ) {
				cache_handle->cacheInternalLogger =
				  std::make_unique<spdlog::logger>( "INTERNAL", begin( sinks ), end( sinks ) );
				m_internalLogger = std::make_shared<spdlog::logger>( "INTERNAL", begin( sinks ), end( sinks ) );
				spdlog::register_logger( m_internalLogger );
				m_internalLogger->set_level( mappedLevel );
				m_internalLogger->flush_on( mappedLevel );

				m_clientLogger = std::make_shared<spdlog::logger>( logger.GetLoggerName( ), begin( sinks ), end( sinks ) );
				cache_handle->cacheClientLogger =
				  std::make_unique<spdlog::logger>( logger.GetLoggerName( ), begin( sinks ), end( sinks ) );
				spdlog::register_logger( m_clientLogger );
				m_clientLogger->set_level( mappedLevel );
				m_clientLogger->flush_on( mappedLevel );
				SE_INFO( "Logger: \"{}\" Successfully Initialized", logger.GetLoggerName( ) );
			}
			else {
				if( i == 0 ) {
					SE_WARN( "Warning: Trying To Initialize A Logger Of Same Name: {}", logger.GetLoggerName( ) );
					i++;
				}
			}
		}
		catch( const std::exception &e ) {
			SE_ERROR( "Logger {} Failed To Initialize", logger.GetLoggerName( ) );
			SE_FATAL( "{}", e.what( ) );
			for( i; i < retries; i++ ) {
				printf( "Retrying... Attempt %i", i + 1 );
				logger.CloseLog( logger.GetLoggerName( ) );
				spdlog::drop( "INTERNAL" );
				spdlog::drop( logger.GetLoggerName( ) );
				// Might Cause Unending Loop?
				Init( logger, setLevel );
			}
		}
	}

	using MappedLevel = serenity::MappedLevel;
	LoggerLevel Logger::MapToLogLevel( MappedLevel level )
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

	void Logger::SetLoggerLevel( LoggerLevel level, LoggerInterface logInterface )
	{
		loggerInstance->m_level = MapToMappedLevel( level );
		if( !loggerInstance->m_level ) {
			throw std::runtime_error( "Log Level Was Not A Valid Value" );
		}
		switch( logInterface ) {
			case LoggerInterface::internal:
				{
					loggerInstance->m_internalLogger->set_level( loggerInstance->m_level );
				}
				break;
			case LoggerInterface::client:
				{
					loggerInstance->m_clientLogger->set_level( loggerInstance->m_level );
				}
				break;
			default:
				{
					loggerInstance->m_internalLogger->set_level( MappedLevel::off );
					loggerInstance->m_clientLogger->set_level( MappedLevel::off );
					throw std::runtime_error( "Log Interface Was Not A Valid Value - Log Level Set To 'OFF'\n" );
				}
				break;
		}
	}

	serenity::MappedLevel Logger::MapToMappedLevel( LoggerLevel level )
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
}  // namespace serenity