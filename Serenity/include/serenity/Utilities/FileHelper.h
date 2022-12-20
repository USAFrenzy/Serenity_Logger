#pragma once

#include <UTF-Utils/utf-utils.h>
#include <serenity/Common.h>
#include <serenity/Utilities/TargetHelper.h>

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

namespace serenity::targets::helpers {

	constexpr size_t max_size_size_t { (size_t)-1 };
	constexpr size_t pageSize =
#if defined(_WIN32) && not defined(_WIN64)
	/** x86-32 has large page sizes of 2MB available, so use that instead **/
	static_cast<size_t>(2 * MB);
#elif defined _WIN64
	/** x86-64 has large page sizes of 4MB available, so use that instead **/
	static_cast<size_t>(4 * MB);
#else
	/** We'll default to just  using the default value that was being used before **/
	static_cast<size_t>(DEFAULT_BUFFER_SIZE);
#endif    // _WIN32 && !_WIN64

	class FileHelper
	{
	  public:
		explicit FileHelper(const std::string_view fpath);
		FileHelper(FileHelper&)            = delete;
		FileHelper& operator=(FileHelper&) = delete;
		~FileHelper()                      = default;

		bool OpenFile(bool truncate = false);
		void WriteToFile(size_t writeLimit = max_size_size_t, bool truncateRest = false);
		void Flush();
		bool CloseFile(bool onRotation = false);
		std::vector<char>& FileBuffer();
		size_t FileBufferSize();
		void SetBufferCapacity(size_t value);
		void InitializeFilePath(std::string_view fileName = "");
		virtual bool RenameFile(utf_utils::InputSource newFileName);
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
		void WriteImpl();
		void WriteImpl(size_t writeLimit, bool truncateRest = false);
		void FlushImpl();
		void CloseImpl();

	  private:
		int retryAttempt;
		bool fileOpen;
		int file;
		std::vector<char> buffer;
		std::mutex fileHelperMutex;
		std::unique_ptr<FileCache> fileCache;
		std::shared_ptr<BaseTargetHelper> targetHelper;
		std::unique_ptr<BackgroundThread> flushWorker;
	};

}    // namespace serenity::targets::helpers