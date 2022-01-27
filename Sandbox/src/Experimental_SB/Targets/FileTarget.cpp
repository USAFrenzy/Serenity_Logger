#include "FileTarget.h"

//#include <serenity/Utilities/Utilities.h>
#include <serenity/Color/Color.h>

#include <iostream>  // specifically for std::cerr

namespace serenity::expiremental::targets
{
	FileTarget::FileTarget( ) : TargetBase( "File_Logger" ), policy( Policy( ) )
	{
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
				std::filesystem::create_directories( logDirPath );
				OpenFile( true );
			} else {
				OpenFile( true );
			}
		}
		catch( const std::exception &e ) {
			std::cerr << e.what( ) << "\n";
			CloseFile( );
		}
	}

	FileTarget::FileTarget( std::string_view fileName, bool replaceIfExists ) : TargetBase( "File_Logger" ), policy( Policy( ) )
	{
		fileOptions.fileBuffer.reserve( fileOptions.bufferSize );
		std::filesystem::path fullFilePath = std::filesystem::current_path( );
		const auto            logDir { "Logs" };
		const auto            logDirPath { fullFilePath };
		fullFilePath /= logDir;
		fullFilePath /= fileName;
		fileOptions.filePath = fullFilePath.make_preferred( ).string( );
		logLevel             = LoggerLevel::trace;
		try {
			if( !std::filesystem::exists( logDirPath ) ) {
				std::filesystem::create_directories( logDirPath );
				OpenFile( true );
			} else {
				OpenFile( true );
			}
		}
		catch( const std::exception &e ) {
			std::cerr << e.what( ) << "\n";
			CloseFile( );
		}
	}

	FileTarget::FileTarget( std::string_view name, std::string_view fPath, bool replaceIfExists )
	  : TargetBase( name ), policy( Policy( ) )
	{
		fileOptions.fileBuffer.reserve( fileOptions.bufferSize );
		fileOptions.filePath = fPath;
		fileOptions.filePath.make_preferred( );
		logLevel = LoggerLevel::trace;
		try {
			if( !std::filesystem::exists( fileOptions.filePath ) ) {
				auto dir { fileOptions.filePath };
				dir.remove_filename( );
				std::filesystem::create_directories( dir );
				OpenFile( replaceIfExists );
			} else {
				OpenFile( replaceIfExists );
			}
		}
		catch( const std::exception &e ) {
			std::cerr << e.what( ) << "\n";
			CloseFile( );
		}
	}

	FileTarget::FileTarget( std::string_view name, std::string_view formatPattern, std::string_view fPath, bool replaceIfExists )
	  : TargetBase( name, formatPattern ), policy( Policy( ) )
	{
		fileOptions.fileBuffer.reserve( fileOptions.bufferSize );
		fileOptions.filePath = fPath;
		fileOptions.filePath.make_preferred( );
		logLevel = LoggerLevel::trace;
		try {
			if( !std::filesystem::exists( fileOptions.filePath ) ) {
				auto dir { fileOptions.filePath };
				dir.remove_filename( );
				std::filesystem::create_directories( dir );
				OpenFile( replaceIfExists );
			} else {
				OpenFile( replaceIfExists );
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

	const std::string FileTarget::FilePath( )
	{
		return fileOptions.filePath.string( );
	}
	const std::string FileTarget::FileName( )
	{
		return fileOptions.filePath.filename( ).string( );
	}

	bool FileTarget::OpenFile( bool truncate )
	{
		try {
#ifndef WINDOWS_PLATFORM
			fileHandle.rdbuf( )->pubsetbuf( fileBuffer.data( ), bufferSize );
#endif  // !WINDOWS_PLATFORM
			if( !truncate ) {
				fileHandle.open( fileOptions.filePath.string( ), std::ios_base::binary | std::ios_base::app );
			} else {
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
			std::filesystem::rename( fileOptions.filePath, newFile );
			fileOptions.filePath = std::move( newFile );
			OpenFile( );
			return true;
		}
		catch( const std::exception &e ) {
			std::cerr << "Error In Renaming File:\n";
			std::cerr << e.what( );
			return false;
		}
	}

	void FileTarget::PrintMessage( std::string_view formatted )
	{
		// naive guard from always trying to take a lock regardless of whether the background flush thread is running or not (using
		// manual lock due to scoping here)
		auto flushThread { flushWorker.flushThreadEnabled.load( std::memory_order::relaxed ) };
		if( flushThread ) {
			while( !flushWorker.readWriteMutex.try_lock( ) ) {
				std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
			}
		}
		fileHandle.rdbuf( )->sputn( formatted.data( ), formatted.size( ) );
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

}  // namespace serenity::expiremental::targets
