#include <serenity/Helpers/LogFileHelper.h>
#include <serenity/Utilities/Utilities.h>

namespace serenity
{
	bool LogFileHelper::fileInfoChanged = false;
	// clang-format off
	/*
		For The Constuctors, would be helpful to take in a path, search for the root, find the whole path
		from root to end of input path, then break into component paths and set the member vairables
		respective of those paths
		
		- Also Would Like To Just Have The Constructors Look Like:
		
		LogFileHelper::LogFileHelper()
		: m_logDir("Logs"), m_logName("Log.txt")
		{
			StorePathComponents();
		}
	*/
	// clang-format on

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
	LogFileHelper::~LogFileHelper( )
	{
		m_fileName.clear( );
		m_logDirPath.clear( );
		m_filePath.clear( );
		UpdateFileInfo( m_filePath );
	}

	void LogFileHelper::NotifyLogger( )
	{
		fileInfoChanged = true;
	}

	file_helper::path const LogFileHelper::FileName( )
	{
		if( m_fileName.has_extension( ) )  // i.e Not A Directory
		{
			return m_fileName;
		}
		else {
			return "";
		}
	}
	file_helper::path const LogFileHelper::LogFilePath( )
	{
		return m_filePath;
	}
	file_helper::path const LogFileHelper::LogDirPath( )
	{
		return m_logDirPath;
	}

	void LogFileHelper::SetLogFilePath( file_helper::path logPath )
	{
		m_filePath      = logPath;
		fileInfoChanged = true;
	}
	file_helper::path const LogFileHelper::CurrentDir( )
	{
		return m_currentDir;
	}
	void LogFileHelper::SetDir( file_helper::path oldPathDir, file_helper::path destDirPath )
	{
		oldPathDir      = destDirPath;
		m_cachePath     = destDirPath;
		m_cacheDir      = m_cachePath.stem( );
		fileInfoChanged = true;
	}

	void LogFileHelper::StorePathComponents( file_helper::path &pathToStore )
	{
		m_filePath     = pathToStore;
		m_currentDir   = file_helper::current_path( );
		m_cachePath    = pathToStore;
		m_rootPath     = pathToStore.root_path( );
		m_rootDir      = pathToStore.root_directory( );
		m_rootName     = pathToStore.root_name( );
		m_parentPath   = pathToStore.parent_path( );
		m_relativePath = pathToStore.relative_path( );
		m_pathStem     = pathToStore.stem( );
		m_cacheDir     = m_pathStem;
		m_fileName     = pathToStore.filename( );
	}

	void LogFileHelper::OpenFile( file_helper::path file, bool truncate )
	{
		try {
			file_utils::OpenFile( file, truncate );
		}
		catch( const std::exception &e ) {
			auto msg = fmt::format( "Error In LogFileHelper::OpenFile() For File [{}]:\nReason: {}",
						file.filename( ).string( ), e.what( ) );
			printf( msg.c_str( ) );
		}
	}

	void LogFileHelper::CloseFile( file_helper::path file )
	{
		try {
			file_utils::CloseFile( file );
		}
		catch( const std::exception &e ) {
			auto msg = fmt::format( "Error In LogFileHelper::CloseFile() For File [{}]:\nReason: {}",
						file.filename( ).string( ), e.what( ) );
			printf( msg.c_str( ) );
		}
	}
	// clang-format off
	// ############################################################################### Testing Functions  ###############################################################################
	// clang-format on

	std::string const LogFileHelper::PathComponents_Str( file_helper::path path )
	{
		auto pPath        = path.string( );
		auto rootPath     = path.root_path( ).string( );
		auto rootDir      = path.root_directory( ).string( );
		auto rootName     = path.root_name( ).string( );
		auto parentPath   = path.parent_path( ).string( );
		auto relativePath = path.relative_path( ).string( );
		auto pathStem     = path.stem( ).string( );
		return "\nFor The Path: " + pPath + "\n\t#################### Path Components ####################\n\tRoot Path:\t" +
		       rootPath + "\n\tRoot Name:\t" + rootName + "\n\tRoot Dir:\t" + rootDir + "\n\tRelative Path:\t" + relativePath +
		       "\n\tParent Path:\t" + parentPath + "\n\tPath Stem:\t" + pathStem;
	}
}  // namespace serenity
