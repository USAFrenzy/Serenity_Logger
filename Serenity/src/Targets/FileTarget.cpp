#include <serenity/Targets/FileTarget.h>

#include <iostream>    // specifically for std::cerr

namespace serenity::targets {

	FileTarget::FileTarget(): TargetBase("File_Logger"), FileHelper(""), fileMutex(std::mutex {}) {
		SyncTargetHelpers(TargetHelper());
		std::filesystem::path logDirPath { std::filesystem::current_path() /= "Logs" };
		InitializeFilePath();
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
		if( FileHandle().getloc() != MsgInfo()->GetLocale() ) {
				FileHandle().imbue(MsgInfo()->GetLocale());
		}
	}

	FileTarget::FileTarget(std::string_view fileName, bool replaceIfExists): TargetBase("File_Logger"), FileHelper(""), fileMutex(std::mutex {}) {
		SyncTargetHelpers(TargetHelper());
		std::filesystem::path logDirPath { std::filesystem::current_path() /= "Logs" };
		InitializeFilePath(fileName);
		try {
				if( !std::filesystem::exists(logDirPath) ) {
						std::filesystem::create_directories(logDirPath);
						OpenFile(replaceIfExists);
				} else {
						OpenFile(replaceIfExists);
					}
			}
		catch( const std::exception& e ) {
				std::cerr << e.what() << "\n";
				CloseFile();
			}
		if( FileHandle().getloc() != MsgInfo()->GetLocale() ) {
				FileHandle().imbue(MsgInfo()->GetLocale());
		}
	}

	FileTarget::FileTarget(std::string_view name, std::string_view fPath, bool replaceIfExists): TargetBase(name), FileHelper(fPath), fileMutex(std::mutex {}) {
		SyncTargetHelpers(TargetHelper());
		try {
				if( !std::filesystem::exists(FileCacheHelper()->FilePath()) ) {
						auto dir { FileCacheHelper()->FilePath() };
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
		if( FileHandle().getloc() != MsgInfo()->GetLocale() ) {
				FileHandle().imbue(MsgInfo()->GetLocale());
		}
	}

	FileTarget::FileTarget(std::string_view name, std::string_view formatPattern, std::string_view fPath, bool replaceIfExists)
			: TargetBase(name, formatPattern), FileHelper(fPath), fileMutex(std::mutex {}) {
		SyncTargetHelpers(TargetHelper());
		try {
				if( !std::filesystem::exists(FileCacheHelper()->FilePath()) ) {
						auto dir { FileCacheHelper()->FilePath() };
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
		if( FileHandle().getloc() != MsgInfo()->GetLocale() ) {
				FileHandle().imbue(MsgInfo()->GetLocale());
		}
	}

	FileTarget::~FileTarget() {
		StopBackgroundThread();
		CloseFile();
	}

	void FileTarget::PrintMessage(std::string_view formatted) {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		auto& backgroundThread { BackgoundThreadInfo() };
		auto flushThreadEnabled { backgroundThread->flushThreadEnabled.load() };
		if( flushThreadEnabled ) {
				if( !backgroundThread->flushComplete.load() ) {
						backgroundThread->flushComplete.wait(false);
				}
				backgroundThread->threadWriting.store(true);
		}
		if( TargetHelper()->isMTSupportEnabled() ) {
				lock.lock();
		}
		FileHandle().rdbuf()->sputn(formatted.data(), formatted.size());
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
		if( FileHandle().getloc() != loc ) {
				FileHandle().imbue(loc);
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
