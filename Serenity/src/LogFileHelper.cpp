#include <serenity/Helpers/LogFileHelper.h>
#include <serenity/Utilities/Utilities.h>
#include <serenity/Logger.h>

namespace serenity
{
	std::shared_ptr<InternalLibLogger> LogFileHelper::internalLogger;


	LogFileHelper::LogFileHelper( file_helper::directory_entry &logDir, std::string &fileName )
	{
		internalLogger = Logger::InternalLogger( );
		if( !logDir.exists( ) ) {
			try {
				internalLogger->trace( "Directory [{}] Not Found - Creating Directory Entry...", logDir.path().filename());
				file_helper::create_directories( logDir );
				internalLogger->info( "Successfully Created Directory At [{}]", logDir.path() );
			}
			catch( const std::exception &er ) {
				internalLogger->fatal( "Exception Caught In LogFileHelper():\n{}", er.what( ) );
			}
		}
		serenity::file_helper::path logDirPath = logDir.path( );
		SetLogDirPath( logDirPath );
		StorePathComponents( logDirPath/=fileName );
	}


	bool LogFileHelper::OpenFile( file_helper::path filePath )
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

	bool LogFileHelper::CloseFile( file_helper::path filePath )
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

	void LogFileHelper::SetLogDirPath( file_helper::path logDirPath )
	{
		internalLogger->trace( "Setting Log Directory Path To [{}]", logDirPath );
		m_logDirPath    = logDirPath;
		fileInfoChanged = true;
		internalLogger->trace( "Log Directory Path Successfully Set To [{}]", logDirPath );
	}


	void LogFileHelper::UpdateFileInfo( file_helper::path pathToFile )
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

	file_helper::path const LogFileHelper::LogFilePath( )
	{
		return m_filePath;
	}

	void LogFileHelper::SetLogFilePath( file_helper::path logPath )
	{
		m_filePath      = logPath;
		fileInfoChanged = true;
	}

	void LogFileHelper::StorePathComponents( file_helper::path &pathToStore )
	{
		m_filePath = pathToStore;
		m_fileName = pathToStore.filename( );
	}
}  // namespace serenity
