#pragma once

#include <serenity/Targets/Target.h>
#include <serenity/Utilities/FileHelper.h>

#include <fstream>

namespace serenity::targets {
	class FileTarget: public TargetBase, public helpers::FileHelper
	{
	  public:
		FileTarget();
		explicit FileTarget(utf_utils::InputSource fileName, bool replaceIfExists = false);
		explicit FileTarget(utf_utils::InputSource name, utf_utils::InputSource filePath, bool replaceIfExists = false);
		explicit FileTarget(utf_utils::InputSource name, utf_utils::InputSource formatPattern, utf_utils::InputSource filePath, bool replaceIfExists = false);
		FileTarget(const FileTarget&)            = delete;
		FileTarget& operator=(const FileTarget&) = delete;
		~FileTarget();

		void InitializeLogs(const std::string_view filePath, bool replaceIfExists);
		void SetLocale(const std::locale& loc) override;

	  protected:
		void PolicyFlushOn() override;
		void PrintMessage() override;

	  private:
		mutable std::mutex fileMutex;
	};
}    // namespace serenity::targets