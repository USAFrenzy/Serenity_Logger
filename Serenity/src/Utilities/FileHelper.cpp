#include <serenity/Utilities/FileHelper.h>

#include <iostream>

namespace serenity {

	FileSettings::FileSettings(): filePath(), fileDir(), fileName(), extension(), bufferSize(DEFAULT_BUFFER_SIZE), fileBuffer(bufferSize) { }
}    // namespace serenity

namespace serenity::targets::helpers {

	FileHelper::FileHelper(): BaseTargetHelper(), fileOptions(), retryAttempt(5) { }

	FileHelper::FileHelper(const std::string_view fpath): BaseTargetHelper(), fileOptions(), retryAttempt(5) {
		CacheFile(fpath);
	}

	void FileHelper::CacheFile(const std::filesystem::path& filePath) {
		if( filePath.empty() ) return;
		auto fPath { filePath };
		auto directory { fPath };
		fileOptions.filePath  = fPath.make_preferred().string();
		fileOptions.fileName  = filePath.filename().string();
		fileOptions.extension = fPath.extension().string();
		directory._Remove_filename_and_separator();
		fileOptions.fileDir = directory.stem().string();
	}

	bool FileHelper::OpenFile(bool truncate) {
		std::unique_lock<std::mutex> lock(fileHelperMutex, std::defer_lock);
		if( isMTSupportEnabled() ) {
				lock.lock();
		}
		auto TryOpen = [ &, this ]() {
#ifndef WINDOWS_PLATFORM
			fileHandle.rdbuf()->pubsetbuf(fileBuffer.data(), bufferSize);
#endif    // !WINDOWS_PLATFORM
			if( !truncate ) {
					fileHandle.open(fileOptions.filePath.string(), std::ios_base::binary | std::ios_base::app);
			} else {
					fileHandle.open(fileOptions.filePath.string(), std::ios_base::binary | std::ios_base::trunc);
				}
#ifdef WINDOWS_PLATFORM
			fileHandle.rdbuf()->pubsetbuf(fileOptions.fileBuffer.data(), fileOptions.bufferSize);
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

	void FileHelper::Flush() {
		std::unique_lock<std::mutex> lock(fileHelperMutex, std::defer_lock);
		auto flushThreadEnabled { flushWorker.flushThreadEnabled.load() };
		if( flushThreadEnabled ) {
				if( flushWorker.threadWriting.load() ) {
						flushWorker.threadWriting.wait(true);
				}
				flushWorker.flushComplete.store(false);
		}
		if( isMTSupportEnabled() ) {
				lock.lock();
		}
		// If formatted message wasn't written to file and instead was written to the buffer, write to file now
		if( Buffer()->size() != 0 ) {
				fileHandle.rdbuf()->sputn(Buffer()->data(), Buffer()->size());
				Buffer()->clear();
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

	FileSettings& FileHelper::FileOptions() {
		return fileOptions;
	}

	BackgroundThread& FileHelper::BackgoundThreadInfo() {
		return flushWorker;
	}

	void FileHelper::BackgroundFlushThread(std::stop_token stopToken) {
		auto flushInterval { policy.SecondarySettings().flushEvery };
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
				policy.SetPrimaryMode(serenity::experimental::FlushSetting::never);
				flushWorker.flushThreadEnabled.store(false);
		}
	}

	void FileHelper::StartBackgroundThread() {
		if( !flushWorker.flushThreadEnabled.load() ) {
				policy.SetPrimaryMode(serenity::experimental::FlushSetting::periodically);
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

}    // namespace serenity::targets::helpers
