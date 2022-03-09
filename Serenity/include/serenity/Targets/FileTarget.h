#pragma once

#include <serenity/Targets/Target.h>

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
			FileTarget(const FileTarget&) = delete;
			FileTarget& operator=(const FileTarget&) = delete;
			~FileTarget();
			const std::string FilePath();
			const std::string FileName();
			virtual bool RenameFile(std::string_view newFileName);
			bool OpenFile(bool truncate = false);
			bool CloseFile();
			void Flush();
			void SetLocale(const std::locale& loc) override;
			void WriteToBaseBuffer(bool fmtToBuf = true);
			const bool isWriteToBuf();
			std::string* const Buffer();
			void StopBackgroundThread();

		private:
			LoggerLevel logLevel;
			mutable std::mutex fileMutex;

		protected:
			BackgroundThread flushWorker;
			std::ofstream fileHandle;
			FileSettings fileOptions;
			void PolicyFlushOn() override;
			void PrintMessage(std::string_view formatted) override;
			virtual void BackgroundFlush();
	};
}    // namespace serenity::targets