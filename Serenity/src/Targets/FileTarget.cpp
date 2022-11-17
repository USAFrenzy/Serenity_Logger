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

	FileTarget::FileTarget(std::string_view fileName, bool replaceIfExists): TargetBase("File_Logger"), FileHelper(""), fileMutex(std::mutex {}) {
		SyncTargetHelpers(TargetHelper());
		std::filesystem::path filePath { fileName };
		filePath.has_relative_path() ? InitializeLogs(filePath.string(), replaceIfExists)
									 : InitializeLogs(((std::filesystem::current_path() /= "Logs") /= fileName).string(), replaceIfExists);
	}

	FileTarget::FileTarget(std::string_view name, std::string_view fPath, bool replaceIfExists): TargetBase(name), FileHelper(fPath), fileMutex(std::mutex {}) {
		SyncTargetHelpers(TargetHelper());
		std::filesystem::path filePath { fPath };
		filePath.has_relative_path() ? InitializeLogs(filePath.string(), replaceIfExists) : InitializeLogs(FileCacheHelper()->FilePath().string(), replaceIfExists);
	}

	FileTarget::FileTarget(std::string_view name, std::string_view formatPattern, std::string_view fPath, bool replaceIfExists)
		: TargetBase(name, formatPattern), FileHelper(fPath), fileMutex(std::mutex {}) {
		SyncTargetHelpers(TargetHelper());
		std::filesystem::path filePath { fPath };
		filePath.has_relative_path() ? InitializeLogs(filePath.string(), replaceIfExists) : InitializeLogs(FileCacheHelper()->FilePath().string(), replaceIfExists);
	}

	FileTarget::~FileTarget() {
		StopBackgroundThread();
		CloseFile();
	}

	void FileTarget::PrintMessage() {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		auto& backgroundThread { BackgoundThreadInfo() };
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
		// TODO------------------------------------------------------------------------------------------------------------------------
		// This is where the major time descrepency for both the File Target and Rotating Target lies
		//	const auto& msgDetailsRef { *(MsgInfo().get()) };
		VFORMAT_TO(FileBuffer(), MsgFmt()->Locale(), MsgFmt()->Pattern(), *(MsgInfo().get()), MsgInfo()->TimeInfo());
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
		if( policy->PrimarySetting() == serenity::experimental::FlushSetting::never ) {
				// If the flush thread was active, no need to hog a thread if never flushing
				StopBackgroundThread();
		};
		if( policy->PrimarySetting() == serenity::experimental::FlushSetting::always ) {
				// Similar reasoning as Never setting except for the fact of ALWAYS flushing
				StopBackgroundThread();
				Flush();
		}
		switch( policy->SubSetting() ) {
				case serenity::experimental::PeriodicOptions::timeBased:
					{
						StartBackgroundThread();
					}
					break;    // time based bounds
				case serenity::experimental::PeriodicOptions::logLevelBased:
					{
						if( MsgInfo()->MsgLevel() < policy->SecondarySettings().flushOn ) return;
						Flush();
					}
					break;
			}    // Sub Option Check
	}            // PolicyFlushOn( ) Function

}    // namespace serenity::targets
