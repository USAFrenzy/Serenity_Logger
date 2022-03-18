#include <serenity/Targets/FileTarget.h>

#include <iostream>    // specifically for std::cerr

namespace serenity::targets {

	FileTarget::FileTarget(): TargetBase("File_Logger") {
		fileOptions.fileBuffer.reserve(fileOptions.bufferSize);

		std::filesystem::path fullFilePath = std::filesystem::current_path();
		const auto logDir { "Logs" };
		const auto logDirPath { fullFilePath };
		fullFilePath /= logDir;
		fullFilePath /= "Generic_Log.txt";
		fileOptions.filePath = fullFilePath.make_preferred().string();
		logLevel             = LoggerLevel::trace;

		try {
				if( !std::filesystem::exists(logDirPath) ) {
						std::filesystem::create_directories(logDirPath);
						OpenFile(true);
				} else {
						OpenFile(true);
					}
			}
		catch( const std::exception& e ) {
				std::cerr << e.what() << "\n";
				CloseFile();
			}
		if( fileHandle.getloc() != MsgInfo()->GetLocale() ) {
				fileHandle.imbue(MsgInfo()->GetLocale());
		}
	}

	FileTarget::FileTarget(std::string_view fileName, bool replaceIfExists): TargetBase("File_Logger") {
		fileOptions.fileBuffer.reserve(fileOptions.bufferSize);
		std::filesystem::path fullFilePath = std::filesystem::current_path();
		const auto logDir { "Logs" };
		const auto logDirPath { fullFilePath };
		fullFilePath /= logDir;
		fullFilePath /= fileName;
		fileOptions.filePath = fullFilePath.make_preferred().string();
		logLevel             = LoggerLevel::trace;
		try {
				if( !std::filesystem::exists(logDirPath) ) {
						std::filesystem::create_directories(logDirPath);
						OpenFile(true);
				} else {
						OpenFile(true);
					}
			}
		catch( const std::exception& e ) {
				std::cerr << e.what() << "\n";
				CloseFile();
			}
		if( fileHandle.getloc() != MsgInfo()->GetLocale() ) {
				fileHandle.imbue(MsgInfo()->GetLocale());
		}
	}

	FileTarget::FileTarget(std::string_view name, std::string_view fPath, bool replaceIfExists): TargetBase(name) {
		fileOptions.fileBuffer.reserve(fileOptions.bufferSize);
		fileOptions.filePath = fPath;
		fileOptions.filePath.make_preferred();
		logLevel = LoggerLevel::trace;
		try {
				if( !std::filesystem::exists(fileOptions.filePath) ) {
						auto dir { fileOptions.filePath };
						dir.remove_filename();
						std::filesystem::create_directories(dir);
						OpenFile(replaceIfExists);
				} else {
						OpenFile(replaceIfExists);
					}
			}
		catch( const std::exception& e ) {
				std::cerr << e.what() << "\n";
				CloseFile();
			}
		if( fileHandle.getloc() != MsgInfo()->GetLocale() ) {
				fileHandle.imbue(MsgInfo()->GetLocale());
		}
	}

	FileTarget::FileTarget(std::string_view name, std::string_view formatPattern, std::string_view fPath, bool replaceIfExists)
		: TargetBase(name, formatPattern) {
		fileOptions.fileBuffer.reserve(fileOptions.bufferSize);
		fileOptions.filePath = fPath;
		fileOptions.filePath.make_preferred();
		logLevel = LoggerLevel::trace;
		try {
				if( !std::filesystem::exists(fileOptions.filePath) ) {
						auto dir { fileOptions.filePath };
						dir.remove_filename();
						std::filesystem::create_directories(dir);
						OpenFile(replaceIfExists);
				} else {
						OpenFile(replaceIfExists);
					}
			}
		catch( const std::exception& e ) {
				std::cerr << e.what() << "\n";
				CloseFile();
			}
		if( fileHandle.getloc() != MsgInfo()->GetLocale() ) {
				fileHandle.imbue(MsgInfo()->GetLocale());
		}
	}

	FileTarget::~FileTarget() {
		StopBackgroundThread();
		CloseFile();
	}

	const std::string FileTarget::FilePath() {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( isMTSupportEnabled() ) {
				lock.lock();
		}
		return fileOptions.filePath.string();
	}

	const std::string FileTarget::FileName() {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( isMTSupportEnabled() ) {
				lock.lock();
		}
		return fileOptions.filePath.filename().string();
	}

	bool FileTarget::OpenFile(bool truncate) {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
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
			return fileHandle.is_open();
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

	void serenity::targets::FileTarget::StopBackgroundThread() {
		if( flushWorker.flushThreadEnabled.load() ) {
				flushWorker.cleanUpThreads.store(true);
				flushWorker.cleanUpThreads.notify_one();
				flushWorker.flushThread.request_stop();
				flushWorker.flushThread.join();
				policy.SetPrimaryMode(serenity::experimental::FlushSetting::never);
				flushWorker.flushThreadEnabled.store(false);
		}
	}

	void FileTarget::StartBackgroundThread() {
		if( !flushWorker.flushThreadEnabled.load() ) {
				policy.SetPrimaryMode(serenity::experimental::FlushSetting::periodically);
				flushWorker.cleanUpThreads.store(false);
				flushWorker.flushThread = std::jthread(&FileTarget::BackgroundFlushThread, this, flushWorker.interruptThread);
				flushWorker.flushThreadEnabled.store(true);
		}
	}

	// Testing for the ability to just pause the thread and resume it instead of always destroying/re-creating thread
	// TODO ******************************** IMPLEMENT THESE ********************************
	void FileTarget::PauseBackgroundThread() {
		if( flushWorker.flushThreadEnabled.load() ) {
				flushWorker.pauseThread.store(true);
		}
	}

	void FileTarget::ResumeBackgroundThread() {
		auto threadEnabled { flushWorker.flushThreadEnabled.load() };
		auto threadPaused { flushWorker.pauseThread.load() };
		if( threadEnabled && threadPaused ) {
				flushWorker.pauseThread.store(false);
				flushWorker.pauseThread.notify_one();
		}
	}
	// TODO ******************************** IMPLEMENT THESE ********************************

	/*
	        Taken from modernescpp.com
	        ##############################################
	        cv.wait_until(lock, predicate, itoken);
	                if (itoken.is_interrupted()){
	                        // interrupt occurred
	                }
	        ##############################################
	        The above might be a way to wait on elapsed time OR
	        on a stop token and would be much more stream-lined
	*/

	void FileTarget::BackgroundFlushThread(std::stop_token stopToken) {
		while( !flushWorker.cleanUpThreads.load() ) {
				if( stopToken.stop_requested() ) {
						break;
				}
				if( flushWorker.pauseThread.load() ) {
						flushWorker.pauseThread.wait(true);
				}
				Flush();
				std::this_thread::sleep_for(policy.SecondarySettings().flushEvery);
			}
	}

	bool FileTarget::CloseFile() {
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

	bool FileTarget::RenameFile(std::string_view newFileName) {
		try {
				CloseFile();
				// make copy for old file conversion
				std::filesystem::path newFile { fileOptions.filePath };
				newFile.replace_filename(newFileName);
				std::filesystem::rename(fileOptions.filePath, newFile);
				fileOptions.filePath = std::move(newFile);
				return OpenFile();
			}
		catch( const std::exception& e ) {
				std::cerr << "Error In Renaming File:\n";
				std::cerr << e.what();
				return false;
			}
	}

	void FileTarget::PrintMessage(std::string_view formatted) {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		auto flushThreadEnabled { flushWorker.flushThreadEnabled.load() };
		if( flushThreadEnabled ) {
				if( !flushWorker.flushComplete.load() ) {
						flushWorker.flushComplete.wait(false);
				}
				flushWorker.threadWriting.store(true);
		}
		if( isMTSupportEnabled() ) {
				lock.lock();
		}
		fileHandle.rdbuf()->sputn(formatted.data(), formatted.size());
		if( lock.owns_lock() ) {
				lock.unlock();
		}
		if( flushThreadEnabled ) {
				flushWorker.threadWriting.store(false);
				flushWorker.threadWriting.notify_all();
		}
	}

	void FileTarget::Flush() {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
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

	void FileTarget::SetLocale(const std::locale& loc) {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( isMTSupportEnabled() ) {
				lock.lock();
		}
		if( fileHandle.getloc() != loc ) {
				fileHandle.imbue(loc);
		}
		TargetBase::SetLocale(loc);
	}

	void FileTarget::WriteToBaseBuffer(bool fmtToBuf) {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( isMTSupportEnabled() ) {
				lock.lock();
		}
		TargetBase::WriteToBaseBuffer(fmtToBuf);
	}

	const bool FileTarget::isWriteToBuf() {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( isMTSupportEnabled() ) {
				lock.lock();
		}
		return TargetBase::isWriteToBuf();
	}

	std::string* const FileTarget::Buffer() {
		return TargetBase::Buffer();
	}

	void FileTarget::PolicyFlushOn() {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( isMTSupportEnabled() ) {
				lock.lock();
		}
		if( policy.PrimarySetting() == serenity::experimental::FlushSetting::never ) return;
		if( policy.PrimarySetting() == serenity::experimental::FlushSetting::always ) {
				Flush();
		}
		switch( policy.SubSetting() ) {
				case serenity::experimental::PeriodicOptions::timeBased:
					{
						StartBackgroundThread();
					}
					break;    // time based bounds
				case serenity::experimental::PeriodicOptions::logLevelBased:
					{
						if( MsgInfo()->MsgLevel() < policy.SecondarySettings().flushOn ) return;
						if( lock.owns_lock() ) {
								lock.unlock();
						}
						Flush();
					}
					break;
			}    // Sub Option Check
	}                    // PolicyFlushOn( ) Function

}    // namespace serenity::targets
