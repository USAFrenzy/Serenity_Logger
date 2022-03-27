#pragma once

#include <serenity/Targets/Target.h>
#include <serenity/Utilities/FileHelper.h>

#include <fstream>

namespace serenity::targets {
	class FileTarget: public TargetBase
	{
	      public:
		FileTarget();
		explicit FileTarget(std::string_view fileName, bool replaceIfExists = false);
		explicit FileTarget(std::string_view name, std::string_view filePath, bool replaceIfExists = false);
		explicit FileTarget(std::string_view name, std::string_view formatPattern, std::string_view filePath,
		                    bool replaceIfExists = false);
		FileTarget(const FileTarget&)            = delete;
		FileTarget& operator=(const FileTarget&) = delete;
		~FileTarget();
		bool RenameFile(std::string_view newFileName);
		void SetLocale(const std::locale& loc) override;
		bool OpenFIle(bool truncate = false);
		bool CloseFIle();
		void Flush();

	      protected:
		void PolicyFlushOn() override;
		void PrintMessage(std::string_view formatted) override;

	      private:
		helpers::FileHelper fileHelper;
		mutable std::mutex fileMutex;
	};
}    // namespace serenity::targets