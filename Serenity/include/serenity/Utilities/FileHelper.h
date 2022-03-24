#pragma once

#include <serenity/Common.h>
#include <serenity/Utilities/TargetHelper.h>

#include <fstream>

// TODO: Finish abstracting these away so that any fIle-based target can add this helper class in composition rather than the current inheritance
// model

namespace serenity {

	struct BackgroundThread
	{
		std::jthread flushThread;
		std::stop_token interruptThread {};
		std::atomic<bool> cleanUpThreads { false };
		std::atomic<bool> flushThreadEnabled { false };
		std::atomic<bool> flushComplete { true };
		std::atomic<bool> threadWriting { false };
		std::atomic<bool> pauseThread { false };
	};

	struct FileSettings
	{
		std::filesystem::path filePath;
		std::vector<char> fileBuffer;
		size_t bufferSize { DEFAULT_BUFFER_SIZE };
	};

}    // namespace serenity

namespace serenity::targets::helpers {

	class FIleHelper: public BaseTargetHelper
	{
	      public:
		bool OpenFile(bool truncate = false);
		bool CloseFile();
		void Flush();
		std::ofstream& FileHandle();
		FileSettings& FileOptions();
		BackgroundThread& BackgoundThreadInfo();
		void BackgroundFlushThread(std::stop_token stopToken);
		void StopBackgroundThread();
		void StartBackgroundThread();
		void PauseBackgroundThread();
		void ResumeBackgroundThread();

	      private:
		int retryAttempt { 5 };
		BackgroundThread flushWorker;
		std::ofstream fileHandle;
		FileSettings fileOptions;
		std::mutex fileHelperMutex;
	};

}    // namespace serenity::targets::helpers