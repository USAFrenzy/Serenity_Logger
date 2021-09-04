
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
	std::shared_ptr<spdlog::details::file_helper> Logger::m_FileHelper;
	Logger *                                      Logger::loggerInstance;
	std::shared_ptr<serenity::cache_logger> Logger::cache_handle;

	// clang-format off
/* 
	In The Near Future, Would like to be able to just do 
	- Logger(LoggerInfo loggerInfoStruct, LogFileInfo logInfoStruct, LogSink sinkInfo);
*/
	// clang-format on


	Logger::Logger( logger_info &infoStruct )
	  : m_loggerName( infoStruct.loggerName ), m_logName( infoStruct.logName ), LogFileHelper( infoStruct.logDir, infoStruct.logName )
	{
		loggerInstance = this;
		initInfo = infoStruct;
		// Getting Close To Where I Would Want To Abstract Code Into The Respective Class's Init() Functions To
		// Call Here, However, Should Re-evaluate If It's Neccessary That Logger Strictly Inherits From LogFileHelper
		// Rather Than Just Having A Handle Or Vice Versa
		auto                        defaultPath = serenity::file_helper::current_path( );
		serenity::file_helper::path logDirPath  = infoStruct.logDir.path( );
		logFileHandle = this->_instance();
		auto filePath                           = logDirPath.string( ) + "\\" + infoStruct.logName;
		cache_handle = std::make_shared<cache_logger>( );
		m_cacheInstance = cache_handle.get()->instance();
		m_FileHelper   = std::make_shared<spdlog::details::file_helper>( );

		logFileHandle->UpdateFileInfo( filePath );
		UpdateLoggerFileInfo( );
		SetLogDirPath( logDirPath );
		Init( *loggerInstance, infoStruct.level );
	}

	void Logger::CloseLog( std::string loggerName)
	{
		loggerInstance->GetFileHelperHandle( )->flush( );
		loggerInstance->GetFileHelperHandle( )->close( );
		std::fstream log(loggerInstance->GetLogFilePath().string());
		if( log.is_open( ) ) {
			log.close( );
			if( log.is_open( ) ) {
				printf( "Failed To Close [%s]", loggerInstance->GetLogFilePath( ).string( ).c_str( ) );
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
		loggerInstance->GetCacheHandle()->cacheLogDirPath = GetLogDirPath( );
		}
		
		m_logName    = loggerInstance->GetCacheHandle( )->instance( )->cacheLogName;
		m_loggerName = loggerInstance->GetCacheHandle( )->instance( )->cacheLoggerName;
		m_level      = loggerInstance->GetCacheHandle( )->instance( )->cacheLevel;
		logFileHandle->SetLogFilePath( loggerInstance->GetCacheHandle( )->instance( )->cacheLogPath );
		auto refreshedPath = GetLogFilePath( );
		logFileHandle->UpdateFileInfo( refreshedPath );
		loggerInstance->UpdateLoggerFileInfo( );
	}

	void Logger::CacheLogger( )
	{
		m_logName                                                       = initInfo.logName;
		loggerInstance->GetCacheHandle( )->instance( )->cacheLoggerName = m_loggerName;
		loggerInstance->GetCacheHandle( )->instance( )->cacheLogName    = m_logName;
		loggerInstance->GetCacheHandle( )->instance( )->cacheLevel      = m_level;
		loggerInstance->GetCacheHandle( )->instance( )->cacheLogPath    = logFileHandle->GetLogFilePath( );
		logFileHandle->UpdateFileInfo( loggerInstance->GetCacheHandle( )->instance( )->cacheLogPath );
	}
	void Logger::OpenLog( file_helper::path filePath )
	{
		loggerInstance->RefreshFromCache( );
		SE_DEBUG( "OpenLog(): LogFilePath() = {}", GetLogFilePath( ) );
		try {
			spdlogHandle.open( filePath.string( ) );
		}
		catch( const std::exception &e ) {
			printf( "Exception Thrown In OpenLog():\n%s\n", e.what( ) );
		}
	}
	void Logger::SetLogDirPath( file_helper::path logDirPath )
	{
		loggerInstance->GetCacheHandle( )->instance( )->cacheLogDirPath = logDirPath;
		prev_func_called                                                = true;
		RefreshCache( );
		prev_func_called = false;
	}
	file_helper::path const Logger::GetLogDirPath( )
	{
		return loggerInstance->GetCacheHandle( )->instance( )->cacheLogDirPath;
	}
	void Logger::StopLoggers( ) 
	{ 
		loggerInstance->CloseLog(m_internalLogger.get()->name());
		loggerInstance->CloseLog( m_clientLogger.get( )->name( ));
		spdlog::drop_all( );
		spdlog::shutdown( );

		loggerInstance->m_internalLogger.reset( );
		loggerInstance->m_clientLogger.reset( );
	}

	// Starting To Restructure In Favor of Passing In A Struct Here That Will Be Called In Init()
	void Logger::StartLoggers( ) 
	{ 
		auto cache = loggerInstance->GetCacheHandle( )->instance( );

		loggerInstance->m_internalLogger =
		    std::make_shared<spdlog::logger>( "INTERNAL", begin( cache->cacheSinks ), end( cache->cacheSinks ) );

		cache->cacheInternalLogger.reset( );
		cache->cacheInternalLogger =
		  std::make_unique<spdlog::logger>( "INTERNAL", begin( cache->cacheSinks ), end( cache->cacheSinks ) );

		loggerInstance->m_clientLogger =
		  std::make_shared<spdlog::logger>( cache->cacheLoggerName, begin( cache->cacheSinks ), end( cache->cacheSinks ) );
		
		cache->cacheClientLogger.reset( );
		cache->cacheClientLogger =
		  std::make_unique<spdlog::logger>( cache->cacheLoggerName, begin( cache->cacheSinks ), end( cache->cacheSinks ) );
		
		spdlog::register_logger( m_internalLogger );
		m_internalLogger->set_level( cache->cacheLevel);
		m_internalLogger->flush_on( cache->cacheLevel );

		spdlog::register_logger( m_clientLogger );
		m_clientLogger->set_level( cache->cacheLevel );
		m_clientLogger->flush_on( cache->cacheLevel );
	}
	/* clang-format off
	##################################################################################################################################
	# THIS IS A WORK IN PROGRESS FUNCTION AND CURRENTLY DOES NOT WORK AS INTENDED - DROPS SPDLOG HANDLE W/NO WAY TO ATTAIN IT AGAIN  #
	##################################################################################################################################
	clang-format on */
	void Logger::RenameLog( std::string newName )
	{
		se_thread::se_mutex_guard fileLock;
		fileLock.acquire_lock( );
		// In Case Path Is Passed In
		file_helper::path newFilePath = newName;
		
		const auto &tmpPath = initInfo.logDir.path();
		auto        oldPath = tmpPath.string( ).append( "\\" + m_logName );
		auto        newPath = tmpPath.string( ).append( "\\" + newFilePath.filename( ).string( ) );
		SE_ERROR( "oldPath: {}", oldPath );
		SE_ERROR( "newPath: {}", newPath );

		loggerInstance->StopLoggers( );

		// There's A Process Still Locking The File...
		// Testing The se_thread stuff to see if that helps...
		try {
			file_utils::RenameFile( oldPath, newPath );
		}
		catch( file_helper::filesystem_error &e ) {
			printf( "ERROR: %s\n", e.what( ));
		}
		initInfo.logName = newFilePath.filename().string();
		loggerInstance->UpdateFileInfo( newFilePath);
		RefreshCache( );

		loggerInstance->StartLoggers( );
		loggerInstance->UpdateLoggerFileInfo( );

		fileLock.release_lock( );
		// fileLock isn't currently helping either so its def something else...
		}


	void Logger::UpdateLoggerFileInfo( )
	{
		if( fileInfoChanged ) {
			// Sometimes is called already by caller, however, ensures the cache variables are up to date
			logFileHandle->UpdateFileInfo( logFileHandle->GetLogFilePath( ) );
			RefreshCache( );
		}
		fileInfoChanged = ( !fileInfoChanged );
	}

	Logger::~Logger( )
	{
		spdlog::shutdown( );
		logFileHandle->~LogFileHelper( );
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
		auto                          filePath = logger.initInfo.logDir.path( ).string( ).append( "\\" + logger.initInfo.logName );

		bool truncate { false };
		sinks.emplace_back( std::make_shared<spdlog::sinks::stdout_color_sink_mt>( ) );
		sinks.emplace_back( std::make_shared<spdlog::sinks::basic_file_sink_mt>( filePath, truncate ) );
		// Would Like To Format this in a more personalized and absstracted manner
		sinks[ 0 ]->set_pattern( "%^[%T] %n: %v%$" );
		sinks[ 1 ]->set_pattern( "[%T] [%l] %n: %v" );
		logger.GetCacheHandle( )->cacheSinks = sinks;
		// For both logger types, would like to abstract away the mappedLevel in a way that a flush level doesnt
		// always have to be the same as what is set
		int i { 1 };
		const int retries { 5 };

		spdlog::details::file_helper helper;
		helper.flush( );
		helper.close( );

		try {
			if( ( m_internalLogger.get( ) == nullptr ) && ( m_clientLogger.get( ) == nullptr ) ) {
				cache_handle->cacheInternalLogger = std::make_unique<spdlog::logger>( "INTERNAL", begin( sinks ), end( sinks ) );
				m_internalLogger = std::make_shared<spdlog::logger>( "INTERNAL", begin( sinks ), end( sinks ) );
				spdlog::register_logger( m_internalLogger );
				m_internalLogger->set_level( mappedLevel );
				m_internalLogger->flush_on( mappedLevel );

				m_clientLogger = std::make_shared<spdlog::logger>( logger.GetLoggerName( ), begin( sinks ), end( sinks ) );
				cache_handle->cacheClientLogger = std::make_unique<spdlog::logger>( logger.GetLoggerName( ), begin( sinks ), end( sinks ) );
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
			for( i; i < retries;  i++) {
				printf( "Retrying... Attempt %i", i + 1 );
				logger.CloseLog(logger.GetLoggerName());
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
		m_level = MapToMappedLevel( level );
		if( !m_level ) {
			throw std::runtime_error( "Log Level Was Not A Valid Value" );
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