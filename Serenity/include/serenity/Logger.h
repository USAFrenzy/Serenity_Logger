#pragma once


#pragma warning( push )
#pragma warning( disable : 26812 )
#include <spdlog/fmt/ostr.h>
#pragma warning( pop )

#include <serenity/Common.h>
#include <serenity/Helpers/LogFileHelper.h>
#include <serenity/Interfaces/IObserver.h>
#include <serenity/Sinks/Sinks.h>

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

		std::shared_ptr<spdlog::logger>
				  CreateLogger( /* Sink::SinkType sink, */ logger_info &infoStruct, bool internalLogger = false );
		void              StartLogger( );
		void              StopLogger( );
		void              Shutdown( );
		void              SetLoggerLevel( LoggerLevel logLevel, LoggerInterface logInterface );
		std::string const LoggerName( );
		bool              RenameLog( std::string newName );
		// In The Same Fashion As The Note From LogFileHelper, Just Learned Of CVs Which Seems Perfect For This
		void UpdateFileInfo( ) override;
		void OpenLog( file_helper::path filePath );
		void CloseLog( std::string loggerName );


		static const std::shared_ptr<spdlog::logger> &InternalLogger( )
		{
			return m_internalLogger;
		}
		static const std::shared_ptr<spdlog::logger> &ClientSideLogger( )
		{
			return m_clientLogger;
		}
		const std::unique_ptr<LogFileHelper> &FileHelperHandle( )
		{
			return logFileHandle;
		}

	      public:
		std::mutex m_mutex;

	      private:
		logger_info initInfo           = { };
		// due to the logger_info initInfo struct, could possibly reduce class size by removing these?
		std::string m_loggerName = initInfo.loggerName;
		std::string m_logName    = initInfo.logName;
		MappedLevel m_level;
		logger_info internalLoggerInfo = { };
		// ##########################################################################################
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
	if( Logger::InternalLogger( ) != nullptr ) {                                                                                  \
		Logger::InternalLogger( )->trace( __VA_ARGS__ );                                                                      \
	}
#define SE_INTERNAL_DEBUG( ... )                                                                                                      \
	if( Logger::InternalLogger( ) != nullptr ) {                                                                                  \
		Logger::InternalLogger( )->debug( __VA_ARGS__ );                                                                      \
	}
#define SE_INTERNAL_INFO( ... )                                                                                                       \
	if( Logger::InternalLogger( ) != nullptr ) {                                                                                  \
		Logger::InternalLogger( )->info( __VA_ARGS__ );                                                                       \
	}
#define SE_INTERNAL_WARN( ... )                                                                                                       \
	if( Logger::InternalLogger( ) != nullptr ) {                                                                                  \
		Logger::InternalLogger( )->warn( __VA_ARGS__ );                                                                       \
	}
#define SE_INTERNAL_ERROR( ... )                                                                                                      \
	if( Logger::InternalLogger( ) != nullptr ) {                                                                                  \
		Logger::InternalLogger( )->error( __VA_ARGS__ );                                                                      \
	}
#define SE_INTERNAL_FATAL( ... )                                                                                                      \
	if( Logger::InternalLogger( ) != nullptr ) {                                                                                  \
		Logger::InternalLogger( )->critical( __VA_ARGS__ );                                                                   \
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
		if( Logger::ClientSideLogger( ) != nullptr ) {                                                                        \
			Logger::ClientSideLogger( )->trace( __VA_ARGS__ );                                                            \
		}
	#define SE_DEBUG( ... )                                                                                                       \
		if( Logger::ClientSideLogger( ) != nullptr ) {                                                                        \
			Logger::ClientSideLogger( )->debug( __VA_ARGS__ );                                                            \
		}
	#define SE_INFO( ... )                                                                                                        \
		if( Logger::ClientSideLogger( ) != nullptr ) {                                                                        \
			Logger::ClientSideLogger( )->info( __VA_ARGS__ );                                                             \
		}
	#define SE_WARN( ... )                                                                                                        \
		if( Logger::ClientSideLogger( ) != nullptr ) {                                                                        \
			Logger::ClientSideLogger( )->warn( __VA_ARGS__ );                                                             \
		}
	#define SE_ERROR( ... )                                                                                                       \
		if( Logger::ClientSideLogger( ) != nullptr ) {                                                                        \
			Logger::ClientSideLogger( )->error( __VA_ARGS__ );                                                            \
		}
	#define SE_FATAL( ... )                                                                                                       \
		if( Logger::ClientSideLogger( ) != nullptr ) {                                                                        \
			Logger::ClientSideLogger( )->critical( __VA_ARGS__ );                                                         \
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