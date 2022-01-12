#include "FileTarget.h"

#include <serenity/Utilities/Utilities.h>
#include <serenity/Color/Color.h>

#include <thread>

namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			using namespace std::chrono_literals;

			FileTarget::FileTarget( ) : TargetBase( "File Logger" ), policy( Policy( ) )
			{
				WriteToBaseBuffer( false );
				bufferSize = 64 * KB;
				fileBuffer.reserve( bufferSize );

				std::filesystem::path fullFilePath = std::filesystem::current_path( );
				auto                  logDir { "Logs" };
				// NOTE: This Appends The Log Dir To The File Path AS WELL AS assigns that path to logDirPath
				// (Reason: Foregoing const-ness here)
				auto logDirPath = fullFilePath /= logDir;
				fullFilePath /= "Generic_Log.txt";
				filePath = std::move( fullFilePath.make_preferred( ).string( ) );
				logLevel = LoggerLevel::trace;
				try {
					if( !std::filesystem::exists( this->filePath ) ) {
						file_utils::CreateDir( logDir );
						OpenFile( true );
					}
					else {
						OpenFile( true );
					}
				}
				catch( const std::exception &e ) {
					CloseFile( );
					printf( "%s\n", se_colors::Tag::Red( "Unable To Create Default Directory" ).c_str( ) );
				}
			}

			FileTarget::FileTarget( std::string_view filePath, bool replaceIfExists )
			  : TargetBase( "File Logger" ), policy( Policy( ) )
			{
				WriteToBaseBuffer( false );
				bufferSize = 64 * KB;
				fileBuffer.reserve( bufferSize );

				std::filesystem::path file { filePath };
				this->filePath = std::move( file.relative_path( ).make_preferred( ) );
				logLevel       = LoggerLevel::trace;

				try {
					if( file_utils::ValidateFileName( this->filePath.filename( ).string( ) ) ) {
						if( !std::filesystem::exists( this->filePath ) ) {
							// TODO: Test the code below to ensure it works as intended
							std::filesystem::path dirPath = filePath;
							dirPath.replace_filename( "" );
							file_utils::CreateDir( dirPath );
							OpenFile( );
						}
						else {
							OpenFile( );
						}
					}
					else {
						printf( "%s\n", se_colors::Tag::Red( "Error In File Name" ).c_str( ) );
					}
				}
				catch( const std::exception &e ) {
					CloseFile( );
					printf( "%s\n", se_colors::Tag::Red( "Unable To Create Directory From Path" ).c_str( ) );
				}
			}

			FileTarget::~FileTarget( )
			{
				CloseFile( );
			}

			std::string FileTarget::FilePath( )
			{
				return filePath.string( );
			}

			// Apparently the reason why the buffer wasn't being used is due to different implementations of how to call pubsetbuf()
			// (whether before or after opening a file...)
			// https://stackoverflow.com/questions/59160807/rdbuf-pubsetbuf-using-a-bidirectional-fstream-is-applied-only-to-writes
			// In lieu of this, reverting back to std::ofstream from FILE*, however, NEED a better way to do this that's less clunky..
			bool FileTarget::OpenFile( bool truncate )
			{
				try {
					fileHandle.rdbuf( )->pubsetbuf( fileBuffer.data( ), bufferSize );
					if( !truncate ) {
						fileHandle.open( filePath.string( ), std::ios_base::binary | std::ios_base::app );
					}
					else {
						fileHandle.open( filePath.string( ), std::ios_base::binary | std::ios_base::trunc );
					}
					fileHandle.rdbuf( )->pubsetbuf( fileBuffer.data( ), bufferSize );
				}
				catch( const std::exception &e ) {
					printf( "%s\n", se_colors::Tag::Red( "Error In Opening File:" ).c_str( ) );
					printf( "%s\n", se_colors::Tag::Red( e.what( ) ).c_str( ) );
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
					if( flushThread.joinable( ) ) {
						cleanUpThreads.store( true );
						cleanUpThreads.notify_one( );
						flushThread.join( );
					}
					fileHandle.close( );
				}
				catch( const std::exception &e ) {
					printf( "%s\n", se_colors::Tag::Red( "Error In Closing File:" ).c_str( ) );
					printf( "%s\n", se_colors::Tag::Red( e.what( ) ).c_str( ) );
					return false;
				}
				return true;
			}

			// TODO: Check To See This Works (Not Tested)
			bool FileTarget::RenameFile( std::string_view newFileName )
			{
				std::filesystem::path newFile { filePath };
				newFile.replace_filename( newFileName );
				return file_utils::RenameFile( filePath, newFile.filename( ) );
			}

			void FileTarget::PrintMessage( std::string_view formatted )
			{
				// naive guard from always trying to take a lock regardless of
				// whether the background flush thread is running or not
				auto flushThread { flushThreadEnabled.load( std::memory_order::relaxed ) };
				if( flushThread ) {
					while( !readWriteMutex.try_lock( ) ) {
						std::this_thread::sleep_for( 10ms );
					}
				}
				fileHandle.rdbuf( )->sputn( formatted.data( ), formatted.size( ) );
				if( policy.SubSetting( ) == PeriodicOptions::memUsage ) fileBufOccupied += formatted.size( );
				if( flushThread ) {
					readWriteMutex.unlock( );
				}
			}

			void FileTarget::Flush( )
			{
				// If formatted message wasn't written to file and instead was written to the buffer, write to file now
				if( ( isWriteToBuf( ) ) && ( Buffer( )->size( ) != 0 ) ) {
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
										   ( fileBufOccupied >= policy.SecondarySettings( ).memoryFlushOn ) };
						if( !shouldFlush ) return;
						Flush( );
						fileBufOccupied = 0;
					} break;
					case PeriodicOptions::timeBased:
					{
						if( flushThread.joinable( ) ) return;
						auto &p_policy { policy };
						// lambda that starts a background thread to flush on time interval given
						auto periodic_flush = [ this, p_policy ]( )
						{
							using namespace std::chrono;
							static milliseconds lastTimePoint { };

							while( !cleanUpThreads.load( std::memory_order::relaxed ) ) {
								auto now     = duration_cast<milliseconds>( system_clock::now( ).time_since_epoch( ) );
								auto elapsed = duration_cast<milliseconds>( now - lastTimePoint );
								auto shouldFlush { elapsed >= policy.SecondarySettings( ).flushEvery };
								// naive guard from always trying to take a lock regardless of whether enough time has passed that
								// Flush() should even be called
								if( shouldFlush ) {
									while( !readWriteMutex.try_lock( ) ) {
										std::this_thread::sleep_for( 10ms );
									}
									Flush( );
									readWriteMutex.unlock( );
								}
								lastTimePoint = now;
							}
						};  // periodic_flush

						if( !flushThread.joinable( ) ) {
							flushThread = std::thread( periodic_flush );
							flushThreadEnabled.store( true );
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

		}  // namespace targets
	}      // namespace expiremental
}  // namespace serenity
