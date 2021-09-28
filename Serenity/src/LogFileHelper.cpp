#include <serenity/Helpers/LogFileHelper.h>
#include <serenity/Utilities/Utilities.h>
#include <serenity/Logger.h>

namespace serenity
{
	std::shared_ptr<InternalLibLogger> LogFileHelper::internalLogger;

	LogFileHelper::LogFileHelper( const file_helper::path pathToFile )
	{
		internalLogger = Logger::InternalLogger( );

		file_helper::path logEntry = pathToFile;
		logEntry._Remove_filename_and_separator( );
		file_helper::directory_entry logDir { logEntry };
		if( !logDir.exists( ) ) {
			try {
				internalLogger->trace( "Directory [{}] Not Found - Creating Directory Entry...",
						       logDir.path( ).filename( ) );
				file_helper::create_directories( logDir );
				internalLogger->info( "Successfully Created Directory At [{}]", logDir.path( ) );
			}
			catch( const std::exception &er ) {
				internalLogger->fatal( "Exception Caught In LogFileHelper():\n{}", er.what( ) );
			}
		}
		SetLogDirPath( logDir.path( ) );
		StorePathComponents( pathToFile );
	}

	bool LogFileHelper::OpenFile( const file_helper::path filePath )
	{
		if( file_helper::exists( filePath ) ) {
			try {
				file_utils::OpenFile( filePath );
			}
			catch( const std::exception &e ) {
				internalLogger->fatal( "Exception Caught In OpenLog():\n%s\n", e.what( ) );
				return false;
			}
		}
		return true;
	}

	void LogFileHelper::Flush( )
	{
		file_utils::Flush( LogFilePath( ) );
	}

	bool LogFileHelper::CloseFile( const file_helper::path filePath )
	{
		if( file_helper::exists( filePath ) ) {
			try {
				file_utils::CloseFile( filePath );
			}
			catch( const std::exception &e ) {
				internalLogger->fatal( "Exception Caught In CloseLog():\n%s\n", e.what( ) );
				return false;
			}
		}
		return true;
	}

	void LogFileHelper::SetLogDirPath( const file_helper::path logDirPath )
	{
		internalLogger->trace( "Setting Log Directory Path To [{}]", logDirPath );
		m_logDirPath    = logDirPath;
		fileInfoChanged = true;
		internalLogger->trace( "Log Directory Path Successfully Set To [{}]", logDirPath );
	}


	void LogFileHelper::UpdateFileInfo( const file_helper::path pathToFile )
	{
		internalLogger->trace( "Updating File Info..." );
		internalLogger->trace( "Storing Path Components..." );
		StorePathComponents( pathToFile );
		internalLogger->info( "Path Components Successfully Stored" );
		internalLogger->trace( "Notifying Logger Of Changes..." );
		NotifyLogger( );
		internalLogger->info( "Logger Successfully Notified Of Changes" );
		internalLogger->info( "File Info Successfully Updated" );
	}

	void LogFileHelper::NotifyLogger( )
	{
		fileInfoChanged = true;
	}

	const file_helper::path serenity::LogFileHelper::LogFilePath( )
	{
		return m_filePath;
	}
	const std::string LogFileHelper::LogName( )
	{
		return m_fileName.string( );
	}

	const file_helper::path LogFileHelper::RelativePathToLog( )
	{
		return LogFilePath( ).relative_path( );
	}

	const file_helper::directory_entry LogFileHelper::LogDir( )
	{
		return file_helper::directory_entry { m_logDirPath };
	}


	void LogFileHelper::SetLogFilePath( const file_helper::path logPath )
	{
		m_filePath      = logPath;
		fileInfoChanged = true;
	}

	// Since I'm No Longer Storing Every Component Of A Path - This Might Be A Little Extra...
	// Really Just Keeping This At The Moment If, later On, I Decide To Add Some Relative
	// Pathing Type Of Functions And The Like
	void LogFileHelper::StorePathComponents( const file_helper::path &pathToStore )
	{
		auto logEntry { pathToStore };
		logEntry._Remove_filename_and_separator( );
		m_logDirPath    = logEntry;
		m_filePath      = pathToStore;
		m_fileName      = pathToStore.filename( );
		fileInfoChanged = true;
	}

}  // namespace serenity
