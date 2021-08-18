#include "serenity/LogFileHelper.hpp"
#include "serenity/Defines.hpp"

namespace serenity {

	bool LogFileHelper::fileInfoChanged = false;
	/*
		For The Constuctors, would be helpful to take in a path, search for the root, find the whole path
		from root to end of input path, then break into component paths and set the member vairables
		respective of those paths
	*/

	LogFileHelper::LogFileHelper( )
	  : m_currentDir(file_helper::current_path( )),
	    m_logDir("Logs"),
	    m_logDirPath(m_currentDir /= m_logDir),
	    m_logName("Log.txt"),
	    m_filePath(m_logDirPath /= m_logName),
	    m_fileName(m_filePath.filename( ))
	{
	}
	LogFileHelper::LogFileHelper(file_helper::path& logDir, file_helper::path& fileName)
	  : m_currentDir(file_helper::current_path( )),
	    m_logDir(logDir.string( )),
	    m_logDirPath(m_currentDir /= m_logDir),
	    m_logName(fileName.string( )),
	    m_filePath(m_logDirPath /= m_logName),
	    m_fileName(m_filePath.filename( ))
	{
	}

	std::optional<file_helper::path> LogFileHelper::UpdateFileInfo(optPath optCurrentDir,
								       optPath optLogDir,
								       optPath optFilePath,
								       optPath optFileName)
	{
		if(!optCurrentDir.has_value( )) {
			m_currentDir = file_helper::current_path( );
		}
		if(!optLogDir.has_value( )) {
			m_logDirPath = m_currentDir /= m_logDir;
		}
		if(!optFilePath.has_value( )) {
			m_filePath = m_logDirPath /= m_logName;
		}
		if(!optFileName.has_value( )) {
			m_fileName = m_filePath.filename( );
		}
		return std::nullopt;
	}
	LogFileHelper::~LogFileHelper( )
	{
		m_currentDir.clear( );
		m_logDirPath.clear( );
		m_filePath.clear( );
		m_fileName.clear( );
		UpdateFileInfo("", "", "", "");
	}

	void LogFileHelper::NotifyLogger( )
	{
		fileInfoChanged = true;
	}
	void LogFileHelper::RenameFile(std::string fileNewName)
	{
		m_filePath.replace_filename(fileNewName);
		UpdateFileInfo(optCurrentDir, optLogDir, m_filePath, optFileName);
	}

	// Helper functions

	file_helper::path LogFileHelper::GetFileName( )
	{
		return m_fileName;
	}
	file_helper::path LogFileHelper::GetFilePath( )
	{
		return m_filePath;
	}
	file_helper::path LogFileHelper::GetDirPath( )
	{
		return m_logDirPath;
	}
	file_helper::path LogFileHelper::GetCurrentDir( )
	{
		return m_currentDir;
	}
} // namespace serenity