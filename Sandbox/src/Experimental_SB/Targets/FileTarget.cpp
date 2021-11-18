#include "FileTarget.h"

#include <filesystem>

#include <serenity/Utilities/Utilities.h>
#include <serenity/Color/Color.h>

namespace serenity
{
	namespace expiremental
	{
		namespace targets
		{
			FileTarget::FileTarget( )
			{
				std::filesystem::path            file = std::filesystem::current_path( );
				std::filesystem::directory_entry logDir { file /= "Logs" };
				file /= logDir;
				file /= "Generic_Log.txt";
				filePath = std::move( file.make_preferred( ).string( ) );
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

			FileTarget::FileTarget( std::string_view filePath, bool replaceIfExists )
			{
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
					fileHandle << MsgFmt( )->FormatMessage( msg, args ) << "\n";
					// TODO: #######################################################################################
					// Possibly Create A Flush Policy Kind Of Deal Here - Similar To What spdlog Has. Either That,
					// Or Close The File After Every Write Op? Decisions, Decisions.. Better Performance If File Is
					// Left Open During App Lifetime And Regular Flushes Would Add Some Safety In The Event Of
					// Crashes So Not All Data Is Lost, But Resources Are Hogged By Keeping The File Open During
					// The App's Lifetime. Not Only That, Unless I Mess With std::filesystem Permissions, I Don't
					// Believe Any Other Processes Would Even Be Able To Read What Is Being Written Concurrently. I
					// Do Need To Think About How I Want To Approach This In The Future With Multi-Threading As
					// Well - Obviously, If Multiple Threads Are Writing To The Same File, There NEEDS To Be A
					// Locking Mechanism (i.e. mutex) Of Sorts Or Some Way To Say "Hey, Currently Writing To File,
					// Flush To Buffer Until Thread 'A' Finishes". Actually, After Writing That, That Might Be A
					// Good Idea - Have A Logging Thread That Logs To A Buffer, Use A Flush Policy To Determine How
					// Often That Buffer Gets Flushed To Disk, And Use A Secondary Thread To Flush That Buffer To
					// Disk Based Off The Flush Policy So As Not To Hinder The First Thread's Logging. Only
					// Downside To This Idea Is Now I Would Have To Deal With Buffer Sizes And What To Do On Low
					// Memory..
					// TODO: #######################################################################################
					Flush( );
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
