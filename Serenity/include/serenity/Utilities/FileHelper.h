#pragma once

#include <serenity/Common.h>
#include <serenity/Utilities/TargetHelper.h>

#include <fstream>

namespace serenity {

	struct BackgroundThread
	{
		std::jthread flushThread;
		std::stop_token interruptThread {};
		std::atomic<bool> cleanUpThreads { false };
		std::atomic<bool> flushThreadEnabled { false };
		std::atomic<bool> flushComplete { true };
		std::atomic<bool> threadWriting { false };
		std::atomic<bool> pauseThread { false };
	};

	struct FileSettings
	{
		FileSettings();
		FileSettings(FileSettings&)            = delete;
		FileSettings& operator=(FileSettings&) = delete;
		~FileSettings()                        = default;

		std::filesystem::path filePath;
		std::string fileDir;
		std::string fileName;
		std::string extension;
		size_t bufferSize;
		std::vector<char> fileBuffer;
	};

}    // namespace serenity

namespace serenity::targets::helpers {

	class FileHelper: public BaseTargetHelper
	{
	      public:
		explicit FileHelper(const std::string_view fpath);
		FileHelper(FileHelper&)            = delete;
		FileHelper& operator=(FileHelper&) = delete;
		~FileHelper()                      = default;

		void CacheFile(const std::filesystem::path& filePath);
		bool OpenFile(bool truncate = false);
		bool CloseFile();
		void Flush();
		std::ofstream& FileHandle();
		FileSettings& FileOptions();
		BackgroundThread& BackgoundThreadInfo();
		void BackgroundFlushThread(std::stop_token stopToken);
		void StopBackgroundThread();
		void StartBackgroundThread();
		void PauseBackgroundThread();
		void ResumeBackgroundThread();

	      private:
		FileSettings fileOptions;
		int retryAttempt;
		BackgroundThread flushWorker;
		std::ofstream fileHandle;
		std::mutex fileHelperMutex;
	};

}    // namespace serenity::targets::helpers