
#include <serenity/Logger.h>
#include <serenity/Utilities/Utilities.h>

#include <fstream>

#pragma warning( push, 0 )
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning( pop )

namespace serenity
{
	//std::shared_ptr<spdlog::logger> Logger::m_internalLogger;
	std::shared_ptr<spdlog::logger> Logger::m_clientLogger;
	std::unique_ptr<LogFileHelper>  Logger::logFileHandle;
	std::unique_ptr<InternalLibLogger>        Logger::internalLogger;

	static auto &libLogger = Logger::internalLogger->GetInstance( );

	std::string Logger::LogLevelToStr( LoggerLevel level )
	{
		std::map<LoggerLevel, std::string> levelMap = { { LoggerLevel::trace, "[trace]" }, { LoggerLevel::info, "[info]" },
								{ LoggerLevel::debug, "[debug]" }, { LoggerLevel::warning, "[warn]" },
								{ LoggerLevel::error, "[error]" }, { LoggerLevel::fatal, "[fatal]" } };
		std::string                        result   = "[off]";
		auto                               iterator = levelMap.find( level );
		if( iterator != levelMap.end( ) ) {
			result = iterator->second;
		}
		return result;
	}


	Logger::Logger( logger_info &infoStruct ) : initInfo( std::move( infoStruct ) )
	{
		auto              defaultPath = file_helper::current_path( );
		file_helper::path logDirPath  = initInfo.logDir.path( );
		auto              filePath    = logDirPath.string( ) + "\\" + initInfo.logName;

		logFileHandle = std::make_unique<LogFileHelper>( initInfo.logDir, initInfo.logName );
		m_sinks       = std::make_unique<Sink>( );
		internalLogger->GetInstance( );
		internalLogger->GetInstance().EnableInternalLogging( );
		//##########################################################################################################
		/*
		  Some possible features to add:
		  - The ability to enable internal logging support independantly of client logger [X]
		  - Ties in with the above point, but also have the ability to turn off internal logging when in release [.5X]
		  - The ability to customize the internal logger? I mean, I'm using stdout sink here, but hey, who knows? [ ]
		*/
		//##########################################################################################################
		// Set The Paths
		FileHelperHandle( )->SetLogDirPath( logDirPath );
		
		libLogger.trace( "Setting Log Directory Path To [{}]", logDirPath );
		FileHelperHandle( )->UpdateFileInfo( filePath );
		libLogger.trace( "Updated File Info For Path [{}]", filePath );
		this->UpdateInfo( );
		// Creating Client Logger
		m_sinks->clear_sinks( );
		m_sinks->set_sinks( initInfo.sink_info.sinks );
		m_clientLogger = std::move( CreateLogger( initInfo ) );
		libLogger.trace( "Logger [{}] Successfully Initialized", initInfo.loggerName );
	}

	Logger::~Logger( )
	{
		Shutdown( );
	}

	//void Logger::CreateInternalLogger( logger_info &infoStruct )
	//{
	//	// Creating Internal Logger
	//	internalLoggerInfo.level      = LoggerLevel::trace;  // after testing -> LoggerLevel::warning
	//	internalLoggerInfo.loggerName = "SERENITY";
	//	file_helper::directory_entry internalDir { initInfo.logDir.path( ) };
	//	internalLoggerInfo.logDir  = internalDir;
	//	internalLoggerInfo.logName = "Internal_Log.txt";
	//	internalLoggerInfo.sink_info.sinks.clear( );
	//	internalLoggerInfo.sink_info.sinks.emplace_back( SinkType::stdout_color_mt );
	//	m_sinks->clear_sinks( );
	//	m_sinks->set_sinks( internalLoggerInfo.sink_info.sinks );
	//	m_internalLogger = std::move( CreateLogger( internalLoggerInfo, true ) );
	//	SE_INTERNAL_INFO( "Logger [{}] Successfully Initialized", internalLoggerInfo.loggerName );
	//}

	void Logger::CloseLog( file_helper::path filePath )
	{
		if( file_helper::exists( filePath ) ) {
			try {
				libLogger.trace( "Closing File [{}]...", filePath.filename( ) );
				file_utils::CloseFile( filePath );
				libLogger.trace( "File [{}] Successfully Closed", filePath.filename( ) );
			}
			catch( const std::exception &e ) {
				libLogger.error( "Exception Thrown In CloseLog():\n%s\n", e.what( ) );
			}
		}
	}

	void Logger::OpenLog( file_helper::path filePath )
	{
		if( file_helper::exists( filePath ) ) {
			try {
				libLogger.trace( "Opening File [{}]...", filePath.filename( ) );
				file_utils::OpenFile( filePath );
				libLogger.trace( "File [{}] Successfully Opened", filePath.filename( ) );
			}
			catch( const std::exception &e ) {
				libLogger.error( "Exception Thrown In OpenLog():\n%s\n", e.what( ) );
			}
		}
	}


	void Logger::StopLogger( )
	{
		libLogger.trace( "Stopping Logger..." );
		CloseLog( FileHelperHandle( )->LogFilePath( ) );
		DropLogger( );  // Drop client/internal loggers w/o shutting spdlog down
		libLogger.trace( "Logger Has Been Stopped" );
	}

	void Logger::StartLogger( )
	{
		libLogger.trace( "Starting Logger(s)..." );
		// DropLogger() resets m_sinks ptr -> have to recreate sink handle for CreateLogger()
		if( m_sinks == nullptr ) {
			m_sinks = std::make_unique<Sink>( );
			m_sinks->clear_sinks( );
			libLogger.trace( "Sinks Handle Has Been Successfully Re-Initialized" );
		}
		if( m_clientLogger == nullptr ) {
			m_sinks->set_sinks( initInfo.sink_info.sinks );
			m_clientLogger = CreateLogger( initInfo );
			libLogger.trace( "Client Logger Has Been Successfully Re-Initialized" );
		}
		OpenLog( FileHelperHandle( )->LogFilePath( ) );
		libLogger.trace( "Logger Has Been Successfully Restarted" );
	}

	void Logger::Shutdown( )
	{
		StopLogger( );
		libLogger.trace( "Shutting Down..." );
		spdlog::drop_all( );
		spdlog::shutdown( );
		m_sinks.reset( );
		m_clientLogger.reset( );
	}

	void Logger::DropLogger( )
	{
		libLogger.trace( "Dropping Logger And Resetting Handles...", m_clientLogger->name( ) );
		spdlog::drop( m_clientLogger->name( ) );
		libLogger.trace( "Logger [{}] Has Been Dropped", m_clientLogger->name( ) );
		m_sinks.reset( );
		libLogger.trace( "Sinks Handle For Logger Has Been Reset" );
		m_clientLogger.reset( );
		libLogger.trace( "Logger Handle Has Been Reset" );
	}

	std::shared_ptr<spdlog::logger> Logger::CreateLogger( logger_info &infoStruct)
	{
		m_sinks->sinkVector.clear( );
		m_sinks->CreateSink( infoStruct );

		libLogger.trace( "Sinks For Logger [{}] Have Been Succesfully Created", infoStruct.loggerName );
		auto mappedLevel = ToMappedLevel( infoStruct.level );

			if( GetGlobalLevel( ) != infoStruct.level ) {
				mappedLevel = ToMappedLevel( GetGlobalLevel( ) );
			}
			std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>(
			  infoStruct.loggerName, begin( m_sinks->sinkVector ), end( m_sinks->sinkVector ) );
			libLogger.trace( "Logger [{}] Has Been Successfully Created", infoStruct.loggerName );

			spdlog::register_logger( logger );
			libLogger.trace( "Logger [{}] Has Been Registered", infoStruct.loggerName );

			logger->set_level( mappedLevel );
			libLogger.trace( "Logger [{}] Level Has Been Set To: {}", infoStruct.loggerName,
					      LogLevelToStr( infoStruct.level ) );

			logger->flush_on( mappedLevel );
			libLogger.trace( "Logger [{}] Flush Level Has Been Set To: {}", infoStruct.loggerName,
					   LogLevelToStr( infoStruct.level ) );

			return logger;
	}

	bool Logger::RenameLog( std::string newName, bool replaceIfExists )
	{
		// In Case Path Is Passed In
		file_helper::path newFilePath = newName;
		const auto &      tmpPath     = initInfo.logDir.path( );
		file_helper::path oldPath     = tmpPath.string( ).append( "\\" + initInfo.logName );
		file_helper::path newPath     = tmpPath.string( ).append( "\\" + newFilePath.filename( ).string( ) );
		libLogger.trace( "Renaming Log File [{}] To [{}]", oldPath.filename( ), newPath.filename( ) );
		try {
			// Flush and close logs, Drop Loggers From spdlog's registry so sinks
			// ref to files are dropped, and reset the logger pointers
			StopLogger( );
			if( file_helper::exists( newPath ) ) {
				libLogger.warn( "File [{}] Already Exists", newPath.filename( ) );
				if( replaceIfExists ) {
					libLogger.trace( "Parameter \"replaceIfExists\" = true." );
					libLogger.trace( "Removing [{}] Before Renaming [{}] To [{}].", newPath.filename( ),
							      oldPath.filename( ), newPath.filename( ) );
					file_utils::RemoveEntry( newPath );
					libLogger.trace( "Removed File [{}]", newPath.filename( ) );
				}
				else {
					libLogger.trace( "Parameter \"replaceIfExists\" = false." );
					libLogger.trace( "Copying File Contents: \n- From:\t[{}] \n- To:\t[{}]", oldPath, newPath );
					file_utils::CopyContents( oldPath, newPath );
					libLogger.trace( "Copying Contents Finished. Removing File [{}]", oldPath.filename( ) );
					file_utils::RemoveEntry( oldPath );
					libLogger.trace( "Removed File Link From [{}]", oldPath.filename( ) );
				}
			}
			file_utils::RenameFile( oldPath, newPath );
			libLogger.info( "Renaming [{}] To [{}] Was Successful.\n", oldPath.filename( ), newPath.filename( ) );
			// Effectively Updating Variables
			initInfo.logName = newPath.filename( ).string( );
			libLogger.trace( "Log File Name Has Been Updated To Reflect Rename To [{}]", newPath.filename( ) );
			FileHelperHandle( )->UpdateFileInfo( newPath );
			libLogger.trace( "File Info Has Been Updated Based On Path:\n{}", newPath );
			UpdateInfo( );
			libLogger.trace( "Logger Has Been Updated For File Path Changes" );
			StartLogger( );
			return true;
		}
		catch( const std::exception &e ) {
			libLogger.error( "Renaming Failed." );
			libLogger.error( "EXCEPTION CAUGHT IN RenameLog():\n{}\n", e.what( ) );
			return false;
		}
	}

	void Logger::UpdateInfo( )
	{
		libLogger.trace( "Checking For File Info Changes..." );
		if( FileHelperHandle( )->fileInfoChanged ) {
			libLogger.trace( "File Info Has Changed. Updating File Info Handles" );
			// Sometimes is called already by caller, however, ensures everything is up to date
			FileHelperHandle( )->UpdateFileInfo( FileHelperHandle( )->LogFilePath( ) );
			libLogger.trace( "File Info Handles Have Been Updated" );
		}
		else {
			libLogger.trace( "File Info Already Up-To-Date" );
		}
		FileHelperHandle( )->fileInfoChanged = false;
	}

	std::string const Logger::LoggerName( )
	{
		return initInfo.loggerName;
	}

	const LoggerLevel Logger::GetLogLevel( )
	{
		return ToLogLevel( m_clientLogger->level( ) );
	}

	void Logger::SetLogLevel( LoggerLevel level)
	{
		libLogger.trace( "Setting Logger Level..." );
		auto m_level = ToMappedLevel( level );
		if( m_level == MappedLevel::n_levels ) {
			libLogger.warn( "Log Level Was Not A Valid Value - Log Level Set To [off]" );
			m_clientLogger->set_level( MappedLevel::off );
		}
		else {
			m_clientLogger->set_level( m_level );
			libLogger.trace( "Log Level Successfully Set To: [{}]", LogLevelToStr( level ) );
		}
	}

	bool Logger::ShouldLog( )
	{
		return ( GetLogLevel( ) <= GetGlobalLevel( ) ) ? true : false;
	}


	// MISC Functions
	void SetGlobalLevel( LoggerLevel level )
	{
		global_level = level;
		if( Logger::ClientSideLogger( ) != nullptr ) {
			Logger::ClientSideLogger( )->set_level( ToMappedLevel( level ) );
		}
		//Logger::internalLogger.InternalLogger( )->set_level( ToMappedLevel( level ) );
	}
}  // namespace serenity
