#pragma once


#include <serenity/Interfaces/IObserver.h>
#include <serenity/Common.h>
#include <serenity/Helpers/LogFileHelper.h>
#include <serenity/Sinks/Sinks.h>

#include "../experimental/LibLogger.h"

#pragma warning( push, 0 )
#include <spdlog/fmt/ostr.h>
#pragma warning( pop )

namespace serenity
{
	class Logger : public ILogger
	{
	      public:
		explicit Logger( logger_info &infoStruct );
		Logger( )                     = delete;
		Logger( const Logger &copy )  = delete;
		Logger( const Logger &&move ) = delete;
		Logger &operator=( const Logger & ) = delete;
		~Logger( );

		static const std::shared_ptr<spdlog::logger> &ClientSideLogger( )
		{
			return m_clientLogger;
		}
		static const std::unique_ptr<LogFileHelper> &FileHelperHandle( )
		{
			return logFileHandle;
		}
		void                                         StartLogger( );
		void                                         StopLogger( );
		void                                         DropLogger( );
		void                                         Shutdown( );
		void                                         SetLogLevel( LoggerLevel logLevel ) override;
		const LoggerLevel                            GetLogLevel( );
		std::string                                  LogLevelToStr( LoggerLevel level ) override;
		std::string const                            LoggerName( );
		bool                                         RenameLog( std::string newName, bool replaceIfExists = true );
		void                                         UpdateInfo( ) override;
		void                                         OpenLog( file_helper::path filePath );
		void                                         CloseLog( file_helper::path filePath );
		template <typename T, typename... Args> void se_trace( T message, Args &&...args );
		template <typename T, typename... Args> void se_debug( T message, Args &&...args );
		template <typename T, typename... Args> void se_info( T message, Args &&...args );
		template <typename T, typename... Args> void se_warn( T &message, Args &&...args );
		template <typename T, typename... Args> void se_error( T message, Args &&...args );
		template <typename T, typename... Args> void se_fatal( T message, Args &&...args );
		void                                         ChangeInternalLoggerOptions( se_internal::internal_logger_info options );

	      private:
		logger_info                            initInfo           = { };
		se_internal::internal_logger_info      internalLoggerInfo = { };
		static std::shared_ptr<spdlog::logger> m_clientLogger;
		static std::unique_ptr<LogFileHelper>  logFileHandle;
		std::unique_ptr<Sink>                  m_sinks;
		std::unique_ptr<InternalLibLogger>     internalLogger;

	      private:
		std::shared_ptr<spdlog::logger> CreateLogger( logger_info &infoStruct );
		bool                            ShouldLog( ) override;
	};

	static LoggerLevel  global_level { LoggerLevel::trace };

	void                SetGlobalLevel( LoggerLevel level );
	static LoggerLevel &GetGlobalLevel( )
	{
		return global_level;
	}

#include <serenity/Logger-impl.h>
}  // namespace serenity

#ifndef NDEBUG
	#define SE_ASSERT( condition, message, ... )                                                                                  \
		if( !( condition ) ) {                                                                                                \
			Logger::se_fatal( "ASSERTION FAILED: {}\nIn File: {} On Line: {}\n{}", SE_MACRO_STRING( condition ),          \
					  std::filesystem::path( __FILE__ ).filename( ).string( ), ( __LINE__ ),                      \
					  ( SE_ASSERT_VAR_MSG( message, __VA_ARGS__ ) ) );                                            \
			SE_DEBUG_BREAK( );                                                                                            \
		}
#else
	#define SE_ASSERT( condition, message, ... ) ( void ) 0
#endif  // !NDEBUG