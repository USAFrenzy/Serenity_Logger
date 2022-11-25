#include <serenity/Targets/FileTarget.h>

#include <iostream>    // specifically for std::cerr

namespace serenity::targets {

	void FileTarget::InitializeLogs(const std::string_view fileName, bool replaceIfExists) {
		std::filesystem::path filePath { fileName };
		if( filePath.has_relative_path() ) {
				InitializeFilePath(filePath.string());
				try {
						if( !std::filesystem::exists(FileCacheHelper()->DirPath()) ) {
								std::filesystem::create_directories(FileCacheHelper()->DirPath());
								OpenFile(replaceIfExists);
						} else {
								OpenFile(replaceIfExists);
							}
					}
				catch( const std::exception& e ) {
						std::cerr << e.what() << "\n";
						CloseFile();
					}
		}
	}

	FileTarget::FileTarget(): TargetBase("File_Logger"), FileHelper(""), fileMutex(std::mutex {}) {
		SyncTargetHelpers(TargetHelper());
		std::filesystem::path logDirPath { std::filesystem::current_path() /= "Logs" };
		InitializeLogs(((std::filesystem::current_path() /= "Logs") /= "Generic_Log.txt").string(), true);
	}

	FileTarget::FileTarget(utf_utils::InputSource fileName, bool replaceIfExists): TargetBase("File_Logger"), FileHelper(""), fileMutex(std::mutex {}) {
		SyncTargetHelpers(TargetHelper());
		std::filesystem::path filePath { fileName.input };
		filePath.has_relative_path() ? InitializeLogs(filePath.string(), replaceIfExists)
									 : InitializeLogs(((std::filesystem::current_path() /= "Logs") /= fileName.input).string(), replaceIfExists);
	}

	FileTarget::FileTarget(utf_utils::InputSource name, utf_utils::InputSource fPath, bool replaceIfExists)
		: TargetBase(std::move(name.input)), FileHelper(fPath.input), fileMutex(std::mutex {}) {
		SyncTargetHelpers(TargetHelper());
		std::filesystem::path filePath { std::move(fPath.input) };
		filePath.has_relative_path() ? InitializeLogs(filePath.string(), replaceIfExists) : InitializeLogs(FileCacheHelper()->FilePath().string(), replaceIfExists);
	}

	FileTarget::FileTarget(utf_utils::InputSource name, utf_utils::InputSource formatPattern, utf_utils::InputSource fPath, bool replaceIfExists)
		: TargetBase(std::move(name.input), std::move(formatPattern.input)), FileHelper(fPath.input), fileMutex(std::mutex {}) {
		SyncTargetHelpers(TargetHelper());
		std::filesystem::path filePath { std::move(fPath.input) };
		filePath.has_relative_path() ? InitializeLogs(filePath.string(), replaceIfExists) : InitializeLogs(FileCacheHelper()->FilePath().string(), replaceIfExists);
	}

	FileTarget::~FileTarget() {
		StopBackgroundThread();
		CloseFile();
	}

	void FileTarget::PrintMessage() {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		const auto& backgroundThread { BackgoundThreadInfo() };
		const auto& flushThreadEnabled { backgroundThread->flushThreadEnabled.load() };
		if( flushThreadEnabled ) {
				if( !backgroundThread->flushComplete.load() ) {
						backgroundThread->flushComplete.wait(false);
				}
				backgroundThread->threadWriting.store(true);
		}
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		FormatLogMessage(FileBuffer());
		WriteToFile();
		if( lock.owns_lock() ) {
				lock.unlock();
		}
		if( flushThreadEnabled ) {
				backgroundThread->threadWriting.store(false);
				backgroundThread->threadWriting.notify_all();
		}
	}

	void FileTarget::SetLocale(const std::locale& loc) {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		TargetBase::SetLocale(loc);
	}

	void FileTarget::PolicyFlushOn() {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		auto& policy { TargetHelper()->Policy() };
		if( policy->PrimarySetting() == FlushSetting::never ) {
				// If the flush thread was active, no need to hog a thread if never flushing
				StopBackgroundThread();
		};
		if( policy->PrimarySetting() == FlushSetting::always ) {
				// Similar reasoning as Never setting except for the fact of ALWAYS flushing
				StopBackgroundThread();
				Flush();
		}
		switch( policy->SubSetting() ) {
				case PeriodicOptions::timeBased:
					{
						StartBackgroundThread();
					}
					break;    // time based bounds
				case PeriodicOptions::logLevelBased:
					{
						if( MsgInfo()->MsgLevel() < policy->SecondarySettings().flushOn ) return;
						Flush();
					}
					break;
			}    // Sub Option Check
	}            // PolicyFlushOn( ) Function

}    // namespace serenity::targets
