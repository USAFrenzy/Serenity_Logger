#include "FileTarget.h"

#include <serenity/Utilities/Utilities.h>
#include <serenity/Color/Color.h>

#include <iostream>  // specifically for std::cerr

namespace serenity::expiremental::targets
{
	FileTarget::FileTarget( ) : TargetBase( "File Logger" ), policy( Policy( ) )
	{
		WriteToBaseBuffer( false );
		fileOptions.fileBuffer.reserve( fileOptions.bufferSize );

		std::filesystem::path fullFilePath = std::filesystem::current_path( );
		const auto            logDir { "Logs" };
		const auto            logDirPath { fullFilePath };
		fullFilePath /= logDir;
		fullFilePath /= "Generic_Log.txt";
		fileOptions.filePath = fullFilePath.make_preferred( ).string( );
		logLevel             = LoggerLevel::trace;

		try {
			if( !std::filesystem::exists( logDirPath ) ) {
				file_utils::CreateDir( logDirPath );
				OpenFile( true );
			}
			else {
				OpenFile( true );
			}
		}
		catch( const std::exception &e ) {
			std::cerr << e.what( ) << "\n";
			CloseFile( );
		}
	}

	FileTarget::FileTarget( std::string_view fPath, bool replaceIfExists ) : TargetBase( "File Logger" ), policy( Policy( ) )
	{
		WriteToBaseBuffer( false );
		fileOptions.fileBuffer.reserve( fileOptions.bufferSize );

		std::filesystem::path file { fileOptions.filePath };
		fileOptions.filePath = file.relative_path( ).make_preferred( );
		logLevel             = LoggerLevel::trace;
		try {
			if( file_utils::ValidateFileName( fileOptions.filePath.filename( ).string( ) ) ) {
				if( !std::filesystem::exists( fileOptions.filePath ) ) {
					file._Remove_filename_and_separator( );
					file_utils::CreateDir( file );
					OpenFile( );
				}
				else {
					OpenFile( );
				}
			}
			else {
				std::cerr << se_colors::Tag::Red( "Error In File Name" );
			}
		}
		catch( const std::exception &e ) {
			std::cerr << e.what( ) << "\n";
			CloseFile( );
		}
	}

	FileTarget::~FileTarget( )
	{
		CloseFile( );
	}

	std::string FileTarget::FilePath( )
	{
		return fileOptions.filePath.string( );
	}

	bool FileTarget::OpenFile( bool truncate )
	{
		try {
#ifndef WINDOWS_PLATFORM
			fileHandle.rdbuf( )->pubsetbuf( fileBuffer.data( ), bufferSize );
#endif  // !WINDOWS_PLATFORM
			if( !truncate ) {
				fileHandle.open( fileOptions.filePath.string( ), std::ios_base::binary | std::ios_base::app );
			}
			else {
				fileHandle.open( fileOptions.filePath.string( ), std::ios_base::binary | std::ios_base::trunc );
			}
#ifdef WINDOWS_PLATFORM
			fileHandle.rdbuf( )->pubsetbuf( fileOptions.fileBuffer.data( ), fileOptions.bufferSize );
#endif  // WINDOWS_PLATFORM
		}
		catch( const std::exception &e ) {
			std::cerr << "Error In Opening File:\n";
			std::cerr << e.what( ) << "\n";
			return false;
		}
		return true;
	}

	void FileTarget::EraseContents( )
	{
		CloseFile( );
		OpenFile( true );
	}

	bool FileTarget::CloseFile( )
	{
		try {
			if( flushWorker.flushThread.joinable( ) ) {
				flushWorker.cleanUpThreads.store( true );
				flushWorker.cleanUpThreads.notify_one( );
				flushWorker.flushThread.join( );
			}
			Flush( );
			fileHandle.close( );
		}
		catch( const std::exception &e ) {
			std::cerr << "Error In Closing File:\n";
			std::cerr << e.what( ) << "\n";
			return false;
		}
		return true;
	}

	bool FileTarget::RenameFile( std::string_view newFileName )
	{
		try {
			// make copy for old file conversion
			std::filesystem::path newFile { fileOptions.filePath };
			newFile.replace_filename( newFileName );
			CloseFile( );
			file_utils::RenameFile( fileOptions.filePath, newFile );
			fileOptions.filePath = std::move( newFile );

			if( fileOptions.rotateFileSettings != nullptr ) {
				fileOptions.rotateFileSettings->OriginalFileName() = fileOptions.filePath;
				RenameFileForRotation( );
			}
			OpenFile( );
			return true;
		}
		catch( const std::exception &e ) {
			std::cerr << se_colors::Tag::Red( e.what( ) );
			return false;
		}
	}

	void FileTarget::PrintMessage( std::string_view formatted )
	{
		// naive guard from always trying to take a lock regardless of
		// whether the background flush thread is running or not (using manual lock due to scoping here)
		auto flushThread { flushWorker.flushThreadEnabled.load( std::memory_order::relaxed ) };
		if( flushThread ) {
			while( !flushWorker.readWriteMutex.try_lock( ) ) {
				std::this_thread::sleep_for( 10ms );
			}
		}
		if( fileOptions.rotateFile ) {
			if( fileOptions.rotateFileSettings != nullptr ) {
				if( std::filesystem::file_size( fileOptions.filePath ) >= fileOptions.rotateFileSettings->fileSize ) {
					RotateFileOnSize( );
				}
			}
		}

		fileHandle.rdbuf( )->sputn( formatted.data( ), formatted.size( ) );
		if( policy.SubSetting( ) == PeriodicOptions::memUsage ) fileOptions.fileBufOccupied += formatted.size( );
		if( flushThread ) {
			flushWorker.readWriteMutex.unlock( );
		}
	}

	void FileTarget::Flush( )
	{
		// If formatted message wasn't written to file and instead was written to the buffer, write to file now
		if( Buffer( )->size( ) != 0 ) {
			fileHandle.rdbuf( )->sputn( Buffer( )->data( ), Buffer( )->size( ) );
			Buffer( )->clear( );
		}
		fileHandle.flush( );
	}

	void FileTarget::PolicyFlushOn( )
	{
		if( policy.PrimarySetting( ) == Flush::never ) return;
		if( policy.PrimarySetting( ) == Flush::always ) {
			Flush( );
			return;
		}
		switch( policy.SubSetting( ) ) {
			case PeriodicOptions::memUsage:
			{
				auto shouldFlush { ( Buffer( )->size( ) >= policy.SecondarySettings( ).memoryFlushOn ) ||
								   ( fileOptions.fileBufOccupied >= policy.SecondarySettings( ).memoryFlushOn ) };
				if( !shouldFlush ) return;
				Flush( );
				fileOptions.fileBufOccupied = 0;
			} break;
			case PeriodicOptions::timeBased:
			{
				if( flushWorker.flushThreadEnabled.load( std::memory_order::relaxed ) ) return;
				auto &p_policy { policy };
				// lambda that starts a background thread to flush on time interval given
				auto periodic_flush = [ this, p_policy ]( )
				{
					using namespace std::chrono;
					using namespace std::chrono_literals;
					static milliseconds lastTimePoint { };

					while( !flushWorker.cleanUpThreads.load( std::memory_order::relaxed ) ) {
						auto now     = duration_cast<milliseconds>( system_clock::now( ).time_since_epoch( ) );
						auto elapsed = duration_cast<milliseconds>( now - lastTimePoint );
						auto shouldFlush { elapsed >= policy.SecondarySettings( ).flushEvery };
						if( shouldFlush ) {
							std::scoped_lock<std::mutex> lock( flushWorker.readWriteMutex );
							Flush( );
						}
						lastTimePoint = now;
					}
				};  // periodic_flush

				if( !flushWorker.flushThreadEnabled.load( std::memory_order::relaxed ) ) {
					flushWorker.flushThread = std::thread( periodic_flush );
					flushWorker.flushThreadEnabled.store( true );
				}
			} break;  // time based bounds
			case PeriodicOptions::logLevelBased:
			{
				if( MsgInfo( )->MsgLevel( ) < policy.SecondarySettings( ).flushOn ) return;
				Flush( );
			} break;
			default: break;  // Don't bother with undef field
		}                    // Sub Option Check
	}                        // PolicyFlushOn( ) Function

	// ------------------------------------------------------------- WIP -------------------------------------------------------------
	void FileTarget::ShouldRotateFile( bool shouldRotate )
	{
		fileOptions.rotateFile = shouldRotate;
	}

	void FileTarget::RenameFileForRotation() {
		auto       extension { fileOptions.filePath.extension( ).string( ) };
		const auto oldFile { fileOptions.filePath };
		// need to make copy to avoid changing old file so that we can rename it
		auto        rotateFile { fileOptions.filePath };
		std::string fileName { rotateFile.replace_extension( ).string( ) };
		rotateFile.replace_filename( fileName.append( "_" ).append( "01" ).append( extension ) );
		if( fileHandle.is_open( ) ) {
			CloseFile( );
		}
		try {
			std::filesystem::rename( oldFile, rotateFile );
		}
		catch( const std::exception &e ) {
			std::cerr << e.what( );
		}
		fileOptions.filePath = std::move( rotateFile );
		OpenFile( );
	}


	void FileTarget::SetRotateSettings( RotateSettings settings )
	{
		fileOptions.rotateFileSettings                   = &settings;
		fileOptions.rotateFileSettings->OriginalFileName( ) = fileOptions.filePath;
		RenameFileForRotation( );
	}

	void FileTarget::RotateFileOnSize( )
	{
		CloseFile( );
		auto                  newFilePath { fileOptions.rotateFileSettings->OriginalFileName( ) };
		std::filesystem::path oldFile { newFilePath.filename( ) };
		auto                  extension { oldFile.extension( ).string( ) };
		// remove extension now that we have a copy of original extension so that we can append file number to name
		oldFile.replace_extension( );
		bool  rotateSuccessful { false };
		auto &numberOfFiles { fileOptions.rotateFileSettings->maxNumberOfFiles };

		for( size_t fileNumber { 1 }; fileNumber <= numberOfFiles; ++fileNumber ) {
			auto newFile { oldFile.string( ).append("_").append( SE_LUTS::numberStr[ fileNumber ] ).append( extension ) };
			newFilePath.replace_filename( std::move( newFile ) );
			newFilePath.make_preferred( );

			if( !std::filesystem::exists( newFilePath ) ) {
				fileOptions.filePath = std::move( newFilePath );
				if( OpenFile( ) ) {
					rotateSuccessful = true;
					break;
				}
			}
		}

		if( !rotateSuccessful ) {
			auto                            logDirPath { newFilePath.remove_filename( ) };
			std::filesystem::file_time_type oldestWriteTime = { };
			std::filesystem::path           fileToReplace   = { };
			bool                            firstIteration { true };
			for( auto &file : std::filesystem::directory_iterator( logDirPath ) ) {
				if( file.is_regular_file( ) ) {
					auto db { file.path( ).filename( ).string( ) };
					if( file.path( ).filename( ).string( ).find( oldFile.string( ) ) != std::string::npos ) {
						if( firstIteration ) {
							firstIteration  = false;
							oldestWriteTime = file.last_write_time( );
							fileToReplace   = file.path( );
						}
						if( file.last_write_time( ) < oldestWriteTime ) {
							oldestWriteTime = file.last_write_time( );
							fileToReplace   = file.path( );
						}
					}
				}
			}
			std::filesystem::remove( fileToReplace );
			fileOptions.filePath = std::move( fileToReplace );
			// Overwriting file, so truncate file when opening
			if( !OpenFile( true ) ) {
				std::cerr << std::vformat("Error: Unable To Finish Rotating File To {}\n", std::make_format_args(fileOptions.filePath.string()));
			}
		}
	}
	// ------------------------------------------------------------- WIP -------------------------------------------------------------

}  // namespace serenity::expiremental::targets
