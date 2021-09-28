#pragma once


#include <serenity/Interfaces/IObserver.h>
#include <serenity/Common.h>
#include <serenity/Helpers/LogFileHelper.h>
#include <serenity/Helpers/LibLogger.h>
#include <serenity/Sinks/Sinks.h>


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
		static const std::shared_ptr<InternalLibLogger> &InternalLogger( )
		{
			return internalLogger;
		}

		void                                         StartLogger( );
		void                                         StopLogger( );
		void                                         DropLogger( );
		void                                         Shutdown( );
		void                                         SetLogLevel( LoggerLevel logLevel ) override;
		void                                         SetFlushLevel( LoggerLevel flushLevel ) override;
		const LoggerLevel                            GetFlushLevel( );
		const LoggerLevel                            GetLogLevel( );
		bool                                         ShouldLog( ) override;
		std::string                                  LogLevelToStr( LoggerLevel level ) override;
		const std::string                            LoggerName( );
		const std::string                            LogName( );
		bool                                         RenameLog( const std::string newName, bool replaceIfExists = true );
		bool                                         WriteToNewLog( const std::string newLogName );
		void                                         UpdateInfo( ) override;
		void                                         OpenLog( const file_helper::path filePath );
		void                                         CloseLog( const file_helper::path filePath );
		template <typename T, typename... Args> void se_trace( T message, Args &&...args );
		template <typename T, typename... Args> void se_debug( T message, Args &&...args );
		template <typename T, typename... Args> void se_info( T message, Args &&...args );
		template <typename T, typename... Args> void se_warn( T &message, Args &&...args );
		template <typename T, typename... Args> void se_error( T message, Args &&...args );
		template <typename T, typename... Args> void se_fatal( T message, Args &&...args );
		// clang-format off
		// ************************ ChangeInternalLoggerOptions( se_internal::internal_logger_info options ) ************************ 
		/*	
		 *	More So Helpful If Using This To Debug The Current Application Using This Library Or Customize Internal Logger While
		 *	Working On An Application Using This Library, Otherwise, The Internal Logger Is Supposed To Mimic A Validation Layer
		 *	For This Library And Is Default Set To Log To The Console With "Trace" Levels Set For Log/Flush Levels And Defaulted
		 *	To "Off"
		 *	To Enable Internal Logging, You Can Do Either One Of The Following:
		 *	Option 1) InternalLibLogger::EnableInternalLogging();		// Statically
		 *	Option 2) Logger log(infoStruct);
		 *		->   log.InternalLogger()->EnableInternalLogging();	// Object Based
		 **************************************************************************************************************************/
		void ChangeInternalLoggerOptions( se_internal::internal_logger_info options );
		// clang-format on

	      private:
		logger_info                               initInfo           = { };
		se_internal::internal_logger_info         internalLoggerInfo = { };
		static std::shared_ptr<spdlog::logger>    m_clientLogger;
		static std::unique_ptr<LogFileHelper>     logFileHandle;
		std::unique_ptr<Sink>                     m_sinks;
		static std::shared_ptr<InternalLibLogger> internalLogger;

	      private:
		void CreateLogger( logger_info &infoStruct );
	};

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