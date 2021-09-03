#pragma once

#include <memory>

#pragma warning( push )
#pragma warning( disable : 26812 )
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/details/file_helper.h>

#pragma warning( pop )

#include <serenity/Defines.hpp>
#include <serenity/Common.hpp>
#include <serenity/Helpers/LogFileHelper.hpp>
#include <serenity/Interfaces/IObserver.hpp>
#include <serenity/Cache/LoggerCache.hpp>


/* clang-format off
 
	I believe what I'll end up doing is having the logger class have instance handles to the inherited classes
   and in the constructor, ill go ahead and call those inherited classes move constructors(?) to go ahead and move
   in the data thats passed into the logger construction to those handles. The question really becomes how best to
   move the data around from user side to internally for correct variable values. Might create respective structs
   to initialize values and use those values by reference in the logger construction
   i.e.
   struct LogFileInfo     fileInfo = {};           AND/OR    Have a default construction that initializes these values
   struct LogggerSinkInfo loggerSinks = {};                             Logger defaultLogger;                
   struct LoggerInfo      loggerInfo = {};								** Logger(){ defaultInit(); }
   Logger myLogger(loggerInfo, loggerSinkInfo, fileInfo);

clang-format on                                                                                                     */

namespace serenity
{
	class Logger : public serenity::LogFileHelper, public ILogger
	{
	      public:
		explicit Logger( logger_info &infoStruct );
		Logger( )                = delete;
		Logger( const Logger & ) = delete;
		~Logger( );

		void                    SetLoggerLevel( LoggerLevel logLevel, LoggerInterface logInterface );
		void                    RenameLog( std::string newName );
		std::string const       GetLoggerName( );
		void                    UpdateLoggerFileInfo( ) override;
		void                    SetLogDirPath( file_helper::path logDirPath ) override;
		file_helper::path const GetLogDirPath( ) override;
		void                    CloseLog( std::string loggerName );
		void                    OpenLog( file_helper::path filePath );
		void                    RefreshCache( );
		void                    RefreshFromCache( );
		static MappedLevel      MapToMappedLevel( LoggerLevel level );
		LoggerLevel             MapToLogLevel( MappedLevel level );
		static void             Init( Logger &logger, LoggerLevel level );

		static const std::shared_ptr<spdlog::logger> &GetInternalLogger( )
		{
			return m_internalLogger;
		}
		static const std::shared_ptr<spdlog::logger> &GetClientSideLogger( )
		{
			return m_clientLogger;
		}
		static const std::shared_ptr<spdlog::details::file_helper> &GetFileHelperHandle( )
		{
			return m_FileHelper;
		}
		static const std::shared_ptr<cache_logger> &GetCacheHandle( )
		{
			return cache_handle;
		}
		inline static void ForwardFileUpdates( )
		{
			loggerInstance->UpdateLoggerFileInfo( );
		}


	      public:
		LogFileHelper *logFileHandle;

	      private:
		logger_info           initInfo     = { };
		std::string           m_loggerName = initInfo.loggerName;
		std::string           m_logName    = initInfo.logName;
		serenity::MappedLevel m_level      = MapToMappedLevel( initInfo.level );

		static std::shared_ptr<spdlog::logger>               m_internalLogger;
		static std::shared_ptr<spdlog::logger>               m_clientLogger;
		static std::shared_ptr<spdlog::details::file_helper> m_FileHelper;
		spdlog::details::file_helper                         spdlogHandle;
		// Thinking Of Somehow Manipulating Multiple Instances Of Loggers Using Something As A Pointer To Other Instances Here
		static Logger *loggerInstance;

		cache_logger *                       m_cacheInstance;
		static std::shared_ptr<cache_logger> cache_handle;


		void CacheLogger( );
		bool prev_func_called { false };
	};

}  // namespace serenity

namespace serenity
{
	std::string GetSerenityVerStr( );
}  // namespace serenity


// Internal or "non-user" side macros
#define SE_INTERNAL_TRACE( ... )                                                                                                      \
	if( Logger::GetInternalLogger( ) != nullptr ) {                                                                               \
		Logger::ForwardFileUpdates( );                                                                                        \
		Logger::GetInternalLogger( )->trace( __VA_ARGS__ );                                                                   \
	}
#define SE_INTERNAL_DEBUG( ... )                                                                                                      \
	if( Logger::GetInternalLogger( ) != nullptr ) {                                                                               \
		Logger::ForwardFileUpdates( );                                                                                        \
		Logger::GetInternalLogger( )->debug( __VA_ARGS__ );                                                                   \
	}
#define SE_INTERNAL_INFO( ... )                                                                                                       \
	if( Logger::GetInternalLogger( ) != nullptr ) {                                                                               \
		Logger::ForwardFileUpdates( );                                                                                        \
		Logger::GetInternalLogger( )->info( __VA_ARGS__ );                                                                    \
	}
#define SE_INTERNAL_WARN( ... )                                                                                                       \
	if( Logger::GetInternalLogger( ) != nullptr ) {                                                                               \
		Logger::ForwardFileUpdates( );                                                                                        \
		Logger::GetInternalLogger( )->warn( __VA_ARGS__ );                                                                    \
	}
#define SE_INTERNAL_ERROR( ... )                                                                                                      \
	if( Logger::GetInternalLogger( ) != nullptr ) {                                                                               \
		Logger::ForwardFileUpdates( );                                                                                        \
		Logger::GetInternalLogger( )->error( __VA_ARGS__ );                                                                   \
	}
#define SE_INTERNAL_FATAL( ... )                                                                                                      \
	if( Logger::GetInternalLogger( ) != nullptr ) {                                                                               \
		Logger::ForwardFileUpdates( );                                                                                        \
		Logger::GetInternalLogger( )->critical( __VA_ARGS__ );                                                                \
	}
#define SE_INTERNAL_ASSERT( condition, message, ... )                                                                                 \
	Logger::ForwardFileUpdates( );                                                                                                \
	if( !( condition ) ) {                                                                                                        \
		SE_INTERNAL_FATAL( "ASSERTION FAILED: {}\nIn File: {} On Line: {}\n{}", SE_MACRO_STRING( condition ),                 \
				   std::filesystem::path( __FILE__ ).filename( ).string( ), ( __LINE__ ),                             \
				   ( SE_ASSERT_VAR_MSG( message, __VA_ARGS__ ) ) );                                                   \
		SE_DEBUG_BREAK                                                                                                        \
	}

#if defined( SERENITY_TEST_RUN ) || !defined( NDEBUG )
   // Client side macros
	#define SE_TRACE( ... )                                                                                                       \
		if( Logger::GetClientSideLogger( ) != nullptr ) {                                                                     \
			Logger::ForwardFileUpdates( );                                                                                \
			Logger::GetClientSideLogger( )->trace( __VA_ARGS__ );                                                         \
		}
	#define SE_DEBUG( ... )                                                                                                       \
		if( Logger::GetClientSideLogger( ) != nullptr ) {                                                                     \
			Logger::ForwardFileUpdates( );                                                                                \
			Logger::GetClientSideLogger( )->debug( __VA_ARGS__ );                                                         \
		}
	#define SE_INFO( ... )                                                                                                        \
		if( Logger::GetClientSideLogger( ) != nullptr ) {                                                                     \
			Logger::ForwardFileUpdates( );                                                                                \
			Logger::GetClientSideLogger( )->info( __VA_ARGS__ );                                                          \
		}
	#define SE_WARN( ... )                                                                                                        \
		if( Logger::GetClientSideLogger( ) != nullptr ) {                                                                     \
			Logger::ForwardFileUpdates( );                                                                                \
			Logger::GetClientSideLogger( )->warn( __VA_ARGS__ );                                                          \
		}
	#define SE_ERROR( ... )                                                                                                       \
		if( Logger::GetClientSideLogger( ) != nullptr ) {                                                                     \
			Logger::ForwardFileUpdates( );                                                                                \
			Logger::GetClientSideLogger( )->error( __VA_ARGS__ );                                                         \
		}
	#define SE_FATAL( ... )                                                                                                       \
		if( Logger::GetClientSideLogger( ) != nullptr ) {                                                                     \
			Logger::ForwardFileUpdates( );                                                                                \
			Logger::GetClientSideLogger( )->critical( __VA_ARGS__ );                                                      \
		}
	#define SE_ASSERT( condition, message, ... )                                                                                  \
		Logger::ForwardFileUpdates( );                                                                                        \
		if( !( condition ) ) {                                                                                                \
			SE_FATAL( "ASSERTION FAILED: {}\nIn File: {} On Line: {}\n{}", SE_MACRO_STRING( condition ),                  \
				  std::filesystem::path( __FILE__ ).filename( ).string( ), ( __LINE__ ),                              \
				  ( SE_ASSERT_VAR_MSG( message, __VA_ARGS__ ) ) );                                                    \
			SE_DEBUG_BREAK                                                                                                \
		}

#else
	#define SE_ASSERT_VAR_MSG( message, ... )             ( void ) 0
	#define SERENITY_TRACE( ... )                         ( void ) 0
	#define SERENITY_DEBUG( ... )                         ( void ) 0
	#define SERENITY_INFO( ... )                          ( void ) 0
	#define SERENITY_WARN( ... )                          ( void ) 0
	#define SERENITY_ERROR( ... )                         ( void ) 0
	#define SERENITY_FATAL( ... )                         ( void ) 0
	#define SE_INTERNAL_ASSERT( condition, message, ... ) ( void ) 0
	#define SE_TRACE( ... )                               ( void ) 0
	#define SE_DEBUG( ... )                               ( void ) 0
	#define SE_INFO( ... )                                ( void ) 0
	#define SE_WARN( ... )                                ( void ) 0
	#define SE_ERROR( ... )                               ( void ) 0
	#define SE_FATAL( ... )                               ( void ) 0
	#define SE_ASSERT( condition, message, ... )          ( void ) 0
#endif  // SERENITY_TEST_RUN