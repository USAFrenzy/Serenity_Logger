#include "serenity/Utilities/Utilities.hpp"

#include <regex>
#include <fstream>
#include <serenity/Logger.hpp>

#define INSTRUMENTATION_ENABLED 1

namespace serenity
{
	namespace se_utils
	{
		Instrumentor::Instrumentor( ) { }
		void Instrumentor::StopWatch_Start( )
		{
			m_Start = std::chrono::high_resolution_clock::now( );
		}
		void Instrumentor::StopWatch_Stop( )
		{
			m_End = std::chrono::high_resolution_clock::now( );
		}

		float Instrumentor::Elapsed_In( time_mode mode )
		{
			switch( mode ) {
				case time_mode::ms:
					return std::chrono::duration_cast<std::chrono::nanoseconds>( m_End - m_Start ).count( );
					break;
				case time_mode::sec:
					return std::chrono::duration_cast<std::chrono::seconds>( m_End - m_Start ).count( );
					break;
				case time_mode::min:
					return std::chrono::duration_cast<std::chrono::minutes>( m_End - m_Start ).count( );
					break;
				case time_mode::hr:
					return std::chrono::duration_cast<std::chrono::hours>( m_End - m_Start ).count( );
					break;
				default: return 0.f; break;
			}
		}
		Instrumentor::~Instrumentor( ) { }

		void SleepFor( time_mode mode, int time )
		{
			switch( mode ) {
				case time_mode::ms: std::this_thread::sleep_for( std::chrono::milliseconds( time ) ); break;
				case time_mode::sec: std::this_thread::sleep_for( std::chrono::seconds( time ) ); break;
				case time_mode::min: std::this_thread::sleep_for( std::chrono::minutes( time ) ); break;
				case time_mode::hr: std::this_thread::sleep_for( std::chrono::hours( time ) ); break;
			}
		}
	}  // namespace se_utils
	namespace file_utils
	{
		/*
		This Regex Pattern will allow any filename other than those that contain these in their name:
		-/	-|	-<	->	-:	-\	-?	-*	-" // Invalid Windows File Name
	   Characters -CON, PRN, AUX, NUL, COM1, COM2, COM3, COM4, COM5, COM6, COM7, COM8, COM9, LPT1, LPT2, LPT3,
	   LPT4, LPT5, LPT6, LPT7, LPT8, LPT9 // Windows Reserved File Names

		Otherwise The Only Banned Characters Are:
		-\	-NUL	-:   -/                      // Only Restrictions On Linux/Unix And Forcing ':' And '/'
	   To Be Restricted For MacOS And Sanity Respectively
	*/

		bool ValidateFileName( std::string fileName )
		{
			std::smatch match;
#if WIN32
			std::regex validateFile(
			  "^[\\/:\"*?<>|]|CON|PRN|AUX|NUL|COM1|COM2|COM3|COM4|COM5|COM6|COM7|"
			  "COM8|COM9|LPT1|LPT2|LPT3|LPT4|LPT5|LPT6|LPT7|LPT8|LPT9+$" );
#else
			std::regex validateFile( "^[\\/:]|NUL+$" );
#endif
			if( ( std::regex_search( fileName, match, validateFile ) ) ) {
				SE_INTERNAL_ERROR(
				  "ERROR:\t"
				  "File Name [ {} ] Contains Invalid Characters Or Reserved File Names. (If On Linux/Unix, This "
				  "Includes "
				  "':' And '\\'",
				  fileName );
				return false;
			}
			else {
				return true;
			}
		}

		bool ValidateExtension( std::string fileName )
		{
			std::smatch match;
			std::regex  validExtension( "^\\.[a-zA-Z]{7}$" );  // making 7 a thing here due to files like .config
			if( ( std::regex_search( fileName, match, validExtension ) ) ) {
				SE_INTERNAL_WARN(
				  "WARNING:\t"
				  "File Name [ {} ] Extension Is Invalid.",
				  fileName );
				return false;
			}
			else {
				return true;
			}
		}


		bool CompareExtensions( std::string oldFile, std::string newFile )
		{
			std::string oldExtension, newExtension;
			// Find The Respective Extensions
			if( !( oldFile.find_last_of( "." ) != std::string::npos ) ) {
				oldExtension = static_cast<char>( oldFile.substr( ).find_last_of( "." ) + 1 );
			}
			if( !( newFile.find_last_of( "." ) != std::string::npos ) ) {
				newExtension = static_cast<char>( newFile.substr( ).find_last_of( "." ) + 1 );
			}
			return ( newExtension == oldExtension ) ? true : false;
		}

		bool RenameFile( file_helper::path oldFile, file_helper::path newFile )
		{
			std::error_code ec;
			std::ofstream   file1( oldFile );
			std::ofstream   file2( newFile );
			// dir_entries are really only neccessary due to file_size comparison and exists()
			// if std::fs implements this for path objects, this is a candidate for removal
			file_helper::directory_entry oldPath { oldFile };
			file_helper::directory_entry newPath { newFile };

			if( newPath.exists( ) ) {
				return true;
			}
			if( file1.is_open( ) ) {
				try {
					file1.close( );
				}
				catch( const std::exception &e ) {
					SE_INTERNAL_ERROR( "ERROR: {}", e.what( ) );
					return false;
				}
			}
			try {
				file_utils::ValidateFileName( newFile.filename( ).string( ) );
			}
			catch( std::exception &fileName_err ) {
				SE_INTERNAL_ERROR( "Could Not Rename {} To {}\nReason: {}", oldFile.filename( ), newFile.filename( ),
						   fileName_err.what( ) );
				return false;
			}
			if( !file_utils::ValidateExtension( newFile.extension( ).string( ) ) ) {
				SE_INTERNAL_ERROR( "Could Not Rename {} To {}\tReason: Not A Valid Extension String",
						   oldFile.filename( ), newFile.filename( ) );
				return false;
			}
			else {
				try {
					if( !( file_utils::CompareExtensions( oldFile.extension( ).string( ),
									      newFile.extension( ).string( ) ) ) ) {
						SE_INTERNAL_WARN(
						  "WARNING:\t"
						  "New File Name: [ {} ] Does Not Have The Same Extension As Old File: [ {} ]\n",
						  newFile.filename( ), oldFile.filename( ) );
						file_helper::rename( oldPath.path( ).filename( ), newPath.path( ).filename( ) );
					}
					else {
						file_helper::rename( oldPath.path( ).filename( ), newPath.path( ).filename( ) );
					}
				}
				catch( const file_helper::filesystem_error &err ) {
					SE_INTERNAL_ERROR( "Exception Caught In RenameFile():\n{}", err.what( ) );
					return false;
				}
			}
			if( ( oldPath.path( ).filename( ) == newPath.path( ).filename( ) ) &&
			    ( oldPath.file_size( ) == newPath.file_size( ) ) )
				return true;
			else {
				return false;
			}
		}

		file_utils_results::retrieve_dir_entries RetrieveDirEntries( std::filesystem::path &path, bool recursive )
		{
			std::vector<std::filesystem::directory_entry> dirEntries;
			std::size_t                                   pathSize = path.string( ).size( );
			file_utils_results::retrieve_dir_entries      results;
#if INSTRUMENTATION_ENABLED
			se_utils::Instrumentor timer;
#endif
			dirEntries.clear( );
			int fileCount { 0 };

			if( path.has_extension( ) ) {
				SE_INTERNAL_ERROR(
				  "ERROR: Path To Retrieve Directory Entries Points To A File Instead Of A Directory\nPath: {}", path );
				results.retrievedItems = dirEntries;
				results.success        = false;
			}
#if INSTRUMENTATION_ENABLED

			timer.StopWatch_Start( );
#endif
			if( recursive ) {
				for( const std::filesystem::directory_entry &entry :
				     std::filesystem::recursive_directory_iterator( path ) ) {
					dirEntries.emplace_back( entry.path( ).string( ) );
					fileCount++;
				}
			}
			else {
				for( const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator( path ) ) {
					dirEntries.emplace_back( entry.path( ).string( ) );
					fileCount++;
				}
			}
#if INSTRUMENTATION_ENABLED

			timer.StopWatch_Stop( );
#endif
			results.success        = true;
			results.retrievedItems = dirEntries;
			results.fileCount      = fileCount;
#if INSTRUMENTATION_ENABLED
			results.elapsedTime = timer.Elapsed_In( se_utils::time_mode::ms );
#endif
			return results;
		}


		// std::tuple<bool, std::vector<std::filesystem::directory_entry>>
		file_utils_results::search_dir_entries
		  SearchDirEntries( std::vector<std::filesystem::directory_entry> &dirEntries, std::string searchString )
		{
			std::vector<std::filesystem::directory_entry> matchedResults;
			bool                                          containsMatch { false };
			file_utils_results::search_dir_entries        results;
#if INSTRUMENTATION_ENABLED
			se_utils::Instrumentor timer;
			timer.StopWatch_Start( );
#endif
			for( const auto &entry : dirEntries ) {
				auto entryStr = entry.path( ).filename( ).string( );
				// compare returns 0 if they're equal
				if( entryStr.compare( searchString ) == 0 ) {
					matchedResults.emplace_back( entry );
					containsMatch = true;
				}
			}
#if INSTRUMENTATION_ENABLED
			timer.StopWatch_Stop( );
#endif
			results.fileFound      = containsMatch;
			results.matchedResults = matchedResults;
#if INSTRUMENTATION_ENABLED
			results.elapsedTime = timer.Elapsed_In( se_utils::time_mode::ms );
#endif
			return results;
		}

		// Return The Object Specified (Not Yet Implemented)
		std::filesystem::directory_entry RetrieveDirObject( std::filesystem::directory_entry &entry )
		{
			std::filesystem::directory_entry temp;
			return temp;
		}

	}  // namespace file_utils
}  // namespace serenity