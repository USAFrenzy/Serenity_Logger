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
				buffer.reserve( BUFFER_SIZE );

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
				buffer.reserve( BUFFER_SIZE );
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
						fileHandle = std::fopen( filePath.string( ).c_str( ), "a+" );
					}
					else {
						fileHandle = std::fopen( filePath.string( ).c_str( ), "w+" );
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
					std::fclose( fileHandle );
					fileHandle = nullptr;
				}
				catch( const std::exception &e ) {
					printf( "%s\n", se_colors::Tag::Red( "Error In Closing File:" ).c_str( ) );
					printf( "%s\n", se_colors::Tag::Red( e.what( ) ).c_str( ) );
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


			// clang-format off
			/*
				Just mucking around with what I can optimize in this function to make it comparable to spdlog.
				Currently, spdlog runs ~3x faster than this in the file benchmark (~0.9-1.1 ms vs ~3.3-3.5 ms).
				Commenting this function out results in the this running ~10% faster (~0.98-1 ms vs 0.8-0.9 ms, so the
				bottleneck is definitely in this function. Formatting Alone Only Takes about 0.5 ms so that's not the
				real issue here (Before Doing a quick micro bench, originally thought it was the formatting method).

				EDIT: Commenting out everything but just appending the formatted message to the buffer here took about 
				the same amount of time -> This is probably where I can optimize this function and why I originally
				thought the formatting was what was taking so long until the micro benchmark
			*/
			// clang-format on

			void FileTarget::PrintMessage( msg_details::Message_Info msgInfo, const std::string_view msg,
						       std::format_args &&args )
			{
				static Flush_Policy p_policy;
				p_policy = TargetBase::FlushPolicy( );
				if( !p_policy.ShouldFlush( ) ) {
					buffer.emplace_back( std::move(MsgFmt( )->FormatMsg( msgInfo.TimeDetails( ).Cache( ), msg, args ) ) );
				}
				else {
					this->PolicyFlushOn( p_policy, std::move(MsgFmt( )->FormatMsg(
									 msgInfo.TimeDetails( ).Cache( ), msg, args ) ) );
				}
			}

			void FileTarget::Flush( )
			{
				// buffer already formatted so using fwrite() vs fprintf() here
				std::unique_ptr<std::string> tmp = std::make_unique<std::string>();
				tmp->reserve( buffer.size() );
				tmp->clear( );
				for( const auto &msg : buffer ) {
					tmp->append(msg);
				}
				fwrite( tmp->data( ), sizeof( char ), tmp->size( ), fileHandle );
				std::fflush( fileHandle );
				buffer.clear( );
			}

			void FileTarget::PolicyFlushOn( Flush_Policy &policy, std::string_view msg )
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
										if( ( ( buffer.size( ) + msg.size( ) ) < BUFFER_SIZE ) ) {
											buffer.emplace_back( std::move( msg ) );
										}
										else {
											Flush( );
											buffer.emplace_back( std::move( msg ) );
										}
										return;
									}
								case Flush_Policy::Periodic_Options::time_based:
									{
										// Time Based FLushing Logic Goes Here
									}
									return;
								case Flush_Policy::Periodic_Options::undef:
									{
										// Behave As If Flush Setting = Never
										buffer.emplace_back( std::move( msg ) );
									}
									return;
							}
						}  // periodic option check
					case Flush_Policy::Flush::always:
						{
							// Always Flush
							buffer.emplace_back( std::move( msg ) );
							Flush( );
						}
						return;
				}
			}


		}  // namespace targets
	}          // namespace expiremental
}  // namespace serenity
