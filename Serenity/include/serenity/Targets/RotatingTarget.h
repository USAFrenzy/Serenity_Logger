#pragma once

#include <serenity/Targets/FileTarget.h>

namespace serenity::experimental::targets {
	class RotatingTarget : public serenity::targets::FileTarget, serenity::experimental::RotateSettings
	{
	  public:
		RotatingTarget( );
		explicit RotatingTarget( std::string_view name, std::string_view filePath, bool replaceIfExists = false );
		explicit RotatingTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath, bool replaceIfExists = false );
		RotatingTarget( const RotatingTarget & ) = delete;
		RotatingTarget &operator=( const RotatingTarget & ) = delete;
		~RotatingTarget( );
		void               WriteToBaseBuffer( bool fmtToBuf = true );
		const bool         isWriteToBuf( );
		std::string *const Buffer( );
		void               EnableRotation( bool shouldRotate = true );
		void               SetRotateSettings( RotateSettings settings );
		void               RotateFile( );
		bool               RenameFile( std::string_view newFileName ) override;
		bool               ShouldRotate( );
		// ################################# WIP #################################
		// For IntervalMode::file_size -> interval is the file size limit
		// For IntervalMode::daily, the interval is the hour the logger should rotate on (0-23)
		// For IntervalMode::weekly -> interval is the weekday that the logger should rotate on (0-6)
		// For IntervalMode::monthly -> interval is the day of the month the logger should rotate on (1-31)
		// EXAMPLES:
		// - SetRotationSetting( IntervalMode::daily, 23); -> rotate every day at 11PM
		// - SetRotationSetting(IntervalMode::file_size, 512*KB) -> rotate when file hits 512KB
		// - SetRotationSetting(IntervalMode::weekly, 2) -> Rotate Every Tuesday
		// - SetRotationSetting(IntervalMode::monthly, 24) -> Rotate Every Month on the 24th day of that month
		// This gives granularity over which setting to change vs all settings in SetRotateSettings();
		void                               SetRotationSetting( IntervalMode mode, size_t setting, size_t secondSetting = 0 );
		void                               SetRotationMode( IntervalMode mode );
		const RotateSettings::IntervalMode RotationMode( );

	  private:
		void         PrintMessage( std::string_view formatted ) override;
		bool         shouldRotate;
		IntervalMode m_mode;
		int          currentDay, currentWeekday, currentHour;
	};

}  // namespace serenity::experimental::targets