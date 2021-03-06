#pragma once

#include <serenity/Targets/Target.h>
#include <serenity/Utilities/FileHelper.h>

namespace serenity::experimental {

	class RotateSettings: public serenity::targets::helpers::FileHelper
	{
	      public:
		explicit RotateSettings(const std::string& path);
		RotateSettings(RotateSettings&)            = delete;
		RotateSettings& operator=(RotateSettings&) = delete;
		~RotateSettings()                          = default;
		std::filesystem::path OriginalPath() const;
		std::filesystem::path OriginalDirectory() const;
		std::string OriginalName() const;
		std::string OriginalExtension() const;
		size_t FileSize() const;
		bool IsIntervalRotationEnabled() const;

		enum class IntervalMode
		{
			file_size = 0,
			hourly    = 1,
			daily     = 2,
			weekly    = 3,
			monthly   = 4,
		};

		size_t maxNumberOfFiles;
		size_t fileSizeLimit;
		int dayModeSettingHour;
		int dayModeSettingMinute;
		int weekModeSetting;
		int monthModeSetting;

	      protected:
		void SetCurrentFileSize(size_t currentSize);
		void EnableFirstRotation(bool enabled = true);

	      private:
		size_t currentFileSize;
		bool initalRotationEnabled;
	};
}    // namespace serenity::experimental

namespace serenity::experimental::targets {

	class RotatingTarget: public serenity::targets::TargetBase, public serenity::experimental::RotateSettings
	{
	      public:
		RotatingTarget();
		explicit RotatingTarget(std::string_view name, std::string_view filePath, bool replaceIfExists = false);
		explicit RotatingTarget(std::string_view name, std::string_view formatPattern, std::string_view filePath,
		                        bool replaceIfExists = false);
		RotatingTarget(const RotatingTarget&)            = delete;
		RotatingTarget& operator=(const RotatingTarget&) = delete;
		~RotatingTarget();

		void EnableRotation(bool shouldRotate = true);
		void SetRotateSettings(RotateSettings settings);
		void RotateFile();
		bool RenameFile(std::string_view newFileName) override;
		bool ShouldRotate();
		void SetLocale(const std::locale& loc) override;
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
		RotateSettings::IntervalMode RotationMode() const;

	protected:
		void PrintMessage(std::string_view formatted) override;
		void PolicyFlushOn() override;
		bool RenameFileInRotation(std::filesystem::path newFilePath);
		bool ReplaceOldFileInRotation();

	private:
		bool rotationEnabled;
		IntervalMode m_mode;
		int currentDay;
		int currentWeekday;
		int currentHour;
		mutable std::mutex rotatingMutex;
	};

}    // namespace serenity::experimental::targets