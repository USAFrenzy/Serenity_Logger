#pragma once

#include <serenity/Common.h>
#include <serenity/Utilities/TargetHelper.h>

#ifndef USE_INDEPENDANT_UTF_UTILS
	#include <ArgFormatter/dependencies/UTF-Utils/utf-utils.h>
#else
	#include <UTF-Utils/utf-utils.h>
#endif

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

	// TODO: Add utf-8 encoding support to file paths and file naming
	class FileCache
	{
	  public:
		FileCache(std::string_view path);
		FileCache(FileCache&)            = delete;
		FileCache& operator=(FileCache&) = delete;
		~FileCache()                     = default;
		void CacheFile(std::string_view path, bool ignoreExtInFileName = false);
		std::filesystem::path FilePath() const;
		std::filesystem::path DirPath() const;
		std::string DirName() const;
		std::string FileName() const;
		std::string Extenstion() const;

		// Helper Functions
		void SetFilePath(const std::filesystem::path& newPath);
		void SetFileDir(const std::string& newDir);
		void SetFileName(const std::string& newName);
		void SetExtension(const std::string& newExt);

	  protected:
		std::filesystem::path filePath;
		std::string fileDir;
		std::string fileName;
		std::string extension;
		std::filesystem::path dirPath;
	};

}    // namespace serenity

// TODO: Add utf-8 encoding support to file paths and file naming
namespace serenity::targets::helpers {

	constexpr size_t max_size_size_t { (size_t)-1 };

	class FileHelper
	{
	  public:
		explicit FileHelper(const std::string_view fpath);
		FileHelper(FileHelper&)            = delete;
		FileHelper& operator=(FileHelper&) = delete;
		~FileHelper()                      = default;

		bool OpenFile(bool truncate = false);
		void WriteToFile(std::string_view msg, size_t writeLimit = max_size_size_t, bool truncateRest = false);
		void Flush();
		bool CloseFile(bool onRotation = false);
		std::vector<char>& FileBuffer();
		size_t FileBufferSize();
		void SetBufferCapacity(size_t value);
		void InitializeFilePath(std::string_view fileName = "");
		virtual bool RenameFile(std::string_view newFileName);
		void BackgroundFlushThread(std::stop_token stopToken);
		void StopBackgroundThread();
		void StartBackgroundThread();
		void PauseBackgroundThread();
		void ResumeBackgroundThread();
		void SyncTargetHelpers(std::shared_ptr<BaseTargetHelper>& syncToHelper);
		const std::unique_ptr<FileCache>& FileCacheHelper() const;
		const std::unique_ptr<BackgroundThread>& BackgoundThreadInfo() const;

	  private:
		void OpenImpl(bool truncate);
		void WriteImpl(std::string_view msg);
		void WriteImpl(std::string_view msg, size_t writeLimit, bool truncateRest = false);
		void FlushImpl();
		void CloseImpl();

	  private:
		int retryAttempt;
		bool fileOpen;
		int file;
		size_t pageSize;
		std::vector<char> buffer;
		std::mutex fileHelperMutex;
		std::unique_ptr<FileCache> fileCache;
		std::shared_ptr<BaseTargetHelper> targetHelper;
		std::unique_ptr<BackgroundThread> flushWorker;
	};

}    // namespace serenity::targets::helpers