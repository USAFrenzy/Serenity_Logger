#include <serenity/Helpers/LibLogger.h>
#include <cstdarg>

namespace serenity
{
	LibLogger::LibLogger( std::string name ) : m_name( name )
	{
		if( m_internalLogger == nullptr ) {
			CreateLibLogger( );
		}
	}
	const std::shared_ptr<spdlog::logger> &LibLogger::InternalLogger( )
	{
		return m_internalLogger;
	}
	// template <typename T, typename... Args> void LibLogger::se_trace( T &message, Args... args )
	//{
	//	if( InternalLogger( ) != nullptr ) {
	//		InternalLogger( )->trace( fmt::format( message, std::forward<Args>( args )... ) );
	//	}
	//}
	// template <typename T, typename... Args> void LibLogger::se_debug( T &message, Args... args )
	//{
	//	if( LibLogger::InternalLogger( ) != nullptr ) {
	//		LibLogger::InternalLogger( )->debug( fmt::format( message, std::forward<Args>( args )... ) );
	//	}
	//}
	// template <typename T, typename... Args> void LibLogger::se_info( T &message, Args... args )
	//{
	//	if( LibLogger::InternalLogger( ) != nullptr ) {
	//		LibLogger::InternalLogger( )->info( message, std::forward<Args>( args )... );
	//	}
	//}
	// template <typename T, typename... Args> void LibLogger::se_warn( T &message, Args... args )
	//{
	//	if( LibLogger::InternalLogger( ) != nullptr ) {
	//		LibLogger::InternalLogger( )->warn( fmt::format( message, std::forward<Args>( args )... ) );
	//	}
	//}
	// template <typename T, typename... Args> void LibLogger::se_error( T &message, Args... args )
	//{
	//	if( LibLogger::InternalLogger( ) != nullptr ) {
	//		LibLogger::InternalLogger( )->error( fmt::format( message, std::forward<Args>( args )... ) );
	//	}
	//}
	// template <typename T, typename... Args> void LibLogger::se_fatal( T &message, Args... args )
	//{
	//	if( LibLogger::InternalLogger( ) != nullptr ) {
	//		LibLogger::InternalLogger( )->critical( fmt::format( message, std::forward<Args>( args )... ) );
	//	}
	//}
	void LibLogger::se_assert( void *condition, std::string message, ... )
	{
		if( !( condition ) ) {
			se_fatal( "ASSERTION FAILED: {}\nIn File: {} On Line: {}\n{}", SE_MACRO_STRING( condition ),
				  std::filesystem::path( __FILE__ ).filename( ).string( ), ( __LINE__ ), ( fmt::format( message ) ) );
			SE_DEBUG_BREAK
		}
	}


	std::shared_ptr<spdlog::logger> LibLogger::InitLibLogger( logger_info infoStruct )
	{
		auto                         logDirPath = file_helper::current_path( ).append( "/Logs/Internal_Log/" );
		file_helper::directory_entry internalLogDir { logDirPath };
		internalLoggerInfo.level      = LoggerLevel::trace;  // after testing -> LoggerLevel::warning
		internalLoggerInfo.loggerName = "INTERNAL_LOGGER";
		internalLoggerInfo.logDir     = internalLogDir;
		internalLoggerInfo.logName    = "Internal_Log.txt";

		m_sinks.sinkVector.clear( );
		m_sinks.CreateSink( infoStruct );
		auto mappedLevel = map_helper::MapToMappedLevel( infoStruct.level );
		auto internalLogger =
		  std::make_shared<spdlog::logger>( infoStruct.loggerName, begin( m_sinks.sinkVector ), end( m_sinks.sinkVector ) );
		spdlog::register_logger( internalLogger );
		internalLogger->set_level( mappedLevel );
		internalLogger->flush_on( mappedLevel );
		return internalLogger;
	}


	void LibLogger::CreateLibLogger( )
	{
		internalLoggerInfo.sink_info.sinks.clear( );
		internalLoggerInfo.sink_info.sinks.emplace_back( SinkType::stdout_color_mt );
		m_internalLogger = std::move( InitLibLogger( internalLoggerInfo ) );
	}
	const std::string LibLogger::name( )
	{
		return m_name;
	}
	void LibLogger::Shutdown( )
	{
		spdlog::drop_all( );
		spdlog::shutdown( );
		m_internalLogger.reset( );
		m_internalLogger = nullptr;
	}

}  // namespace serenity