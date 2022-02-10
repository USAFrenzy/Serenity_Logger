#include <serenity/Targets/RotatingTarget.h>

#include <iostream>

namespace serenity::experimental::targets
{
	using namespace serenity::targets;

	RotatingTarget::RotatingTarget( ) : FileTarget( "Rotating_Log.txt", true ), shouldRotate( true )
	{
		CacheOriginalPathComponents( fileOptions.filePath );
		RenameFileForRotation( );
		SetCurrentFileSize( std::filesystem::file_size( fileOptions.filePath ) );
		SetLoggerName( "Rotating_Logger" );
		auto &cache { MsgInfo( )->TimeDetails( ).Cache( ) };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
	}

	RotatingTarget::RotatingTarget( std::string_view name, std::string_view filePath, bool replaceIfExists )
	  : FileTarget( name, filePath, replaceIfExists ), shouldRotate( true )
	{
		CacheOriginalPathComponents( filePath );
		RenameFileForRotation( );
		SetCurrentFileSize( std::filesystem::file_size( fileOptions.filePath ) );
		SetLoggerName( "Rotating_Logger" );
		auto &cache { MsgInfo( )->TimeDetails( ).Cache( ) };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
	}

	RotatingTarget::RotatingTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath,
									bool replaceIfExists )
	  : FileTarget( name, formatPattern, filePath, replaceIfExists ), shouldRotate( true )
	{
		CacheOriginalPathComponents( filePath );
		RenameFileForRotation( );
		SetCurrentFileSize( std::filesystem::file_size( fileOptions.filePath ) );
		SetLoggerName( "Rotating_Logger" );
		auto &cache { MsgInfo( )->TimeDetails( ).Cache( ) };
		currentHour    = cache.tm_hour;
		currentDay     = cache.tm_mday;
		currentWeekday = cache.tm_wday;
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
		try {
			// make copy for old file conversion
			std::filesystem::path newFile { fileOptions.filePath };
			newFile.replace_filename( newFileName );
			if( fileHandle.is_open( ) ) {
				CloseFile( );
			}
			std::filesystem::rename( fileOptions.filePath, newFile );
			fileOptions.filePath = std::move( newFile );
			CacheOriginalPathComponents( fileOptions.filePath );
			RenameFileForRotation( );
			if( !fileHandle.is_open( ) ) {
				OpenFile( );
			}
			return true;
		}
		catch( const std::exception &e ) {
			std::cerr << "Error In Renaming File:\n";
			std::cerr << e.what( );
			return false;
		}
	}

	void RotatingTarget::EnableRotation( bool shouldRotate )
	{
		this->shouldRotate = shouldRotate;
		SetCurrentFileSize( std::filesystem::file_size( fileOptions.filePath ) );
		InitFirstRotation( shouldRotate );
	}

	void RotatingTarget::RenameFileForRotation( )
	{
		auto       extension { fileOptions.filePath.extension( ).string( ) };
		const auto oldFile { fileOptions.filePath };
		// need to make copy to avoid changing old file so that we can rename it
		auto        rotateFile { fileOptions.filePath };
		std::string fileName { rotateFile.replace_extension( ).string( ) };
		fileName.append( "_" ).append( "01" ).append( extension );
		rotateFile.replace_filename( fileName );
		if( fileHandle.is_open( ) ) {
			CloseFile( );
		}
		// only if the file doesn't exist already should it be renamed,
		// otherwise we just open the already existing file
		if( !std::filesystem::exists( rotateFile ) ) {
			std::filesystem::rename( oldFile, rotateFile );
		}
		fileOptions.filePath = std::move( rotateFile );
		OpenFile( );
	}

	void RotatingTarget::SetRotateSettings( RotateSettings settings )
	{
		fileSizeLimit    = settings.fileSizeLimit;
		maxNumberOfFiles = settings.maxNumberOfFiles;
		dayModeSetting   = settings.dayModeSetting;
		monthModeSetting = settings.monthModeSetting;
		weekModeSetting  = settings.weekModeSetting;
	}

	void RotatingTarget::RotateFile( )
	{
		if( !shouldRotate ) return;

		CloseFile( );
		bool rotateSuccessful { false };
		// make local copies of originals
		auto newFilePath { OriginalPath( ) };
		auto originalFile { OriginalName( ) };
		auto extension { OriginalExtension( ) };
		auto numberOfFiles { maxNumberOfFiles };

		for( size_t fileNumber { 1 }; fileNumber <= numberOfFiles; ++fileNumber ) {
			std::string newFile { originalFile };  // effectively reset each loop
												   // iteration
			newFile.append( "_" ).append( SERENITY_LUTS::numberStr[ fileNumber ] ).append( extension );
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
			std::filesystem::file_time_type oldestWriteTime = { std::chrono::file_clock::now( ) };
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
				std::cerr << std::vformat( "Warning: Unable To Locate Oldest File With Base Name \"{}\". Opening And "
										   "Truncating Previous File, \"{}\"\n",
										   std::make_format_args( originalFile, previousFile ) );
			}

			if( !FileTarget::OpenFile( true ) ) {
				if( fileToReplace != previousFile ) {
					std::cerr
					<< std::vformat( "Error: Unable To Finish Rotating From File \"{}\" To File \"{}\"\n",
									 std::make_format_args( previousFile, fileOptions.filePath.filename( ).string( ) ) );
				} else {
					std::cerr << std::vformat( "Error: Unable To Open And Truncate File \"{}\"\n",
											   std::make_format_args( previousFile ) );
				}
			}
		}
	}

	void RotatingTarget::PrintMessage( std::string_view formatted )
	{
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
	}

	bool RotatingTarget::ShouldRotate( )
	{
		if( !shouldRotate ) return false;
		// If previous file was empty - no need to rotate
		if( FileSize( ) == 0 ) return false;

		using mode  = RotateSettings::IntervalMode;
		auto &cache = MsgInfo( )->TimeDetails( ).Cache( );

		switch( RotationMode( ) ) {
			case mode::file_size:
			{
				auto currentSize { FileSize( ) + MsgInfo( )->MessageSize( ) };
				return (currentSize >= fileSizeLimit);
			} break;
			case mode::hourly:
			{
				if( currentHour != cache.tm_hour ) {
					currentHour = cache.tm_hour;
					InitFirstRotation( true );
				}
				if( IsIntervalRotationEnabled( ) ) {
					InitFirstRotation( false );
					return true;
				}
			} break;
			case mode::daily:
				if( currentDay != cache.tm_mday ) {
					currentDay = cache.tm_mday;
					InitFirstRotation( true );
				}
				if( cache.tm_hour == dayModeSetting ) {
					if( IsIntervalRotationEnabled( ) ) {
						InitFirstRotation( false );
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
					if( IsIntervalRotationEnabled( ) ) {
						InitFirstRotation( false );
						return true;
					}
				}
				break;
			case mode::monthly:
			{
				if( currentDay != cache.tm_mday ) {
					currentDay = cache.tm_mday;
					InitFirstRotation( true );
				}
				if( currentDay == monthModeSetting ) {
					if( IsIntervalRotationEnabled( ) ) {
						InitFirstRotation( false );
						return true;
					}
				}
			} break;
		}
		// If we got here, then rotation shouldn't occur since we return true in the case statements directly if we should
		// rotate the file
		return false;
	}

	void RotatingTarget::SetRotationSetting( IntervalMode mode, size_t setting )
	{
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
				dayModeSetting = setting;
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
	}

	void RotatingTarget::SetRotationMode( IntervalMode mode )
	{
		m_mode = mode;
	}

	const RotateSettings::IntervalMode RotatingTarget::RotationMode( )
	{
		return m_mode;
	}
}  // namespace serenity::experimental::targets