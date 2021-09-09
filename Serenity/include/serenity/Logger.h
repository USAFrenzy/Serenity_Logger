#pragma once


#pragma warning( push )
#pragma warning( disable : 26812 )
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
// Only Here For The Sink Struct - Remove When Struct Moves From Here
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#pragma warning( pop )

#include <serenity/Common.h>
#include <serenity/Helpers/LogFileHelper.h>
#include <serenity/Interfaces/IObserver.h>
#include <serenity/Sinks/Sinks.h>


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
	class Logger : public ILogger
	{
	      public:
		explicit Logger( logger_info &infoStruct );
		Logger( ) = delete;
		// Debating If I Want Copying And/Or Moving Of Logger Objects Allowed
		Logger( const Logger &copy )  = delete;
		Logger( const Logger &&move ) = delete;
		Logger &operator=( const Logger & ) = delete;
		~Logger( );

		void              SetLoggerLevel( LoggerLevel logLevel, LoggerInterface logInterface );
		bool              RenameLog( std::string newName );
		std::string const GetLoggerName( );
		// In The Same Fashion As The Note From LogFileHelper, Just Learned Of CVs Which Seems Perfect For This
		void                            UpdateFileInfo( ) override;
		void                            CloseLog( std::string loggerName );
		void                            OpenLog( file_helper::path filePath );
		void                            StopLogger( );
		void                            StartLogger( );
		void                            Shutdown( );
		std::shared_ptr<spdlog::logger> CreateLogger( Sink::SinkType sink, logger_info &infoStruct, bool internalLogger = false );

		static MappedLevel MapToMappedLevel( LoggerLevel level );
		LoggerLevel        MapToLogLevel( MappedLevel level );
		static void        Init( Logger &logger, LoggerLevel level );

		static const std::shared_ptr<spdlog::logger> &GetInternalLogger( )
		{
			return m_internalLogger;
		}
		static const std::shared_ptr<spdlog::logger> &GetClientSideLogger( )
		{
			return m_clientLogger;
		}
		const std::unique_ptr<LogFileHelper> &GetFileHelperHandle( )
		{
			return logFileHandle;
		}

	      public:
		std::mutex m_mutex;

	      private:
		logger_info           initInfo     = { };
		std::string           m_loggerName = initInfo.loggerName;
		std::string           m_logName    = initInfo.logName;
		serenity::MappedLevel m_level      = MapToMappedLevel( initInfo.level );

		static std::shared_ptr<spdlog::logger> m_internalLogger;
		static std::shared_ptr<spdlog::logger> m_clientLogger;
		std::unique_ptr<LogFileHelper>         logFileHandle;
		Sink                                   m_sinks;
	};

}  // namespace serenity

namespace serenity
{
	std::string GetSerenityVerStr( );
}  // namespace serenity


// Internal or "non-user" side macros
#define SE_INTERNAL_TRACE( ... )                                                                                                      \
	if( Logger::GetInternalLogger( ) != nullptr ) {                                                                               \
		Logger::GetInternalLogger( )->trace( __VA_ARGS__ );                                                                   \
	}
#define SE_INTERNAL_DEBUG( ... )                                                                                                      \
	if( Logger::GetInternalLogger( ) != nullptr ) {                                                                               \
		Logger::GetInternalLogger( )->debug( __VA_ARGS__ );                                                                   \
	}
#define SE_INTERNAL_INFO( ... )                                                                                                       \
	if( Logger::GetInternalLogger( ) != nullptr ) {                                                                               \
		Logger::GetInternalLogger( )->info( __VA_ARGS__ );                                                                    \
	}
#define SE_INTERNAL_WARN( ... )                                                                                                       \
	if( Logger::GetInternalLogger( ) != nullptr ) {                                                                               \
		Logger::GetInternalLogger( )->warn( __VA_ARGS__ );                                                                    \
	}
#define SE_INTERNAL_ERROR( ... )                                                                                                      \
	if( Logger::GetInternalLogger( ) != nullptr ) {                                                                               \
		Logger::GetInternalLogger( )->error( __VA_ARGS__ );                                                                   \
	}
#define SE_INTERNAL_FATAL( ... )                                                                                                      \
	if( Logger::GetInternalLogger( ) != nullptr ) {                                                                               \
		Logger::GetInternalLogger( )->critical( __VA_ARGS__ );                                                                \
	}
#define SE_INTERNAL_ASSERT( condition, message, ... )                                                                                 \
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
			Logger::GetClientSideLogger( )->trace( __VA_ARGS__ );                                                         \
		}
	#define SE_DEBUG( ... )                                                                                                       \
		if( Logger::GetClientSideLogger( ) != nullptr ) {                                                                     \
			Logger::GetClientSideLogger( )->debug( __VA_ARGS__ );                                                         \
		}
	#define SE_INFO( ... )                                                                                                        \
		if( Logger::GetClientSideLogger( ) != nullptr ) {                                                                     \
			Logger::GetClientSideLogger( )->info( __VA_ARGS__ );                                                          \
		}
	#define SE_WARN( ... )                                                                                                        \
		if( Logger::GetClientSideLogger( ) != nullptr ) {                                                                     \
			Logger::GetClientSideLogger( )->warn( __VA_ARGS__ );                                                          \
		}
	#define SE_ERROR( ... )                                                                                                       \
		if( Logger::GetClientSideLogger( ) != nullptr ) {                                                                     \
			Logger::GetClientSideLogger( )->error( __VA_ARGS__ );                                                         \
		}
	#define SE_FATAL( ... )                                                                                                       \
		if( Logger::GetClientSideLogger( ) != nullptr ) {                                                                     \
			Logger::GetClientSideLogger( )->critical( __VA_ARGS__ );                                                      \
		}
	#define SE_ASSERT( condition, message, ... )                                                                                  \
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