#include <serenity/Targets/FileTarget.h>

#include <iostream>    // specifically for std::cerr

namespace serenity::targets {

	static int tries { 0 }, retryAttempt { 5 };

	FileTarget::FileTarget(): TargetBase("File_Logger")
	{
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
		} catch( const std::exception &e ) {
				std::cerr << e.what() << "\n";
				CloseFile();
		}
	}

	FileTarget::FileTarget(std::string_view fileName, bool replaceIfExists): TargetBase("File_Logger")
	{
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
		} catch( const std::exception &e ) {
				std::cerr << e.what() << "\n";
				CloseFile();
		}
	}

	FileTarget::FileTarget(std::string_view name, std::string_view fPath, bool replaceIfExists): TargetBase(name)
	{
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
		} catch( const std::exception &e ) {
				std::cerr << e.what() << "\n";
				CloseFile();
		}
	}

	FileTarget::FileTarget(std::string_view name, std::string_view formatPattern, std::string_view fPath, bool replaceIfExists)
		: TargetBase(name, formatPattern)
	{
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
		} catch( const std::exception &e ) {
				std::cerr << e.what() << "\n";
				CloseFile();
		}
	}

	FileTarget::~FileTarget()
	{
		CloseFile();
	}

	const std::string FileTarget::FilePath()
	{
		return fileOptions.filePath.string();
	}

	const std::string FileTarget::FileName()
	{
		return fileOptions.filePath.filename().string();
	}

	bool FileTarget::OpenFile(bool truncate)
	{
		CloseFile();
		try {
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
		} catch( const std::exception &e ) {
				std::cerr << "Error In Opening File:\n";
				std::cerr << e.what() << "\n";
				return false;
		}
		return true;
	}

	bool FileTarget::CloseFile()
	{
		if( !fileHandle.is_open() ) return true;
		try {
				if( flushWorker.flushThread.joinable() ) {
						flushWorker.cleanUpThreads.store(true);
						flushWorker.cleanUpThreads.notify_one();
						flushWorker.flushThread.join();
				}
				Flush();

				auto TryClose = [ &, this ]() {
					fileHandle.close();
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
					return !fileHandle.is_open();
				};

				for( tries; tries <= retryAttempt; ++tries ) {
						if( tries == retryAttempt ) {
								tries = 0;
								break;
						}
						if( TryClose() ) break;
					}
		} catch( const std::exception &e ) {
				std::cerr << "Error In Closing File:\n";
				std::cerr << e.what() << "\n";
				return false;
		}
		return true;
	}

	bool FileTarget::RenameFile(std::string_view newFileName)
	{
		try {
				// make copy for old file conversion
				std::filesystem::path newFile { fileOptions.filePath };
				newFile.replace_filename(newFileName);
				CloseFile();
				std::filesystem::rename(fileOptions.filePath, newFile);
				fileOptions.filePath = std::move(newFile);
				OpenFile();
				return true;
		} catch( const std::exception &e ) {
				std::cerr << "Error In Renaming File:\n";
				std::cerr << e.what();
				return false;
		}
	}

	void FileTarget::PrintMessage(std::string_view formatted)
	{
		// naive guard from always trying to take a lock regardless of whether the
		// background flush thread is running or not (using manual lock due to scoping
		// here)
		auto flushThread { flushWorker.flushThreadEnabled.load(std::memory_order::relaxed) };
		if( flushThread ) {
				// this is where the locking would go --- placeholder ---
				fileHandle.rdbuf()->sputn(formatted.data(), formatted.size());

		} else {
				fileHandle.rdbuf()->sputn(formatted.data(), formatted.size());
			}
	}

	void FileTarget::Flush()
	{
		// If formatted message wasn't written to file and instead was written to the
		// buffer, write to file now
		if( Buffer()->size() != 0 ) {
				fileHandle.rdbuf()->sputn(Buffer()->data(), Buffer()->size());
				Buffer()->clear();
		}
		fileHandle.flush();
	}

	void FileTarget::WriteToBaseBuffer(bool fmtToBuf)
	{
		TargetBase::WriteToBaseBuffer(fmtToBuf);
	}

	const bool FileTarget::isWriteToBuf()
	{
		return TargetBase::isWriteToBuf();
	}

	std::string *const FileTarget::Buffer()
	{
		return TargetBase::Buffer();
	}

	void FileTarget::PolicyFlushOn()
	{
		if( policy.PrimarySetting() == serenity::experimental::FlushSetting::never ) return;
		if( policy.PrimarySetting() == serenity::experimental::FlushSetting::always ) {
				Flush();
				return;
		}
		switch( policy.SubSetting() ) {
				case serenity::experimental::PeriodicOptions::timeBased:
					{
						if( flushWorker.flushThreadEnabled.load(std::memory_order::relaxed) ) return;
						// lambda that starts a background thread to flush on time interval given
						auto periodic_flush = [ this ]() {
							namespace ch = std::chrono;
							static ch::milliseconds lastTimePoint {};
							while( !flushWorker.cleanUpThreads.load(std::memory_order::relaxed) ) {
									auto now     = ch::duration_cast<ch::milliseconds>(ch::system_clock::now().time_since_epoch());
									auto elapsed = ch::duration_cast<ch::milliseconds>(now - lastTimePoint);
									auto shouldFlush { elapsed >= policy.SecondarySettings().flushEvery };
									if( shouldFlush ) {
											// this is where the locking would go --- placeholder ---
											Flush();
									}
									lastTimePoint = now;
								}
						};    // periodic_flush lambda

						if( !flushWorker.flushThreadEnabled.load(std::memory_order::relaxed) ) {
								flushWorker.flushThread = std::thread(periodic_flush);
								flushWorker.flushThreadEnabled.store(true);
						}
					}
					break;    // time based bounds
				case serenity::experimental::PeriodicOptions::logLevelBased:
					{
						if( MsgInfo()->MsgLevel() < policy.SecondarySettings().flushOn ) return;
						Flush();
					}
					break;
				default: break;    // Don't bother with undef field
			}                      // Sub Option Check
	}                              // PolicyFlushOn( ) Function

}    // namespace serenity::targets
