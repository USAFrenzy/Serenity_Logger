#pragma once


#pragma warning( push )
#pragma warning( disable : 26812 )
#include <spdlog/fmt/ostr.h>
#pragma warning( pop )

#include <serenity/Common.h>
#include <serenity/Helpers/LogFileHelper.h>
#include <serenity/Helpers/LibLogger.h>
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

		std::shared_ptr<spdlog::logger> CreateLogger( logger_info &infoStruct );
		void                            StartLogger( );
		void                            StopLogger( );
		void                            Shutdown( );
		void                            SetLoggerLevel( LoggerLevel logLevel );
		std::string const               LoggerName( );
		bool                            RenameLog( std::string newName );
		// In The Same Fashion As The Note From LogFileHelper, Just Learned Of CVs Which Seems Perfect For This
		void                                          UpdateFileInfo( ) override;
		void                                          OpenLog( std::string filePath );
		void                                          CloseLog( std::string loggerName );
		const std::unique_ptr<LogFileHelper> &        FileHelperHandle( );
		static const std::shared_ptr<spdlog::logger> &ClientLogger( )
		{
			return m_clientLogger;
		}

	      private:
		logger_info                            initInfo = { };
		static std::shared_ptr<spdlog::logger> m_clientLogger;
		std::unique_ptr<LogFileHelper>         logFileHandle;
		std::unique_ptr<LibLogger>             libLoggerhandle;
		Sink                                   m_sinks;
		std::mutex                             m_mutex;

	      private:
		const std::unique_ptr<LibLogger> &GetLibLogger( );
	};

}  // namespace serenity


#if defined( SERENITY_TEST_RUN ) || !defined( NDEBUG )
   // Client side macros
	#define SE_TRACE( ... )                                                                                                       \
		if( Logger::ClientLogger( ) != nullptr ) {                                                                            \
			Logger::ClientLogger( )->trace( __VA_ARGS__ );                                                                \
		}
	#define SE_DEBUG( ... )                                                                                                       \
		if( Logger::ClientLogger( ) != nullptr ) {                                                                            \
			Logger::ClientLogger( )->debug( __VA_ARGS__ );                                                                \
		}
	#define SE_INFO( ... )                                                                                                        \
		if( Logger::ClientLogger( ) != nullptr ) {                                                                            \
			Logger::ClientLogger( )->info( __VA_ARGS__ );                                                                 \
		}
	#define SE_WARN( ... )                                                                                                        \
		if( Logger::ClientLogger( ) != nullptr ) {                                                                            \
			Logger::ClientLogger( )->warn( __VA_ARGS__ );                                                                 \
		}
	#define SE_ERROR( ... )                                                                                                       \
		if( Logger::ClientLogger( ) != nullptr ) {                                                                            \
			Logger::ClientLogger( )->error( __VA_ARGS__ );                                                                \
		}
	#define SE_FATAL( ... )                                                                                                       \
		if( Logger::ClientLogger( ) != nullptr ) {                                                                            \
			Logger::ClientLogger( )->critical( __VA_ARGS__ );                                                             \
		}
	#define SE_ASSERT( condition, message, ... )                                                                                  \
		if( !( condition ) ) {                                                                                                \
			SE_FATAL( "ASSERTION FAILED: {}\nIn File: {} On Line: {}\n{}", SE_MACRO_STRING( condition ),                  \
				  std::filesystem::path( __FILE__ ).filename( ).string( ), ( __LINE__ ),                              \
				  ( SE_VAR_MSG( message, __VA_ARGS__ ) ) );                                                           \
			SE_DEBUG_BREAK                                                                                                \
		}

#else
	#define SE_ASSERT_VAR_MSG( message, ... )    ( void ) 0
	#define SE_TRACE( ... )                      ( void ) 0
	#define SE_DEBUG( ... )                      ( void ) 0
	#define SE_INFO( ... )                       ( void ) 0
	#define SE_WARN( ... )                       ( void ) 0
	#define SE_ERROR( ... )                      ( void ) 0
	#define SE_FATAL( ... )                      ( void ) 0
	#define SE_ASSERT( condition, message, ... ) ( void ) 0
#endif  // SERENITY_TEST_RUN