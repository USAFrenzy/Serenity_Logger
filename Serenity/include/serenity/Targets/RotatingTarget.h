#pragma once

#include <serenity/Targets/Target.h>
#include <serenity/Utilities/FileHelper.h>

namespace serenity::experimental {

	struct RotateLimits
	{
		size_t maxNumberOfFiles { 5 };
		size_t fileSizeLimit { 512 * KB };
		int dayModeSettingHour { 0 };
		int dayModeSettingMinute { 0 };
		int weekModeSetting { 0 };
		int monthModeSetting { 1 };
	};

	enum class IntervalMode
	{
		file_size = 0,
		hourly    = 1,
		daily     = 2,
		weekly    = 3,
		monthly   = 4,
	};

	class RotateSettings: public serenity::targets::helpers::FileHelper
	{
	  public:
		explicit RotateSettings(const std::string& path);
		explicit RotateSettings(const RotateLimits& limits);
		explicit RotateSettings(const std::string& path, const RotateLimits& limits);
		RotateSettings(RotateSettings&)            = delete;
		RotateSettings& operator=(RotateSettings&) = delete;
		~RotateSettings()                          = default;

		std::filesystem::path OriginalPath() const;
		std::filesystem::path OriginalDirectoryPath() const;
		std::string OriginalDirName() const;

		std::string OriginalName() const;
		std::string OriginalExtension() const;
		size_t FileSize() const;
		void SetRotationLimits(const RotateLimits& limits);
		RotateLimits RotationLimits() const;

	  protected:
		void SetCurrentFileSize(size_t currentSize);

	  private:
		RotateLimits settingLimits;
		size_t currentFileSize;
		bool initalRotationEnabled;
	};
}    // namespace serenity::experimental

namespace serenity::experimental::targets {

	// Initialize to negative values so condition checks pass if isn't DS and these haven't been
	// updated rather than zero-initializing them and causing a potential false positive conditional check

	struct RotatingDaylightCache
	{
		bool initialDSValue { false };
		bool dsShouldRotate { false };
		int dsHour { -1 };
		int dsMinute { -1 };
		int dsWkDay { -1 };
		int dsDayOfMonth { -1 };
	};

	class RotatingTarget: public serenity::targets::TargetBase, public serenity::experimental::RotateSettings
	{
	  public:
		explicit RotatingTarget();
		explicit RotatingTarget(std::string_view name, std::string_view filePath, bool replaceIfExists = false);
		explicit RotatingTarget(std::string_view name, std::string_view formatPattern, std::string_view filePath, bool replaceIfExists = false);
		RotatingTarget(const RotatingTarget&)            = delete;
		RotatingTarget& operator=(const RotatingTarget&) = delete;
		~RotatingTarget();

		void EnableRotation(bool shouldRotate = true);
		void SetRotationLimits(const RotateLimits& limits);
		RotateLimits RotationLimits() const;

		void RotateFile();
		bool RenameFile(std::string_view newFileName) override;
		bool ShouldRotate();
		void SetLocale(const std::locale& loc) override;
		void TruncLogOnFileSize(bool truncate);

		// clang-format off
		// ################################# WIP #################################

		// For IntervalMode::file_size -> interval is the file size limit
		// For IntervalMode::daily, the interval is the hour the logger should rotate on (0-23) 
		// For IntervalMode::weekly -> interval is the weekday that the logger should rotate on (0-6) 
		// For IntervalMode::monthly -> interval is the day of the month the logger should rotate on (1-31) 
		// EXAMPLES:
		// - SetRotationSetting( IntervalMode::daily, 23); -> rotate every day at 11PM
		// - SetRotationSetting(IntervalMode::file_size, 512*KB) -> rotate when file hits 512KB
		// - SetRotationSetting(IntervalMode::weekly, 2) -> Rotate Every Tuesday
		// - SetRotationSetting(IntervalMode::monthly, 24) -> Rotate Every Month on the 24th day of that month.
		void SetRotationSetting(IntervalMode mode, int setting = 0, int secondSetting = 0);
		// clang-format on	
	
		void SetRotationMode(IntervalMode mode);
		IntervalMode RotationMode() const;

	protected:
		void PrintMessage() override;
		void PolicyFlushOn() override;
		bool RenameFileInRotation(std::filesystem::path newFilePath);
		bool ReplaceOldFileInRotation();

	private:
		// added these to account for premessage stamp size when checking in ShouldRotate(),
		// which was unaccounted for with original usage of MsgInfo()->MessageSize()
		void SetMessageSize(size_t size); 
		size_t MessageSize() const; 

	private:
		bool rotationEnabled;
		IntervalMode m_mode;
		std::tm currentCache;
		mutable std::mutex rotatingMutex;
		size_t messageSize;
		RotatingDaylightCache dsCache;
		bool isAboveMsgLimit;
		bool truncateMessage;
		std::array<char, 6> numBuff;
	};

}    // namespace serenity::experimental::targets