#pragma once

#include <serenity/Targets/FileTarget.h>

namespace serenity::experimental::targets
{
	class RotatingTarget : public serenity::targets::FileTarget, serenity::experimental::RotateSettings
	{
	  public:
		RotatingTarget( );
		explicit RotatingTarget( std::string_view name, std::string_view filePath, bool replaceIfExists = false );
		explicit RotatingTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath,
								 bool replaceIfExists = false );
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
		void                               SetRotationSetting( IntervalMode mode, size_t interval );
		void                               SetRotationMode( IntervalMode mode );
		const RotateSettings::IntervalMode RotationMode( );

	  private:
		void         PrintMessage( std::string_view formatted ) override;
		void         RenameFileForRotation( );
		bool         shouldRotate;
		IntervalMode m_mode { IntervalMode::file_size };
		int          currentDay, currentWeekday, currentHour;
	};

}  // namespace serenity::experimental::targets

/******************************************************** Note to self: *****************************************************
	All rotating modes seem to work - I've only tested hourly and daily so far, but weekly and monthly follow the same logic
	so I assume those should work as well. Definitely need to test those two though instead of making assumptions here.

	As a secondary note though, I'm debating if I only want the current renaming method to only apply to file size rotations.
	- i.e. The current method of Rotating_Log.txt -> Rotating_Log_01.txt -> Rotating_Log_02.txt -> etc...

	Main reason is that it could be helpful to users if say each file has a date format or hour marker:
	- For hourly logging, Rotating_Log.txt -> Rotating_Log_8pm.txt -> Rotating_Log_9pm.txt -> etc...
	- For daily and weekly logging, Rotating_Log.txt -> Rotating_Log_09Feb22.txt -> Rotating_Log_16Feb22.txt
	- For monthly logging, Rotating_Log.txt -> Rotating_Log_Feb22.txt -> Rotating_Log_Mar22.txt
****************************************************************************************************************************/