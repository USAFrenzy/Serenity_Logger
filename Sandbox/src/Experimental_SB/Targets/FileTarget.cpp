#include "FileTarget.h"

#include <serenity/Utilities/Utilities.h>
#include <serenity/Color/Color.h>

namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
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
				static Flush_Policy p_policy;
				p_policy = TargetBase::FlushPolicy( );
				std::lock_guard<std::mutex> lock( m_mutex );
				m_futures.push_back( std::async( std::launch::async, &FileTarget::Write, this,
								 std::forward<std::string &>( buffer ),
								 std::forward<Flush_Policy &>( p_policy ) ) );
			}

			void FileTarget::Write( std::string buffer, Flush_Policy policy )
			{
				std::lock_guard<std::mutex> lock( m_mutex );
				if( !policy.ShouldFlush( ) ) {
					fileHandle.write( buffer.c_str( ), buffer.size( ) );
				}
				else {
					FileTarget::PolicyFlushOn( buffer, policy );
				}
			}


			void FileTarget::Flush( )
			{
				std::lock_guard<std::mutex> lock( m_mutex );
				if( isWriteToBuf( ) ) {
					static std::string futuresResult;
					futuresResult.reserve( AsyncFutures( )->size( ) );
					futuresResult.clear( );
					for( auto &future : *AsyncFutures( ) ) {
						future.wait( );
						auto tmp = future.get( );
						futuresResult.append( std::move( tmp ) );
					}
					AsyncFutures( )->clear( );
					for( ; futuresResult.size( ) != 0; ) {
						fileHandle.write( futuresResult.c_str( ), futuresResult.size( ) );
						break;
					}
				}
				fileHandle.flush( );
			}

			void FileTarget::PolicyFlushOn( std::string &buffer, Flush_Policy policy )
			{
				switch( policy.GetFlushSetting( ) ) {
					case Flush_Policy::Flush::periodically:
						{
							switch( policy.GetPeriodicSetting( ) ) {
								case Flush_Policy::Periodic_Options::mem_usage:
									{
										// Check that the message won't cause
										// an allocation if larger than
										// BUFFER_SIZE
										size_t bufferSize = isWriteToBuf( ) ? Buffer( )->size( )
														    : buffer.size( );
										if( !( bufferSize < BUFFER_SIZE ) ) {
											Flush( );
										}
										fileHandle.write( buffer.c_str( ), buffer.size( ) );
									}
									break;
								case Flush_Policy::Periodic_Options::time_based:
									{
										// Still a work in progress. Unsure if this will
										// correctly block this thread from writing if flushing
										// and then resume
										using namespace std::chrono;
										static bool threadStarted { false }, isFlushing { false };
										std::condition_variable cv;

										auto periodic_flush = [ =, &cv, &policy ]( ) {
											while( true ) {
												static seconds lastTimePoint;
												auto now = duration_cast<seconds>(
												  system_clock::now( ).time_since_epoch( ) );
												auto elapsed = duration_cast<milliseconds>(
												  now - lastTimePoint );
												if( elapsed >
												    policy.GetSettings( ).interval ) {
													isFlushing = true;
													cv.notify_one( );
													Flush( );
													lastTimePoint = now;
												}
												isFlushing = false;
												cv.notify_one( );
											}
										};
										std::unique_lock<std::mutex> lock( m_mutex );
										if( !threadStarted ) {
											std::thread t( periodic_flush );
											threadStarted = true;
										}
										cv.wait( lock, []( ) { return isFlushing; } );
										fileHandle.write( buffer.c_str( ), buffer.size( ) );
									}
									break;
								case Flush_Policy::Periodic_Options::undef:
									{
										// Behave As If Flush Setting = Never & Do Nothing
									}
									break;
							}
							break;
						}  // periodic option check
					case Flush_Policy::Flush::always:
						{
							fileHandle.write( buffer.c_str( ), buffer.size( ) );
							// Always Flush
							Flush( );
						}
						break;
				}
			}


		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity
