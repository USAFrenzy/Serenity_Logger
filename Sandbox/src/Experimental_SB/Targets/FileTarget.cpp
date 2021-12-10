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

			FileTarget::FileTarget( ) : TargetBase( "File Logger" )
			{
				WriteToBaseBuffer( );
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

			FileTarget::FileTarget( std::string_view filePath, bool replaceIfExists ) : TargetBase( "File Logger" )
			{
				WriteToBaseBuffer( );

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
						fileHandle.open( filePath, std::ios_base::app );
					}
					else {
						fileHandle.open( filePath, std::ios_base::trunc );
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

			void FileTarget::PrintMessage( std::string &buffer )
			{
				auto ignore = std::async( std::launch::async, &FileTarget::Write, this, std::forward<std::string &>( buffer ) );
			}

			void FileTarget::Write( std::string buffer )
			{
				static auto policy = Policy( );
				ableToFlush.exchange( false );
				ableToFlush.notify_one( );
				while( !readWriteMutex.try_lock_shared( ) ) {
					std::this_thread::sleep_for( 100ms );
				}
				fileHandle.write( buffer.c_str( ), buffer.size( ) );
				Buffer( )->clear( );
				ableToFlush.exchange( true );
				ableToFlush.notify_one( );
				readWriteMutex.unlock_shared( );
			}

			void FileTarget::Flush( )
			{
				std::lock_guard<std::shared_mutex> lock( readWriteMutex );
				if( !fileHandle.is_open( ) ) return;
				if( isWriteToBuf( ) ) {
					if( AsyncFutures( )->size( ) == 0 ) return;
					static std::string futuresResult;
					futuresResult.reserve( AsyncFutures( )->size( ) );
					futuresResult.clear( );
					for( auto &future : *AsyncFutures( ) ) {
						future.wait( );
						auto tmp = future.get( );
						futuresResult.append( std::move( tmp ) );
					}
					AsyncFutures( )->clear( );
					fileHandle.write( futuresResult.c_str( ), futuresResult.size( ) );
				}
				fileHandle.flush( );
			}

			void FileTarget::PolicyFlushOn( Flush_Policy settings )
			{
				switch( settings.GetSettings( ).primaryOption ) {
					case Flush_Policy::Flush::periodically:
					{
						switch( Policy( ).GetSettings( ).subOption ) {
							case Flush_Policy::Periodic_Options::memUsage:
							{
								// Check that the message won't cause an allocation if larger than BUFFER_SIZE
								if( !( Buffer( )->size( ) < settings.GetSettings( ).subSettings.memUsage.load( ) ) ) {
									Flush( );
								}
							} break;
							case Flush_Policy::Periodic_Options::timeBased:
							{
								static bool threadStarted { false };
								if( threadStarted ) return;
								// lambda that starts a background thread to flush on time interval given
								auto periodic_flush = [ this, &settings ]( )
								{
									using namespace std::chrono;
									while( !cleanUpThreads.load( ) ) {
										while( ableToFlush.load( ) ) {
											static milliseconds lastTimePoint;
											auto now     = duration_cast<milliseconds>( system_clock::now( ).time_since_epoch( ) );
											auto elapsed = duration_cast<milliseconds>( now - lastTimePoint );
											if( elapsed >= settings.GetSettings( ).subSettings.timeBased.load( ) ) {
												Flush( );
												lastTimePoint = now;
												while( !ableToFlush.load( ) ) {
													std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
												}
											}
										}
									}
								};  // periodic_flush

								if( !threadStarted ) {
									std::thread t( periodic_flush );
									t.detach( );
									threadStarted = true;
								}
							} break;  // time based bounds
							case Flush_Policy::Periodic_Options::logLevelBased:
							{
								if( ( Policy( ).GetSettings( ).subSettings.logLevelBased.load( ) > MsgInfo( )->MsgLevel( ) ) ) return;
								return;
								Flush( );
							} break;
							default: break;  // Don't bother with undef field
						}                    // Sub Option Check
					} break;                 // Periodic Option Check
					case Flush_Policy::Flush::always:
					{
						Flush( );
					} break;
					case Flush_Policy::Flush::never:
					{
					} break;
				}
			}  // PolicyFlushOn( ) Function

			void FileTarget::EnableAsyncFlush( bool isEnabled )
			{
				ableToFlush.store( isEnabled );
				ableToFlush.notify_one( );
			}

		}  // namespace targets
	}      // namespace expiremental
}  // namespace serenity
