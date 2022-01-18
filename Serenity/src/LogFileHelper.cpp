#include <serenity/Helpers/LogFileHelper.h>
#include <serenity/Utilities/Utilities.h>
#include <serenity/Logger.h>

namespace serenity
{
	std::shared_ptr<InternalLibLogger> LogFileHelper::internalLogger;

	LogFileHelper::LogFileHelper( const file_helper::path pathToFile )
	{
		internalLogger = Logger::InternalLogger( );

		file_helper::path prefPath = pathToFile;
		prefPath.make_preferred( );

		prefPath._Remove_filename_and_separator( );
		file_helper::directory_entry logDir { prefPath };
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
		StorePathComponents( prefPath /= pathToFile.filename( ) );
	}

	bool LogFileHelper::OpenFile( const file_helper::path filePath, bool truncate )
	{
		auto path = filePath;
		path.make_preferred( );

		if( file_helper::exists( path ) ) {
			try {
				file_utils::OpenFile( path, truncate );
			}
			catch( const std::exception &e ) {
				internalLogger->fatal( "Exception Caught In OpenLog():\n%s\n", e.what( ) );
				return false;
			}
		}
		return true;
	}

	bool LogFileHelper::CloseFile( const file_helper::path filePath )
	{
		auto path = filePath;
		path.make_preferred( );

		if( file_helper::exists( path ) ) {
			try {
				file_utils::CloseFile( path );
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
		auto path = logDirPath;
		path.make_preferred( );

		internalLogger->trace( "Setting Log Directory Path To [{}]", path );
		m_logDirPath    = path;
		fileInfoChanged = true;
		internalLogger->trace( "Log Directory Path Successfully Set To [{}]", path );
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

	const file_helper::path LogFileHelper::LogFilePath( )
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
		auto path = pathToStore;
		path.make_preferred( );

		auto logEntry { path };
		logEntry._Remove_filename_and_separator( );
		m_logDirPath    = logEntry;
		m_filePath      = path;
		m_fileName      = path.filename( );
		fileInfoChanged = true;
	}

}  // namespace serenity
