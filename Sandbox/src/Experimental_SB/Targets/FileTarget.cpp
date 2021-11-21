#include "FileTarget.h"

#include <filesystem>

#include <serenity/Utilities/Utilities.h>
#include <serenity/Color/Color.h>

#define BUFFER_SIZE 4096
#define BUFFER_CHECK ( BUFFER_SIZE/2 + BUFFER_SIZE/3)

namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			FileTarget::FileTarget( ) : TargetBase( "File Logger" )
			{
				buffer.resize( BUFFER_SIZE );
				std::filesystem::path fullFilePath = std::filesystem::current_path( );
				auto                  logDir { "Logs" };
				// NOTE: This Appends The Log Dir To The File Path AS WELL AS assigns that path to logDirPath
				// (Reason: Foregoing const-ness here)
				auto logDirPath = fullFilePath /= logDir;
				fullFilePath /= "Generic_Log.txt";
				filePath = std::move( fullFilePath.make_preferred( ).string( ) );
				logLevel = LoggerLevel::trace;
				if( !std::filesystem::exists( this->filePath ) ) {
					try {
						file_utils::CreateDir( logDir );
						OpenFile( );
					}
					catch( const std::exception &e ) {
						printf( "%s\n",
							( se_colors::Tag::Red( "Unable To Create Default Directory" ) + e.what( ) + "\n" )
							  .c_str( ) );
					}
				}
			}


			FileTarget::FileTarget( std::string_view filePath, bool replaceIfExists ) : TargetBase( "File Logger" )
			{
				buffer.resize( BUFFER_SIZE );
				std::filesystem::path file { filePath };
				this->filePath = std::move( file.relative_path( ).make_preferred( ).string( ) );
				logLevel       = LoggerLevel::trace;

				if( file_utils::ValidateFileName( this->filePath ) ) {
					if( !std::filesystem::exists( this->filePath ) ) {
						try {
							file_utils::CreateDir( this->filePath );
							OpenFile( replaceIfExists );
						}
						catch( const std::exception &e ) {
							printf( "%s\n", se_colors::Tag::Red( "Unable To Create Directory From File "
											     "Path "
											     "Given" )
									  .c_str( ) );
						}
					}
				}
				else {
					printf( "%s\n", se_colors::Tag::Red( "Error With File Name - Check That File Name Does Not "
									     "Contain Invalid Characters" )
							  .c_str( ) );
				}
			}

			FileTarget::~FileTarget( )
			{
				CloseFile( );
			}
			std::string FileTarget::FilePath( )
			{
				return filePath;
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
					printf( "%s\n", se_colors::Tag::Red( "Error In Opening File:\n" ).c_str( ) );
					printf( "%s %s %s\n", se_colors::basic_colors::foreground::red, e.what( ),
						se_colors::formats::reset );
					return false;
				}
				return true;
			}
			bool FileTarget::EraseContents( )
			{
				try {
					CloseFile( );
					OpenFile( true );
				}
				catch( const std::exception &e ) {
					printf( "%s\n",
						( se_colors::Tag::Red( "Error In Erasing File Contents:\n" ) + e.what( ) ).c_str( ) );
					return false;
				}
				return true;
			}

			bool FileTarget::CloseFile( )
			{
				try {
					fileHandle.flush( );
					fileHandle.close( );
				}
				catch( const std::exception &e ) {
					printf( "%s\n", se_colors::Tag::Red( "Error In Closing File:" ).c_str( ) );
					printf( "%s %s %s\n", se_colors::basic_colors::foreground::red, e.what( ),
						se_colors::formats::reset );
					return false;
				}
				return true;
			}

			bool FileTarget::RenameFile( std::string_view newFileName )
			{
				std::filesystem::path newFile { filePath };
				newFile.replace_filename( newFileName );
				return file_utils::RenameFile( filePath, newFile.filename( ) );
			}

			void FileTarget::PrintMessage( LoggerLevel level, const std::string msg, std::format_args &&args )
			{
				if( logLevel <= level ) {
					MsgInfo( )->SetMessageLevel( level );
					if( !fileHandle.is_open( ) ) {
						OpenFile( );
					}

					if( policy.GetFlushSetting( ) == Flush_Policy::Flush::always ) {
						fileHandle << std::move( MsgFmt( )->FormatMessage( msg, args ) );
						Flush( );
					}
					else {
						// Logic Based Off Sub-Options For Flush Policy Goes Here
						switch( policy.GetPeriodicSetting( ) ) {
							case Flush_Policy::Periodic_Options::mem_usage:
								{
									if( buffer.size( ) >= BUFFER_CHECK ) {
										fileHandle << buffer;
										Flush( );
										buffer.clear( );
										buffer = std::move(MsgFmt( )->FormatMessage( msg, args ));
									}
									else {
										buffer += MsgFmt( )->FormatMessage( msg , args );	
									}
								}
								break;
							case Flush_Policy::Periodic_Options::time_based:
								{
									// Would cause re-allocations when buffer creeps past reserved
									// size but would flush in the time interval given, once i find
									// a nice way to add a time param
								}
								break;
							case Flush_Policy::Periodic_Options::undef:
								{
									fileHandle << std::move( MsgFmt( )->FormatMessage( msg, args ) );
									Flush( );  // if undefined, just default to flushing as if set to "Always"
								}
								break;
						}
					}
				}
				else {
					return;
				}
			}

			bool FileTarget::Flush( )
			{
				try {
					fileHandle.flush( );
				}
				catch( std::exception &e ) {
					return false;
				}
				return true;
			}


		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity
