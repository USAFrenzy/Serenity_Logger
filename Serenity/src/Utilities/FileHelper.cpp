#include <serenity/Utilities/FileHelper.h>
#include <fcntl.h>

#ifdef WINDOWS_PLATFORM
	#define OPEN(file, pathname, flags) _sopen_s(&file, pathname, flags, SH_DENYNO, _S_IREAD | _S_IWRITE)
	#define WRITE(file, src, size)      _write(file, src, static_cast<unsigned int>(size))
	#define CLOSE(file)                 _close(file)
#else
	#define OPEN(file, pathname, flags) open(file, pathname, flags)
	#define Write(file, src, size)      write(file, src, size)
	#define CLOSE(file)                 close(file)
#endif    // WINDOWS_PLATFORM

// TODO: Add utf-8 encoding support to file paths and file naming

namespace serenity {

	FileCache::FileCache(std::string_view path)
		: filePath(std::filesystem::path {}), fileDir(std::string {}), fileName(std::string {}), extension(std::string {}), dirPath(filePath) {
		dirPath._Remove_filename_and_separator();
		CacheFile(std::move(path));
	}

	void FileCache::CacheFile(std::string_view path, bool ignoreExtInFileName) {
		if( path.empty() ) return;
		if( !utf_utils::IsValidU8(path) ) {
				throw std::runtime_error("Error In File Path Or File Name: Invalid UTF-8 Sequence Detected - A Proper File Path Cannot Be Constructed");
		}
		std::filesystem::path fPath { path };
		if( !fPath.has_relative_path() ) {
				auto directory { std::filesystem::current_path() /= "Logs" };
				filePath = directory.string().append(fPath.make_preferred().string());
				directory._Remove_filename_and_separator();
				fileDir   = directory.stem().string();
				dirPath   = directory;
				extension = fPath.extension().string();
				if( ignoreExtInFileName ) {
						auto temp { filePath };
						temp.replace_extension();
						fileName = temp.filename().string();
				} else {
						fileName = filePath.filename().string();
					}
		} else {
				auto directory { fPath };
				filePath = fPath.make_preferred().string();
				directory._Remove_filename_and_separator();
				fileDir   = directory.stem().string();
				dirPath   = directory;
				extension = fPath.extension().string();
				if( ignoreExtInFileName ) {
						auto temp { filePath };
						temp.replace_extension();
						fileName = temp.filename().string();
				} else {
						fileName = filePath.filename().string();
					}
			}
	}

	std::filesystem::path FileCache::FilePath() const {
		return filePath;
	}

	std::string FileCache::DirName() const {
		return fileDir;
	}

	std::string FileCache::FileName() const {
		return fileName;
	}

	std::string FileCache::Extenstion() const {
		return extension;
	}

	std::filesystem::path FileCache::DirPath() const {
		return dirPath;
	}

	// Helper Functions
	void FileCache::SetFilePath(const std::filesystem::path& newPath) {
		filePath = newPath;
	}
	void FileCache::SetFileDir(const std::string& newDir) {
		fileDir = newDir;
	}
	void FileCache::SetFileName(const std::string& newName) {
		fileName = newName;
	}
	void FileCache::SetExtension(const std::string& newExt) {
		extension = newExt;
	}

}    // namespace serenity

static constexpr size_t PageSize() {
#if defined(_WIN32) && not defined(_WIN64)
	/** x86-32 has large page sizes of 2MB available, so use that instead **/
	return static_cast<size_t>(2 * MB);
#elif defined _WIN64
	/** x86-64 has large page sizes of 4MB available, so use that instead **/
	return static_cast<size_t>(4 * MB);
#else
	/** We'll default to just  using the default value that was being used before **/
	return static_cast<size_t>(DEFAULT_BUFFER_SIZE);
#endif    // _WIN32 && !_WIN64
}

namespace serenity::targets::helpers {

	FileHelper::FileHelper(const std::string_view fpath): retryAttempt(5), fileOpen(false), file(int {}), pageSize(PageSize()), buffer(std::vector<char> {}) {
		fileCache    = std::make_unique<FileCache>(fpath);
		targetHelper = std::make_unique<BaseTargetHelper>();
		flushWorker  = std::make_unique<BackgroundThread>();
		buffer.reserve(pageSize);
	}

	void FileHelper::InitializeFilePath(std::string_view fileName) {
		std::filesystem::path fullFilePath = std::filesystem::current_path();
		const auto logDir { "Logs" };
		fullFilePath /= logDir;
		fileName.empty() ? fullFilePath /= "Generic_Log.txt" : fullFilePath /= fileName;
		fileCache->CacheFile(fullFilePath.string());
	}

	void FileHelper::SetBufferCapacity(size_t value) {
		buffer.reserve(value);
	}
	std::vector<char>& FileHelper::FileBuffer() {
		return buffer;
	}

	size_t FileHelper::FileBufferSize() {
		return buffer.size();
	}

	bool FileHelper::OpenFile(bool truncate) {
		std::unique_lock<std::mutex> lock(fileHelperMutex, std::defer_lock);
		if( targetHelper->isMTSupportEnabled() ) {
				lock.lock();
		}
		auto TryOpen = [ &, this ]() {
			if( !truncate ) {
					OpenImpl(truncate);
			} else {
					OpenImpl(truncate);
				}
			if( file != -1 ) {
					ResumeBackgroundThread();
					return true;
			}
			return false;
		};

		for( int tries = 0; tries < retryAttempt; ++tries ) {
				if( tries == retryAttempt ) {
						printf("Max Attempts At Opening File Reached - Unable To Open File\n");
						return false;
				}
				if( TryOpen() ) break;
				std::this_thread::sleep_for(std::chrono::nanoseconds(100));
			}
		return true;
	}

	bool FileHelper::CloseFile(bool onRotation) {
		PauseBackgroundThread();
		if( !onRotation ) Flush();

		auto TryClose = [ this ]() {
			CloseImpl();
			return true;
		};
		for( int tries = 0; tries < retryAttempt; ++tries ) {
				if( tries == retryAttempt ) {
						printf("Max Attempts At Closing File Reached - Unable To Close File\n");
						return false;
				}
				if( TryClose() ) break;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		return true;
	}

	void FileHelper::Flush() {
		std::unique_lock<std::mutex> lock(fileHelperMutex, std::defer_lock);
		const auto& flushThreadEnabled { flushWorker->flushThreadEnabled.load() };
		if( flushThreadEnabled ) {
				if( flushWorker->threadWriting.load() ) {
						flushWorker->threadWriting.wait(true);
				}
				flushWorker->flushComplete.store(false);
		}
		if( targetHelper->isMTSupportEnabled() ) {
				lock.lock();
		}
		FlushImpl();
		if( lock.owns_lock() ) {
				lock.unlock();
		}
		if( flushThreadEnabled ) {
				flushWorker->flushComplete.store(true);
				flushWorker->flushComplete.notify_all();
		}
	}

	const std::unique_ptr<FileCache>& FileHelper::FileCacheHelper() const {
		return fileCache;
	}

	void FileHelper::SyncTargetHelpers(std::shared_ptr<BaseTargetHelper>& syncToHelper) {
		targetHelper = syncToHelper;
	}

	const std::unique_ptr<BackgroundThread>& FileHelper::BackgoundThreadInfo() const {
		return flushWorker;
	}

	void FileHelper::BackgroundFlushThread(std::stop_token stopToken) {
		auto flushInterval { targetHelper->Policy()->SecondarySettings().flushEvery };
		while( !flushWorker->cleanUpThreads.load() ) {
				if( stopToken.stop_requested() ) {
						break;
				}
				if( flushWorker->pauseThread.load() ) {
						flushWorker->pauseThread.wait(true);
				}
				Flush();
				std::this_thread::sleep_for(flushInterval);
			}
	}

	void FileHelper::StopBackgroundThread() {
		if( flushWorker->flushThreadEnabled.load() ) {
				flushWorker->flushThread.request_stop();
				flushWorker->cleanUpThreads.store(true);
				flushWorker->cleanUpThreads.notify_one();
				targetHelper->Policy()->SetPrimaryMode(serenity::experimental::FlushSetting::never);
				flushWorker->flushThreadEnabled.store(false);
		}
	}

	void FileHelper::StartBackgroundThread() {
		if( !flushWorker->flushThreadEnabled.load() ) {
				targetHelper->Policy()->SetPrimaryMode(serenity::experimental::FlushSetting::periodically);
				flushWorker->cleanUpThreads.store(false);
				flushWorker->flushThread = std::jthread(&FileHelper::BackgroundFlushThread, this, flushWorker->interruptThread);
				flushWorker->flushThreadEnabled.store(true);
		}
	}

	void FileHelper::PauseBackgroundThread() {
		if( flushWorker->flushThreadEnabled.load() ) {
				flushWorker->pauseThread.store(true);
		}
	}

	void FileHelper::ResumeBackgroundThread() {
		auto threadEnabled { flushWorker->flushThreadEnabled.load() };
		auto threadPaused { flushWorker->pauseThread.load() };
		if( threadEnabled && threadPaused ) {
				flushWorker->pauseThread.store(false);
				flushWorker->pauseThread.notify_one();
		}
	}

	bool FileHelper::RenameFile(std::string_view newFileName) {
		try {
				CloseFile();
				// make copy for old file conversion
				std::filesystem::path newFile { fileCache->FilePath() };
				newFile.replace_filename(newFileName);
				std::filesystem::rename(fileCache->FilePath(), newFile);
				fileCache->SetFilePath(std::move(newFile));
				return OpenFile();
			}
		catch( const std::exception& e ) {
				printf("Error In Renaming File: %s\n", e.what());
				return false;
			}
	}

	void FileHelper::OpenImpl(bool truncate) {
		std::filesystem::path filePath { fileCache->FilePath() };
		if( auto dirPath { filePath }; !std::filesystem::exists(dirPath.make_preferred().remove_filename()) ) {
				std::filesystem::create_directories(dirPath);
				OPEN(file, fileCache->FilePath().string().c_str(), O_CREAT | _O_WRONLY | _O_BINARY);
		} else {
				if( !std::filesystem::exists(filePath) ) {
						OPEN(file, fileCache->FilePath().string().c_str(), O_CREAT | _O_WRONLY | _O_BINARY);
				} else {
						truncate ? OPEN(file, fileCache->FilePath().string().c_str(), O_TRUNC | _O_WRONLY | _O_BINARY)
								 : OPEN(file, fileCache->FilePath().string().c_str(), O_APPEND | _O_WRONLY | _O_BINARY);
					}
			}
		if( file != -1 ) {
				fileOpen = true;
		} else {
				switch( errno ) {
						case EACCES: throw std::system_error(EACCES, std::system_category());
						case EEXIST: throw std::system_error(EEXIST, std::system_category());
						case EINVAL: throw std::system_error(EINVAL, std::system_category());
						case EMFILE: throw std::system_error(EMFILE, std::system_category());
						case ENOENT: throw std::system_error(ENOENT, std::system_category());
					}
			}
	}

	// This seems to be ~35ns- to ~45ns faster than the fileHandle.rdbuf()->sputn(formatted.data(), formatted.size())
	//  method and brings the throughput from ~1450 MB/s up to ~1715 MB/s
	void FileHelper::WriteImpl() {
		if( buffer.size() >= pageSize ) {
				WRITE(file, buffer.data(), pageSize);
				buffer.erase(buffer.begin(), buffer.begin() + pageSize);
		}
	}

	void FileHelper::WriteImpl(size_t writeLimit, bool truncateRest) {
		if( buffer.size() >= writeLimit ) {
				WRITE(file, buffer.data(), writeLimit);
				truncateRest ? buffer.erase(buffer.begin(), buffer.end()) : buffer.erase(buffer.begin(), buffer.begin() + writeLimit);
		}
	}

	void FileHelper::FlushImpl() {
		if( buffer.size() >= pageSize ) {
				auto pages { buffer.size() / pageSize };
				for( pages; pages != 0; --pages ) {
						WRITE(file, buffer.data(), pageSize);
						buffer.erase(buffer.begin(), buffer.begin() + pageSize);
					}
		}
		if( buffer.size() != 0 ) {
				WRITE(file, buffer.data(), buffer.size());
		}
		buffer.clear();
	}

	void FileHelper::CloseImpl() {
		if( fileOpen ) {
				CLOSE(file);
				fileOpen = false;
		}
		buffer.clear();
	}

	void FileHelper::WriteToFile(size_t writeLimit, bool truncateRest) {
		writeLimit == max_size_size_t ? WriteImpl() : WriteImpl(writeLimit, truncateRest);
	}

}    // namespace serenity::targets::helpers
