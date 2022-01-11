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

			bool FileTarget::OpenFile( bool truncate )
			{
				try {
					if( !truncate ) {
						fileHandle = std::fopen( filePath.string( ).c_str( ), "ab" );
						std::setvbuf( fileHandle, fileBuffer.data( ), _IOFBF, bufferSize );
					}
					else {
						fileHandle = std::fopen( filePath.string( ).c_str( ), "wb" );
						std::setvbuf( fileHandle, fileBuffer.data( ), _IOFBF, bufferSize );
					}
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
					ableToFlush.exchange( false );
					ableToFlush.notify_one( );
					cleanUpThreads.exchange( true );
					cleanUpThreads.notify_one( );
					Flush( );
					if( flushThread.joinable( ) ) {
						flushThread.join( );
					}
					std::fclose( fileHandle );
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
				while( !base_mutex.try_lock( ) ) {
					std::this_thread::sleep_for( 10ms );
				}
				std::fwrite( formatted.data( ), sizeof( char ), formatted.size( ), fileHandle );
				base_mutex.unlock( );
			}

			void FileTarget::Flush( )
			{
				// Since we didn't write to file and instead wrote to the buffer, write to file now
				if( ( isWriteToBuf( ) ) && ( Buffer( )->size( ) != 0 ) ) {
					std::fwrite( Buffer( )->data( ), sizeof( char ), Buffer( )->size( ), fileHandle );
					Buffer( )->clear( );
				}
				std::fflush( fileHandle );
			}

			void FileTarget::PolicyFlushOn(  )
			{
				if( policy.PrimarySetting( ) == Flush::never ) return;
				if( policy.PrimarySetting( ) == Flush::always ) {
					Flush( );
					return;
				}
				switch( policy.SubSetting( ) ) {
					case PeriodicOptions::memUsage:
					{
						// Check that the message won't cause an allocation if larger than BUFFER_SIZE
						if( Buffer( )->size( ) >= policy.SecondarySettings( ).memoryFlushOn ) {
							Flush( );
						}
					} break;
					case PeriodicOptions::timeBased:
					{
						if( flushThread.joinable() ) return;
						// lambda that starts a background thread to flush on time interval given
						auto &p_policy { policy };
						auto periodic_flush = [ this, p_policy ]( )
						{
							using namespace std::chrono;
							while( !cleanUpThreads.load( ) ) {
								while( !ableToFlush.load( ) ) {
									std::this_thread::sleep_for( 100ms );
									if( ableToFlush.load( ) ) break;
								}

								while( !base_mutex.try_lock( ) ) {
									std::this_thread::sleep_for( 10ms );
								}
								static  milliseconds lastTimePoint { };
								auto                now     = duration_cast<milliseconds>( system_clock::now( ).time_since_epoch( ) );
								auto                elapsed = duration_cast<milliseconds>( now - lastTimePoint );

								if( elapsed >= policy.SecondarySettings( ).flushEvery ) {
									Flush( );
									lastTimePoint = now;
								}
								base_mutex.unlock( );
							}
						};  // periodic_flush

						if( !flushThread.joinable() ) {
							flushThread = std::thread( periodic_flush );
						}
					} break;  // time based bounds
					case PeriodicOptions::logLevelBased:
					{
						if( ( policy.SecondarySettings( ).flushOn > MsgInfo( )->MsgLevel( ) ) ) return;
						return;
						Flush( );
					} break;
					default: break;  // Don't bother with undef field
				}                    // Sub Option Check
			}                        // PolicyFlushOn( ) Function

		}  // namespace targets
	}      // namespace expiremental
}  // namespace serenity
