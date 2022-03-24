#include <serenity/Targets/FileTarget.h>

#include <iostream>    // specifically for std::cerr

namespace serenity::targets {

	FileTarget::FileTarget(): TargetBase("File_Logger") {
		fileHelper.FileOptions().fileBuffer.reserve(fileHelper.FileOptions().bufferSize);
		std::filesystem::path fullFilePath = std::filesystem::current_path();
		const auto logDir { "Logs" };
		const auto logDirPath { fullFilePath };
		fullFilePath /= logDir;
		fullFilePath /= "Generic_Log.txt";
		fileHelper.FileOptions().filePath = fullFilePath.make_preferred().string();
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

	FileTarget::FileTarget(std::string_view fileName, bool replaceIfExists): TargetBase("File_Logger") {
		fileHelper.FileOptions().fileBuffer.reserve(fileHelper.FileOptions().bufferSize);
		std::filesystem::path fullFilePath = std::filesystem::current_path();
		const auto logDir { "Logs" };
		const auto logDirPath { fullFilePath };
		fullFilePath /= logDir;
		fullFilePath /= fileName;
		fileHelper.FileOptions().filePath = fullFilePath.make_preferred().string();
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

	FileTarget::FileTarget(std::string_view name, std::string_view fPath, bool replaceIfExists): TargetBase(name) {
		fileHelper.FileOptions().fileBuffer.reserve(fileHelper.FileOptions().bufferSize);
		fileHelper.FileOptions().filePath = fPath;
		fileHelper.FileOptions().filePath.make_preferred();
		try {
				if( !std::filesystem::exists(fileHelper.FileOptions().filePath) ) {
						auto dir { fileHelper.FileOptions().filePath };
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
		: TargetBase(name, formatPattern) {
		fileHelper.FileOptions().fileBuffer.reserve(fileHelper.FileOptions().bufferSize);
		fileHelper.FileOptions().filePath = fPath;
		fileHelper.FileOptions().filePath.make_preferred();
		try {
				if( !std::filesystem::exists(fileHelper.FileOptions().filePath) ) {
						auto dir { fileHelper.FileOptions().filePath };
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

	const std::string FileTarget::FilePath() {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( fileHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		return fileHelper.FileOptions().filePath.string();
	}

	const std::string FileTarget::FileName() {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( fileHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		return fileHelper.FileOptions().filePath.filename().string();
	}

	bool FileTarget::RenameFile(std::string_view newFileName) {
		try {
				fileHelper.CloseFile();
				// make copy for old file conversion
				std::filesystem::path newFile { fileHelper.FileOptions().filePath };
				newFile.replace_filename(newFileName);
				std::filesystem::rename(fileHelper.FileOptions().filePath, newFile);
				fileHelper.FileOptions().filePath = std::move(newFile);
				return fileHelper.OpenFile();
			}
		catch( const std::exception& e ) {
				std::cerr << "Error In Renaming File:\n";
				std::cerr << e.what();
				return false;
			}
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
		if( fileHelper.isMTSupportEnabled() ) {
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
		if( fileHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		if( fileHelper.FileHandle().getloc() != loc ) {
				fileHelper.FileHandle().imbue(loc);
		}
		TargetBase::SetLocale(loc);
	}

	void FileTarget::PolicyFlushOn() {
		std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
		if( fileHelper.isMTSupportEnabled() ) {
				lock.lock();
		}
		if( fileHelper.Policy().PrimarySetting() == serenity::experimental::FlushSetting::never ) {
				// If the flush thread was active, no need to hog a thread if never flushing
				fileHelper.StopBackgroundThread();
		};
		if( fileHelper.Policy().PrimarySetting() == serenity::experimental::FlushSetting::always ) {
				// Similar reasoning as Never setting except for the fact of ALWAYS flushing
				fileHelper.StopBackgroundThread();
				fileHelper.Flush();
		}
		switch( fileHelper.Policy().SubSetting() ) {
				case serenity::experimental::PeriodicOptions::timeBased:
					{
						fileHelper.StartBackgroundThread();
					}
					break;    // time based bounds
				case serenity::experimental::PeriodicOptions::logLevelBased:
					{
						if( MsgInfo()->MsgLevel() < fileHelper.Policy().SecondarySettings().flushOn ) return;
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

}    // namespace serenity::targets
