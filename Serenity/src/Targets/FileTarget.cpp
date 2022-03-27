#include <serenity/Targets/FileTarget.h>

#include <iostream>    // specifically for std::cerr

namespace serenity::targets {

	FileTarget::FileTarget(): TargetBase("File_Logger"), fileMutex(std::mutex {}), fileHelper("") {
		fileHelper.BaseHelper() = baseHelper;
		std::filesystem::path logDirPath { std::filesystem::current_path() /= "Logs" };
		fileHelper.InitializeFilePath();
		try {
				if( !std::filesystem::exists(logDirPath) ) {
						std::filesystem::create_directories(logDirPath);
						fileHelper.OpenFile(true);
				} else {
						fileHelper.OpenFile(true);
					}
			}
		catch( const std::exception& e ) {
				std::cerr << e.what() << "\n";
				fileHelper.CloseFile();
			}
		if( fileHelper.FileHandle().getloc() != MsgInfo()->GetLocale() ) {
				fileHelper.FileHandle().imbue(MsgInfo()->GetLocale());
		}
	}

	FileTarget::FileTarget(std::string_view fileName, bool replaceIfExists)
		: TargetBase("File_Logger"), fileMutex(std::mutex {}), fileHelper("") {
		fileHelper.BaseHelper() = baseHelper;
		std::filesystem::path logDirPath { std::filesystem::current_path() /= "Logs" };
		fileHelper.InitializeFilePath(fileName);
		try {
				if( !std::filesystem::exists(logDirPath) ) {
						std::filesystem::create_directories(logDirPath);
						fileHelper.OpenFile(replaceIfExists);
				} else {
						fileHelper.OpenFile(replaceIfExists);
					}
			}
		catch( const std::exception& e ) {
				std::cerr << e.what() << "\n";
				fileHelper.CloseFile();
			}
		if( fileHelper.FileHandle().getloc() != MsgInfo()->GetLocale() ) {
				fileHelper.FileHandle().imbue(MsgInfo()->GetLocale());
		}
	}

	FileTarget::FileTarget(std::string_view name, std::string_view fPath, bool replaceIfExists)
		: TargetBase(name), fileMutex(std::mutex {}), fileHelper(fPath) {
		fileHelper.BaseHelper() = baseHelper;
		try {
				if( !std::filesystem::exists(fileHelper.FileCacheHelper()->FilePath()) ) {
						auto dir { fileHelper.FileCacheHelper()->FilePath() };
						dir.remove_filename();
						std::filesystem::create_directories(dir);
						fileHelper.OpenFile(replaceIfExists);
				} else {
						fileHelper.OpenFile(replaceIfExists);
					}
			}
		catch( const std::exception& e ) {
				std::cerr << e.what() << "\n";
				fileHelper.CloseFile();
			}
		if( fileHelper.FileHandle().getloc() != MsgInfo()->GetLocale() ) {
				fileHelper.FileHandle().imbue(MsgInfo()->GetLocale());
		}
	}

	FileTarget::FileTarget(std::string_view name, std::string_view formatPattern, std::string_view fPath, bool replaceIfExists)
		: TargetBase(name, formatPattern), fileHelper(fPath) {
		fileHelper.BaseHelper() = baseHelper;
		try {
				if( !std::filesystem::exists(fileHelper.FileCacheHelper()->FilePath()) ) {
						auto dir { fileHelper.FileCacheHelper()->FilePath() };
						dir.remove_filename();
						std::filesystem::create_directories(dir);
						fileHelper.OpenFile(replaceIfExists);
				} else {
						fileHelper.OpenFile(replaceIfExists);
					}
			}
		catch( const std::exception& e ) {
				std::cerr << e.what() << "\n";
				fileHelper.CloseFile();
			}
		if( fileHelper.FileHandle().getloc() != MsgInfo()->GetLocale() ) {
				fileHelper.FileHandle().imbue(MsgInfo()->GetLocale());
		}
	}

	FileTarget::~FileTarget() {
		fileHelper.StopBackgroundThread();
		fileHelper.CloseFile();
	}

	bool FileTarget::RenameFile(std::string_view newFileName) {
		return fileHelper.RenameFile(newFileName);
	}

	void FileTarget::PrintMessage(std::string_view formatted) {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		auto& backgroundThread { fileHelper.BackgoundThreadInfo() };
		auto flushThreadEnabled { backgroundThread.flushThreadEnabled.load() };
		if( flushThreadEnabled ) {
				if( !backgroundThread.flushComplete.load() ) {
						backgroundThread.flushComplete.wait(false);
				}
				backgroundThread.threadWriting.store(true);
		}
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		fileHelper.FileHandle().rdbuf()->sputn(formatted.data(), formatted.size());
		if( lock.owns_lock() ) {
				lock.unlock();
		}
		if( flushThreadEnabled ) {
				backgroundThread.threadWriting.store(false);
				backgroundThread.threadWriting.notify_all();
		}
	}

	void FileTarget::SetLocale(const std::locale& loc) {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		if( fileHelper.FileHandle().getloc() != loc ) {
				fileHelper.FileHandle().imbue(loc);
		}
		TargetBase::SetLocale(loc);
	}

	void FileTarget::PolicyFlushOn() {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( baseHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		if( baseHelper.Policy().PrimarySetting() == serenity::experimental::FlushSetting::never ) {
				// If the flush thread was active, no need to hog a thread if never flushing
				fileHelper.StopBackgroundThread();
		};
		if( baseHelper.Policy().PrimarySetting() == serenity::experimental::FlushSetting::always ) {
				// Similar reasoning as Never setting except for the fact of ALWAYS flushing
				fileHelper.StopBackgroundThread();
				fileHelper.Flush();
		}
		switch( baseHelper.Policy().SubSetting() ) {
				case serenity::experimental::PeriodicOptions::timeBased:
					{
						fileHelper.StartBackgroundThread();
					}
					break;    // time based bounds
				case serenity::experimental::PeriodicOptions::logLevelBased:
					{
						if( MsgInfo()->MsgLevel() < baseHelper.Policy().SecondarySettings().flushOn ) return;
						fileHelper.Flush();
					}
					break;
			}    // Sub Option Check
	}                    // PolicyFlushOn( ) Function

	bool FileTarget::OpenFIle(bool truncate) {
		return fileHelper.OpenFile(truncate);
	}

	bool FileTarget::CloseFIle() {
		return fileHelper.CloseFile();
	}

	void FileTarget::Flush() {
		fileHelper.Flush();
	}

	void FileTarget::SetFileBufferSize(size_t newValue) {
		fileHelper.SetFileBufferSize(newValue);
	}

}    // namespace serenity::targets
