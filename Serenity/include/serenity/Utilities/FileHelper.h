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

	class FileCache
	{
	      public:
		FileCache(std::string_view path);
		FileCache(FileCache&)            = delete;
		FileCache& operator=(FileCache&) = delete;
		~FileCache()                     = default;
		void CacheFile(std::string_view path);
		std::vector<char>& FileBuffer();
		const size_t FileBufferSize();
		const std::filesystem::path FilePath();
		void SetBufferSize(size_t value);
		const std::string DirName();

		// Helper Functions
		void SetFilePath(const std::filesystem::path& newPath);
		void SetFileDir(const std::string& newDir);
		void SetFileName(const std::string& newName);
		void SetExtension(const std::string& newExt);

	      private:
		std::filesystem::path filePath;
		std::string fileDir;
		std::string fileName;
		std::string extension;
		size_t bufferSize;
		std::vector<char> fileBuffer;
	};

}    // namespace serenity

namespace serenity::targets::helpers {

	class FileHelper
	{
	      public:
		explicit FileHelper(const std::string_view fpath);
		FileHelper(FileHelper&)            = delete;
		FileHelper& operator=(FileHelper&) = delete;
		~FileHelper()                      = default;

		BaseTargetHelper& BaseHelper();
		bool OpenFile(bool truncate = false);
		bool CloseFile();
		void Flush();
		void SetFileBufferSize(size_t value);
		std::ofstream& FileHandle();
		void InitializeFilePath(std::string_view fileName = "");
		const std::string FilePath();
		const std::string FileName();
		FileCache* FileCacheHelper();
		bool RenameFile(std::string_view newFileName);
		BackgroundThread& BackgoundThreadInfo();
		void BackgroundFlushThread(std::stop_token stopToken);
		void StopBackgroundThread();
		void StartBackgroundThread();
		void PauseBackgroundThread();
		void ResumeBackgroundThread();

	      private:
		FileCache fileCache;
		int retryAttempt;
		BackgroundThread flushWorker;
		std::ofstream fileHandle;
		std::mutex fileHelperMutex;
		BaseTargetHelper targetHelper;
	};

}    // namespace serenity::targets::helpers