#include "serenity/LogFileHelper.hpp"
#include "serenity/Defines.hpp"
#include <regex>

// Just For Simple Test func()
#include <iostream>
// Unfortunate Really, But To Use The Logger Type Functions, This Is Required
#include <serenity/Logger.hpp>


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
		NotifyLogger( );
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
	// clang-format off
		/*
			 \A(?!(?:COM[0-9]|CON|LPT[0-9]|NUL|PRN|AUX|com[0-9]|con|lpt[0-9]|nul|prn|aux)(\.|\z)|\s|[\.]{2,})[^\\\/:*\"?<>|]{1,254}(?<![\s\.])\z

			This Was Found On stackoverflow via Krzysztof Karski's and Andreas Zita's answer to validating files: 
			Regex That Will Validate The Filename Including Reserved Words -> Doesn't Allow:
			- names sarting with ".."
			- names ending with "."
			- ending with whitespace
			# This Is Much Better Than What I Came Up With, Which Was Limited In Scope And May Not Have Even Worked -> ^\.[a-zA-Z0-9]+|[a-zA-Z0-9]+\.| ^\.[a-zA-Z0-9]+\.
		*/
	// clang-format on

	void LogFileHelper::RenameFile(std::string fileNewName)
	{
		std::smatch match;
		std::regex validateFile("^[a-zA-Z0-9._ -]+[.]$");
		if(!(std::regex_search(fileNewName, match, validateFile))) {
			SE_ERROR("ERROR: File Name [ {} ] Contains Invalid Characters Or "
				 "Extension Is Invalid",
				 fileNewName);
		}
		std::string extension;
		/*
			Check to see if fileNewName contains it's own extension and throw away the stored
		    extension if so, otherwise, retain the old extension format
		*/
		if(!(fileNewName.find_last_of(".") != std::string::npos)) {
			extension = m_filePath.filename( ).extension( ).string( );
		}
		m_filePath.replace_filename(fileNewName + extension);
		UpdateFileInfo(optCurrentDir, optLogDir, m_filePath, m_logName);
	}

	void LogFileHelper::ChangeDir(file_helper::path destDir)
	{
		std::error_code ec;
		file_helper::current_path(destDir, ec);
		m_currentDir = file_helper::current_path( );
		UpdateFileInfo(m_currentDir, optLogDir, optFilePath, m_fileName);
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
	// clang-format off
	// ############################################################################### Testing Functions  ###############################################################################
	// clang-format on

	std::string LogFileHelper::PathComponents_Str(file_helper::path& path)
	{
		auto pPath        = path.string( );
		auto rootPath     = path.root_path( ).string( );
		auto rootDir      = path.root_directory( ).string( );
		auto rootName     = path.root_name( ).string( );
		auto parentPath   = path.parent_path( ).string( );
		auto relativePath = path.relative_path( ).string( );
		auto pathStem     = path.stem( ).string( );
		return "\nFor The Path: " + pPath +
		       "\n\t#################### Path Components ####################\n\tRoot Path:\t" + rootPath +
		       "\n\tRoot Name:\t" + rootName + "\n\tRoot Dir:\t" + rootDir + "\n\tRelative Path:\t" +
		       relativePath + "\n\tParent Path:\t" + parentPath + "\n\tPath Stem:\t" + pathStem;
	}

	/*void LogFileHelper::StorePathComponents( )
	{
		file_helper::path path = file_helper::current_path( );
		m_currentDir           = path;
		m_parentPath           = path.parent_path( );
		m_fileName             = path.filename( );
		m_pathStem             = path.stem( );
		m_relativePath         = path.relative_path( );
		m_rootDir              = path.root_directory( );
		m_rootName             = path.root_name( );
		m_rootPath             = path.root_path( );
	}*/

} // namespace serenity