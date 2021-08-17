#pragma once

#include <string>
#include <filesystem>

namespace serenity {

	struct LoggerInfo
	{
		std::string loggerName;
	};


	namespace file {
		namespace file = std::filesystem;
		// Would Much Rather Be Able To Relatively Place Things If Able
		struct LogFileInfo
		{
		      private:
			std::string m_logDir  = "Logs";
			std::string m_logName = "Log.txt";

		      public:
			LogFileInfo(std::string logDir, std::string logName) : m_logDir(logDir), m_logName(logName)
			{
			}
			// void SetRelativePath( );
			void SetFileName(std::string fileName)
			{
				// This Works But I'm Sure There's A More Acceptable And Better Way To Achieve
				// This..
				m_logName = fileName;
				filePath.file::path::replace_filename(fileName);
				file.file::path::replace_filename(fileName);
			}
			file::path currentPath  = file::current_path( );
			file::path logDirectory = currentPath.append(m_logDir);
			file::path filePath     = logDirectory.append(m_logName);
			file::path file         = filePath.file::path::filename( );
		};
	} // namespace file

	struct LogSinkInfo
	{
	};

} // namespace serenity
