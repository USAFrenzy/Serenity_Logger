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

namespace serenity::targets::helpers {

	FileHelper::FileHelper(const std::string_view fpath): retryAttempt(5), fileOpen(false), file(int {}), buffer(std::vector<char> {}) {
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
		OpenImpl(truncate);
		if( !fileOpen ) return false;
		ResumeBackgroundThread();
		return true;
	}

	bool FileHelper::CloseFile(bool onRotation) {
		std::unique_lock<std::mutex> lock(fileHelperMutex, std::defer_lock);
		if( targetHelper->isMTSupportEnabled() ) {
				lock.lock();
		}
		PauseBackgroundThread();
		if( !onRotation ) Flush();
		CloseImpl();
		return !(fileOpen);
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
				targetHelper->Policy()->SetPrimaryMode(FlushSetting::never);
				flushWorker->flushThreadEnabled.store(false);
		}
	}

	void FileHelper::StartBackgroundThread() {
		if( !flushWorker->flushThreadEnabled.load() ) {
				targetHelper->Policy()->SetPrimaryMode(FlushSetting::periodically);
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

	bool FileHelper::RenameFile(utf_utils::InputSource newFileName) {
		if( !CloseFile() ) {
				fprintf(stderr, "Error In RenameFile(): Unable To Close The File For Renaming\n");
				return false;
		}
		// make copy for old file conversion
		std::filesystem::path newFile { fileCache->FilePath() };
		newFile.replace_filename(std::move(newFileName.input));
		try {
				std::filesystem::rename(fileCache->FilePath(), newFile);
			}
		catch( const std::exception& e ) {
				printf("Error In RenameFile(): %s\n", e.what());
				return false;
			}
		fileCache->SetFilePath(std::move(newFile));
		return OpenFile();
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
		fileOpen = file != -1;
	}

	void FileHelper::WriteImpl(size_t writeLimit, bool truncateRest) {
		if( buffer.size() < writeLimit ) return;
		WRITE(file, buffer.data(), writeLimit);
		truncateRest ? buffer.erase(buffer.begin(), buffer.end()) : buffer.erase(buffer.begin(), buffer.begin() + writeLimit);
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
				if( CLOSE(file) != 1 ) {
						fileOpen = false;
						return;
				}
				fprintf(stderr, "%s", std::system_error(EBADF, std::system_category()).what());
		}
	}

	void FileHelper::WriteToFile(size_t writeLimit, bool truncateRest) {
		writeLimit == max_size_size_t ? FlushImpl() : WriteImpl(writeLimit, truncateRest);
	}

}    // namespace serenity::targets::helpers
