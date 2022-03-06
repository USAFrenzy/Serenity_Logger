#include <serenity/Targets/FileTarget.h>

#include <iostream>    // specifically for std::cerr

namespace serenity::targets {

	static constexpr int retryAttempt { 5 };

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
		CloseFile();
	}

	const std::string FileTarget::FilePath() {
		return fileOptions.filePath.string();
	}

	const std::string FileTarget::FileName() {
		return fileOptions.filePath.filename().string();
	}

	bool FileTarget::OpenFile(bool truncate) {
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
		if( flushWorker.flushThreadEnabled.load(std::memory_order::relaxed) ) {
				flushWorker.cleanUpThreads.store(true);
				flushWorker.cleanUpThreads.notify_one();
				while( !flushWorker.flushThread.joinable() ) {
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
					}
				flushWorker.flushThread.join();
				// change the primary mode so as not to launch another thread upon a message log.
				// Requires SetPrimaryMode(FlushSetting::Periodically) to be called again to
				// re-enable when a new message is logged
				policy.SetPrimaryMode(serenity::experimental::FlushSetting::never);
				flushWorker.flushThreadEnabled.store(false);
		}
	}

	bool FileTarget::CloseFile() {
		StopBackgroundThread();
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
				std::this_thread::sleep_for(std::chrono::nanoseconds(100));
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
		if( flushThreadEnabled ) {
				if( lock.owns_lock() ) {
						lock.unlock();
				}
				flushWorker.threadWriting.store(false);
				flushWorker.threadWriting.notify_one();
		}
	}

	void FileTarget::Flush() {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( isMTSupportEnabled() ) {
				lock.lock();
		}
		auto flushThreadEnabled { flushWorker.flushThreadEnabled.load() };
		if( flushThreadEnabled ) {
				if( flushWorker.threadWriting.load() ) {
						flushWorker.threadWriting.wait(true);
				}
				flushWorker.flushComplete.store(false);
		}
		// If formatted message wasn't written to file and instead was written to the buffer, write to file now
		if( Buffer()->size() != 0 ) {
				fileHandle.rdbuf()->sputn(Buffer()->data(), Buffer()->size());
				Buffer()->clear();
		}
		fileHandle.flush();
#ifndef NDEBUG
		if( fileHandle.rdbuf()->pubsync() == 0 ) {
				DB_PRINT("File Contents Fushed And Synced To DIsk For File\n\t {}\n", FileName());
				std::getchar();
		} else {
				DB_PRINT("File Contents Fushed But Not Synced To DIsk For File\n\t {}\n", FileName());
				std::getchar();
			}
#else
		fileHandle.rdbuf()->pubsync();
#endif
		if( flushThreadEnabled ) {
				if( lock.owns_lock() ) {
						lock.unlock();
				}
				flushWorker.flushComplete.store(true);
				flushWorker.flushComplete.notify_one();
		}
	}

	void FileTarget::SetLocale(const std::locale& loc) {
		if( fileHandle.getloc() != loc ) {
				fileHandle.imbue(loc);
		}
		TargetBase::SetLocale(loc);
	}

	void FileTarget::WriteToBaseBuffer(bool fmtToBuf) {
		TargetBase::WriteToBaseBuffer(fmtToBuf);
	}

	const bool FileTarget::isWriteToBuf() {
		return TargetBase::isWriteToBuf();
	}

	std::string* const FileTarget::Buffer() {
		return TargetBase::Buffer();
	}

	// TODO: Look into this and possibl how I'm waiting with the atomics and locking
	// I genuinely believe this is where my issue is on timed flushing =/ the specific calls for everything else;
	// level based, rotational based (even the sub setting rotations) all work -> however, I seem to be unable
	// to flush the contents on a timed basis reliably
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
						if( flushWorker.flushThreadEnabled.load() ) return;
						// lambda that starts a background thread to flush on time interval given
						auto periodic_flush = [ this ]() {
							namespace ch = std::chrono;
							ch::milliseconds lastTimePoint { 0 };
							std::mutex threadMutex;

							while( !flushWorker.cleanUpThreads.load() ) {
									std::unique_lock<std::mutex> lock(threadMutex);
									auto now = ch::duration_cast<ch::milliseconds>(
									ch::system_clock::now().time_since_epoch());
									auto elapsed = ch::duration_cast<ch::milliseconds>(now - lastTimePoint);
									if( elapsed >= policy.SecondarySettings().flushEvery ) {
											Flush();
									}
									lastTimePoint = now;
								}
						};    // periodic_flush lambda

						if( !flushWorker.flushThreadEnabled.load() ) {
								flushWorker.flushThread = std::thread(periodic_flush);
								flushWorker.flushThreadEnabled.store(true);
						}
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
