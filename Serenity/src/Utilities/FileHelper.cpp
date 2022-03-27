#include <serenity/Utilities/FileHelper.h>

#include <iostream>

namespace serenity {

	FileCache::FileCache(std::string_view path)
		: filePath(std::filesystem::path {}), fileDir(std::string {}), fileName(std::string {}), extension(std::string {}),
		  bufferSize(DEFAULT_BUFFER_SIZE), fileBuffer(bufferSize) {
		CacheFile(std::move(path));
	}

	void FileCache::CacheFile(std::string_view path) {
		if( path.empty() ) return;
		std::filesystem::path fPath = path;
		auto directory { fPath };
		filePath = fPath.make_preferred().string();
		directory._Remove_filename_and_separator();
		fileDir   = directory.stem().string();
		fileName  = filePath.filename().string();
		extension = fPath.extension().string();
	}

	std::vector<char>& FileCache::FileBuffer() {
		return fileBuffer;
	}

	const size_t FileCache::FileBufferSize() {
		return bufferSize;
	}

	const std::filesystem::path FileCache::FilePath() {
		return filePath;
	}

	void FileCache::SetBufferSize(size_t value) {
		bufferSize = value;
	}

	const std::string FileCache::DirName() {
		return fileDir;
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

	FileHelper::FileHelper(const std::string_view fpath): fileCache(fpath), retryAttempt(5) { }

	void FileHelper::InitializeFilePath(std::string_view fileName) {
		std::filesystem::path fullFilePath = std::filesystem::current_path();
		const auto logDir { "Logs" };
		fullFilePath /= logDir;
		fileName.empty() ? fullFilePath /= "Generic_Log.txt" : fullFilePath /= fileName;
		fileCache.CacheFile(fullFilePath.string());
	}

	void FileHelper::SetFileBufferSize(size_t value) {
		if( fileHandle.is_open() ) {
				CloseFile();
		}
		fileCache.SetBufferSize(value);
		OpenFile();
	}

	bool FileHelper::OpenFile(bool truncate) {
		std::unique_lock<std::mutex> lock(fileHelperMutex, std::defer_lock);
		if( targetHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		auto TryOpen = [ &, this ]() {
#ifndef WINDOWS_PLATFORM
			fileHandle.rdbuf()->pubsetbuf(fileBuffer.data(), bufferSize);
#endif    // !WINDOWS_PLATFORM
			if( !truncate ) {
					fileHandle.open(FilePath(), std::ios_base::binary | std::ios_base::app);
			} else {
					fileHandle.open(FilePath(), std::ios_base::binary | std::ios_base::trunc);
				}
#ifdef WINDOWS_PLATFORM
			fileHandle.rdbuf()->pubsetbuf(fileCache.FileBuffer().data(), fileCache.FileBufferSize());
#endif    // WINDOWS_PLATFORM
			auto fileOpen { fileHandle.is_open() };
			if( fileOpen ) {
					ResumeBackgroundThread();
					return true;
			}
			return false;
		};

		for( int tries = 0; tries < retryAttempt; ++tries ) {
				fileHandle.clear();
				if( tries == retryAttempt ) {
						std::cerr << "Max Attempts At Opening File Reached - Unable To Open File\n";
						return false;
				}
				if( TryOpen() ) break;
				std::this_thread::sleep_for(std::chrono::nanoseconds(100));
			}
		return true;
	}

	bool FileHelper::CloseFile() {
		PauseBackgroundThread();
		Flush();

		auto TryClose = [ this ]() {
			fileHandle.close();
			return !fileHandle.is_open();
		};
		for( int tries = 0; tries < retryAttempt; ++tries ) {
				fileHandle.clear();
				if( tries == retryAttempt ) {
						std::cerr << "Max Attempts At Closing File Reached - Unable To Close File\n";
						return false;
				}
				if( TryClose() ) break;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		return true;
	}

	BaseTargetHelper& targets::helpers::FileHelper::BaseHelper() {
		return targetHelper;
	}

	void FileHelper::Flush() {
		std::unique_lock<std::mutex> lock(fileHelperMutex, std::defer_lock);
		auto flushThreadEnabled { flushWorker.flushThreadEnabled.load() };
		if( flushThreadEnabled ) {
				if( flushWorker.threadWriting.load() ) {
						flushWorker.threadWriting.wait(true);
				}
				flushWorker.flushComplete.store(false);
		}
		if( targetHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		// If formatted message wasn't written to file and instead was written to the buffer, write to file now
		if( targetHelper.Buffer()->size() != 0 ) {
				auto& buffer { *targetHelper.Buffer() };
				fileHandle.rdbuf()->sputn(buffer.data(), buffer.size());
				buffer.clear();
		}
		fileHandle.flush();
		if( lock.owns_lock() ) {
				lock.unlock();
		}
		if( flushThreadEnabled ) {
				flushWorker.flushComplete.store(true);
				flushWorker.flushComplete.notify_all();
		}
	}

	std::ofstream& FileHelper::FileHandle() {
		return fileHandle;
	}

	FileCache* FileHelper::FileCacheHelper() {
		return &fileCache;
	}

	BackgroundThread& FileHelper::BackgoundThreadInfo() {
		return flushWorker;
	}

	void FileHelper::BackgroundFlushThread(std::stop_token stopToken) {
		auto flushInterval { targetHelper.Policy().SecondarySettings().flushEvery };
		while( !flushWorker.cleanUpThreads.load() ) {
				if( stopToken.stop_requested() ) {
						break;
				}
				if( flushWorker.pauseThread.load() ) {
						flushWorker.pauseThread.wait(true);
				}
				Flush();
				std::this_thread::sleep_for(flushInterval);
			}
	}

	void FileHelper::StopBackgroundThread() {
		if( flushWorker.flushThreadEnabled.load() ) {
				flushWorker.flushThread.request_stop();
				flushWorker.cleanUpThreads.store(true);
				flushWorker.cleanUpThreads.notify_one();
				targetHelper.Policy().SetPrimaryMode(serenity::experimental::FlushSetting::never);
				flushWorker.flushThreadEnabled.store(false);
		}
	}

	void FileHelper::StartBackgroundThread() {
		if( !flushWorker.flushThreadEnabled.load() ) {
				targetHelper.Policy().SetPrimaryMode(serenity::experimental::FlushSetting::periodically);
				flushWorker.cleanUpThreads.store(false);
				flushWorker.flushThread = std::jthread(&FileHelper::BackgroundFlushThread, this, flushWorker.interruptThread);
				flushWorker.flushThreadEnabled.store(true);
		}
	}

	void FileHelper::PauseBackgroundThread() {
		if( flushWorker.flushThreadEnabled.load() ) {
				flushWorker.pauseThread.store(true);
		}
	}

	void FileHelper::ResumeBackgroundThread() {
		auto threadEnabled { flushWorker.flushThreadEnabled.load() };
		auto threadPaused { flushWorker.pauseThread.load() };
		if( threadEnabled && threadPaused ) {
				flushWorker.pauseThread.store(false);
				flushWorker.pauseThread.notify_one();
		}
	}

	const std::string FileHelper::FilePath() {
		return fileCache.FilePath().string();
	}

	const std::string FileHelper::FileName() {
		return fileCache.FilePath().filename().string();
	}

	bool FileHelper::RenameFile(std::string_view newFileName) {
		try {
				CloseFile();
				// make copy for old file conversion
				std::filesystem::path newFile { fileCache.FilePath() };
				newFile.replace_filename(newFileName);
				std::filesystem::rename(fileCache.FilePath(), newFile);
				fileCache.SetFilePath(std::move(newFile));
				return OpenFile();
			}
		catch( const std::exception& e ) {
				std::cerr << "Error In Renaming File:\n";
				std::cerr << e.what();
				return false;
			}
	}

}    // namespace serenity::targets::helpers
