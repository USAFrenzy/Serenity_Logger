#include <serenity/Targets/RotatingTarget.h>

#include <iostream>

namespace serenity::experimental::targets {
	using namespace serenity::targets;

	RotatingTarget::RotatingTarget( ) : FileTarget( "Rotating_Log.txt", true ), shouldRotate( true ), m_mode( IntervalMode::file_size )
	{
		auto &cache { MsgInfo( )->TimeDetails( ).Cache( ) };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
		std::filesystem::path rotationReadyFile { fileOptions.filePath };
		rotationReadyFile.make_preferred( );
		CacheOriginalPathComponents( fileOptions.filePath );
		std::string rotateFile { OriginalName( ) };
		rotateFile.append( "_01" ).append( OriginalExtension( ) );
		rotationReadyFile.replace_filename( rotateFile );
		if( !std::filesystem::exists( rotationReadyFile ) ) {
			CloseFile( );
			std::filesystem::rename( fileOptions.filePath, rotationReadyFile );
			fileOptions.filePath = std::move( rotationReadyFile );
			OpenFile( true );
		} else {
			RotateFile( );
		}
		SetCurrentFileSize( std::filesystem::file_size( fileOptions.filePath ) );
	}

	RotatingTarget::RotatingTarget( std::string_view name, std::string_view filePath, bool replaceIfExists )
		: FileTarget( name, filePath, replaceIfExists ), shouldRotate( true ), m_mode( IntervalMode::file_size )
	{
		auto &cache { MsgInfo( )->TimeDetails( ).Cache( ) };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
		std::filesystem::path rotationReadyFile { filePath };
		rotationReadyFile.make_preferred( );
		CacheOriginalPathComponents( filePath );
		std::string rotateFile { OriginalName( ) };
		rotateFile.append( "_01" ).append( OriginalExtension( ) );
		rotationReadyFile.replace_filename( rotateFile );
		if( !std::filesystem::exists( rotationReadyFile ) ) {
			CloseFile( );
			std::filesystem::rename( fileOptions.filePath, rotationReadyFile );
			fileOptions.filePath = std::move( rotationReadyFile );
			OpenFile( replaceIfExists );
		} else {
			RotateFile( );
		}
		SetCurrentFileSize( std::filesystem::file_size( fileOptions.filePath ) );
	}

	RotatingTarget::RotatingTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath, bool replaceIfExists )
		: FileTarget( name, formatPattern, filePath, replaceIfExists ), shouldRotate( true ), m_mode( IntervalMode::file_size )
	{
		auto &cache { MsgInfo( )->TimeDetails( ).Cache( ) };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
		std::filesystem::path rotationReadyFile { filePath };
		rotationReadyFile.make_preferred( );
		CacheOriginalPathComponents( filePath );
		std::string rotateFile { OriginalName( ) };
		rotateFile.append( "_01" ).append( OriginalExtension( ) );
		rotationReadyFile.replace_filename( rotateFile );
		if( !std::filesystem::exists( rotationReadyFile ) ) {
			CloseFile( );
			std::filesystem::rename( fileOptions.filePath, rotationReadyFile );
			fileOptions.filePath = std::move( rotationReadyFile );
			OpenFile( replaceIfExists );
		} else {
			RotateFile( );
		}
		SetCurrentFileSize( std::filesystem::file_size( fileOptions.filePath ) );
	}

	RotatingTarget::~RotatingTarget( )
	{
		CloseFile( );
	}

	void RotatingTarget::WriteToBaseBuffer( bool fmtToBuf )
	{
		TargetBase::WriteToBaseBuffer( fmtToBuf );
	}

	const bool RotatingTarget::isWriteToBuf( )
	{
		return TargetBase::isWriteToBuf( );
	}

	std::string *const RotatingTarget::Buffer( )
	{
		return TargetBase::Buffer( );
	}

	bool RotatingTarget::RenameFile( std::string_view newFileName )
	{
		if( isMTSupportEnabled( ) ) {
			while( !rotateMutex.try_lock( ) ) {
				std::this_thread::sleep_for( std::chrono::nanoseconds( 10 ) );
			}
		}
		try {
			// make copy for old file conversion and cache new values
			std::filesystem::path newFile { fileOptions.filePath };
			newFile.replace_filename( newFileName );
			CacheOriginalPathComponents( newFile );
			RotateFile( );
			if( isMTSupportEnabled( ) ) {
				rotateMutex.unlock( );
			}
			return true;
		}
		catch( const std::exception &e ) {
			std::cerr << "Error In Renaming File:\n";
			std::cerr << e.what( );
			if( isMTSupportEnabled( ) ) {
				rotateMutex.unlock( );
			}
			return false;
		}
	}

	void RotatingTarget::EnableRotation( bool shouldRotate )
	{
		if( isMTSupportEnabled( ) ) {
			while( !rotateMutex.try_lock( ) ) {
				std::this_thread::sleep_for( std::chrono::nanoseconds( 10 ) );
			}
		}
		this->shouldRotate = shouldRotate;
		SetCurrentFileSize( std::filesystem::file_size( fileOptions.filePath ) );
		InitFirstRotation( shouldRotate );
		if( isMTSupportEnabled( ) ) {
			rotateMutex.unlock( );
		}
	}

	void RotatingTarget::SetRotateSettings( RotateSettings settings )
	{
		if( isMTSupportEnabled( ) ) {
			while( !rotateMutex.try_lock( ) ) {
				std::this_thread::sleep_for( std::chrono::nanoseconds( 10 ) );
			}
		}
		fileSizeLimit        = settings.fileSizeLimit;
		maxNumberOfFiles     = settings.maxNumberOfFiles;
		dayModeSettingHour   = settings.dayModeSettingHour;
		dayModeSettingMinute = settings.dayModeSettingMinute;
		monthModeSetting     = settings.monthModeSetting;
		weekModeSetting      = settings.weekModeSetting;
		if( isMTSupportEnabled( ) ) {
			rotateMutex.unlock( );
		}
	}

	void RotatingTarget::RotateFile( )
	{
		if( isMTSupportEnabled( ) ) {
			while( !rotateMutex.try_lock( ) ) {
				std::this_thread::sleep_for( std::chrono::nanoseconds( 10 ) );
			}
		}

		if( !shouldRotate ) return;  // If this was explicitly called but rotatation is disabled, just return

		CloseFile( );
		bool rotateSuccessful { false };
		// make local copy of orginal path since we need to modify the path
		auto newFilePath { OriginalPath( ) };

		for( size_t fileNumber { 1 }; fileNumber <= maxNumberOfFiles; ++fileNumber ) {
			std::string newFile { OriginalName( ) };  // effectively reset each loop
			                                          // iteration
			newFile.append( "_" ).append( SERENITY_LUTS::numberStr[ fileNumber ] ).append( OriginalExtension( ) );
			newFilePath.replace_filename( newFile );

			if( !std::filesystem::exists( newFilePath ) ) {
				fileOptions.filePath = std::move( newFilePath );
				if( OpenFile( true ) ) {
					rotateSuccessful = true;
					break;
				}
			}
		}

		if( !rotateSuccessful ) {
			auto                            logDirectory { std::filesystem::directory_iterator( OriginalDirectory( ) ) };
			std::filesystem::file_time_type oldestWriteTime { std::chrono::file_clock::now( ) };
			std::string                     fileNameToFind { OriginalName( ) };
			std::filesystem::path           fileToReplace;
			for( auto &file : logDirectory ) {
				if( file.is_regular_file( ) ) {
					if( file.path( ).filename( ).string( ).find( fileNameToFind ) != std::string::npos ) {
						if( file.last_write_time( ) < oldestWriteTime ) {
							oldestWriteTime = file.last_write_time( );
							fileToReplace   = file.path( );
						}
					}
				}
			}
			std::filesystem::remove( fileToReplace );
			auto previousFile { fileOptions.filePath.filename( ).string( ) };

			if( !fileToReplace.empty( ) ) {
				fileOptions.filePath = std::move( fileToReplace );
			} else {
				std::cerr << std::vformat( "Warning: Unable To Locate Oldest File With Base Name \"{}\". Opening And Truncating "
				                           "Previous File, \"{}\"\n",
				                           std::make_format_args( OriginalName( ), previousFile ) );
			}

			if( !FileTarget::OpenFile( true ) ) {
				if( fileToReplace != previousFile ) {
					std::cerr << std::vformat( "Error: Unable To Finish Rotating From File \"{}\" To File \"{}\"\n",
					                           std::make_format_args( previousFile, fileOptions.filePath.filename( ).string( ) ) );
				} else {
					std::cerr << std::vformat( "Error: Unable To Open And Truncate File \"{}\"\n", std::make_format_args( previousFile ) );
				}
			}
		}

		if( isMTSupportEnabled( ) ) {
			rotateMutex.unlock( );
		}
	}

	void RotatingTarget::PrintMessage( std::string_view formatted )
	{
		if( isMTSupportEnabled( ) ) {
			while( !rotateMutex.try_lock( ) ) {
				std::this_thread::sleep_for( std::chrono::nanoseconds( 10 ) );
			}
		}

		auto flushThread { flushWorker.flushThreadEnabled.load( std::memory_order::relaxed ) };
		if( flushThread ) {
			while( !flushWorker.readWriteMutex.try_lock( ) ) {
				std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
			}
		}
		if( ShouldRotate( ) ) {
			RotateFile( );
			SetCurrentFileSize( 0 );
		}
		fileHandle.rdbuf( )->sputn( formatted.data( ), formatted.size( ) );
		SetCurrentFileSize( FileSize( ) + formatted.size( ) );
		if( flushThread ) {
			flushWorker.readWriteMutex.unlock( );
		}
		if( isMTSupportEnabled( ) ) {
			rotateMutex.unlock( );
		}
	}

	bool RotatingTarget::ShouldRotate( )
	{
		if( isMTSupportEnabled( ) ) {
			while( !rotateMutex.try_lock( ) ) {
				std::this_thread::sleep_for( std::chrono::nanoseconds( 10 ) );
			}
		}
		if( !shouldRotate ) return false;
		// If previous file was empty - no need to rotate
		if( FileSize( ) == 0 ) return false;

		using mode  = RotateSettings::IntervalMode;
		auto &cache = MsgInfo( )->TimeDetails( ).Cache( );

		switch( RotationMode( ) ) {
			case mode::file_size:
			{
				auto currentSize { FileSize( ) + MsgInfo( )->MessageSize( ) };
				if( isMTSupportEnabled( ) ) {
					rotateMutex.unlock( );
				}
				return ( currentSize >= fileSizeLimit );
			} break;
			case mode::hourly:
			{
				if( currentHour != cache.tm_hour ) {
					currentHour = cache.tm_hour;
					InitFirstRotation( true );
				}
				if( IsIntervalRotationEnabled( ) ) {
					InitFirstRotation( false );
					if( isMTSupportEnabled( ) ) {
						rotateMutex.unlock( );
					}
					return true;
				}
			} break;
			case mode::daily:
				if( currentDay != cache.tm_mday ) {
					currentDay = cache.tm_mday;
					InitFirstRotation( true );
				}
				if( ( cache.tm_hour == dayModeSettingHour ) && ( cache.tm_min && dayModeSettingMinute ) ) {
					if( IsIntervalRotationEnabled( ) ) {
						InitFirstRotation( false );
						if( isMTSupportEnabled( ) ) {
							rotateMutex.unlock( );
						}
						return true;
					}
				}
				break;
			case mode::weekly:
				if( currentWeekday != cache.tm_wday ) {
					currentWeekday = cache.tm_wday;
					InitFirstRotation( true );
				}
				if( currentWeekday == weekModeSetting ) {
					if( ( cache.tm_hour == dayModeSettingHour ) && ( cache.tm_min && dayModeSettingMinute ) ) {
						if( IsIntervalRotationEnabled( ) ) {
							InitFirstRotation( false );
							if( isMTSupportEnabled( ) ) {
								rotateMutex.unlock( );
							}
							return true;
						}
					}
				}
				break;
			case mode::monthly:
			{
				if( currentDay != cache.tm_mday ) {
					currentDay = cache.tm_mday;
					InitFirstRotation( true );
				}
				int numberOfDays { SERENITY_LUTS::daysPerMonth.at( cache.tm_mon ) };
				int rotationDay { monthModeSetting };
				// clang-format off
						/********************************************** Simple Summary *****************************************************************
						* - If the number of days per month is less than the setting ( for example, if the setting is 31 but the month has 30 days):   
						*   - The rotation day will be the 30th instead (so as not to skip a month by accident)                                        
						*	 - If the current month is February, checks to see if the current year is a leap year.                                     
						*     - If the year is a leap year, then sets the rotation day to 29 instead of 28                                             
						* - Otherwise, this snippet uses the monthModeSetting value                                                                     
						*******************************************************************************************************************************/
						// clang-format off
					if( numberOfDays < monthModeSetting ) {
					rotationDay = numberOfDays;
					if( cache.tm_mon == serenity::SERENITY_LUTS::FEBRUARY ) {
						if( MsgInfo( )->TimeDetails( ).isLeapYear( ) ) {
							rotationDay = 29;
						}
					}
				}
				if( currentDay == rotationDay ) {
					if( ( cache.tm_hour == dayModeSettingHour ) && ( cache.tm_min && dayModeSettingMinute ) ) {
						if( IsIntervalRotationEnabled( ) ) {
							InitFirstRotation( false );		
							if( isMTSupportEnabled( ) ) {
								rotateMutex.unlock( );
							}
							return true;
						}
					}
				}
			} break;
		}
		// If we got here, then rotation shouldn't occur since we return true in the case statements directly if we should rotate the file
		if( isMTSupportEnabled( ) ) {
			rotateMutex.unlock( );
		}
		return false;
	}

	void RotatingTarget::SetRotationSetting( IntervalMode mode, size_t setting, size_t secondSetting )
	{
		if( isMTSupportEnabled( ) ) {
			while( !rotateMutex.try_lock( ) ) {
				std::this_thread::sleep_for( std::chrono::nanoseconds( 10 ) );
			}
		}

		using mType = RotateSettings::IntervalMode;
		switch( m_mode ) {
			case mType::file_size: fileSizeLimit = setting; break;
			// Including hourly just to avoid compiler whining. Since we check current hour value with internal cached
			// hour value, there's no need to set anything or check anything here.
			case mType::hourly: return; break;
			case mType::daily:
			{
				if( ( setting > 23 ) || ( setting < 0 ) ) {
					setting = 0;
					std::cerr << "Hour setting passed in for IntervalMode::daily is out of bounds. \nValue expected is "
								 "between 0-23 where 0 is 12AM and 23 is 11PM. \nValue passed in: "
							  << setting << " \n";
				}
				if( ( secondSetting > 59 ) || ( secondSetting < 0 ) ) {
					secondSetting = 0;
					std::cerr << "Minute setting passed in for IntervalMode::daily is out of bounds. \nValue expected is "
								 "between 0-59. \nValue passed in: "
							  << secondSetting << " \n";
				}
				dayModeSettingHour   = setting;
				dayModeSettingMinute = secondSetting;

			} break;
			case mType::weekly:
			{
				if( ( setting > 6 ) || ( setting < 0 ) ) {
					setting = 0;
					std::cerr << "Weekday setting passed in for IntervalMode::weekly is out of bounds. \nValue expected is "
								 "between 0-6 where 0 is Sunday and 6 is Saturday. \nValue passed in: "
							  << setting << " \n";
				}
				weekModeSetting = setting;
			} break;
			case mType::monthly:
			{
				if( ( setting > 31 ) || ( setting < 1 ) ) {
					setting = 1;
					std::cerr << "Day setting passed in for IntervalMode::monthly is out of bounds. \nValue expected is "
								 "between 1-31 where 1 is the first day of the month and 31 is the max value of possible "
								 "days in a month. \nValue passed in: "
							  << setting << " \n";
				}
				monthModeSetting = setting;
			} break;
		}
		if( isMTSupportEnabled( ) ) {
			rotateMutex.unlock( );
		}
	}

	void RotatingTarget::SetRotationMode( IntervalMode mode )
	{
		if( isMTSupportEnabled( ) ) {
			while( !rotateMutex.try_lock( ) ) {
				std::this_thread::sleep_for( std::chrono::nanoseconds( 10 ) );
			}
		} 
		m_mode = mode;
		if( isMTSupportEnabled( ) ) {
			rotateMutex.unlock( );
		}
	}



	const RotateSettings::IntervalMode RotatingTarget::RotationMode( )
	{
		std::scoped_lock<std::mutex> lock(rotateMutex);
		return m_mode;
	}
}  // namespace serenity::experimental::targets