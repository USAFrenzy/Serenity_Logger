#include <serenity/Helpers/LogFileHelper.h>

namespace serenity
{
	LogFileHelper::LogFileHelper( file_helper::directory_entry &logDir, std::string &fileName )
	{
		serenity::file_helper::path logDirPath, logPathToFile;
		auto                        defaultPath = file_helper::current_path( );

		if( !logDir.exists( ) ) {
			try {
				serenity::file_helper::create_directories( logDir );
			}
			catch( const std::exception &er ) {
				printf( "Exception Caught In LogFileHelper():\n%s", er.what( ) );
			}
		}
		logDirPath = logDir.path( );
		SetLogDirPath( logDirPath );
		auto defaultFilePath = logDirPath /= fileName;
		StorePathComponents( defaultPath );
	}


	void LogFileHelper::SetLogDirPath( file_helper::path logDirPath )
	{
		m_logDirPath    = logDirPath;
		fileInfoChanged = true;
	}


	void LogFileHelper::UpdateFileInfo( file_helper::path pathToFile )
	{
		StorePathComponents( pathToFile );
		NotifyLogger( );
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
