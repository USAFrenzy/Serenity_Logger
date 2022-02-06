
#include <serenity/Utilities/Utilities.h>

#include <regex>
#include <fstream>
#include <thread>

#define INSTRUMENTATION_ENABLED 1

namespace serenity
{
	namespace se_utils
	{
		Allocation_Statistics Instrumentator::mem_tracker = { };
		Instrumentator::Instrumentator( )
		{
			m_Start = std::chrono::steady_clock::now( );
		}
		void Instrumentator::StopWatch_Reset( )
		{
			m_Start = std::chrono::steady_clock::now( );
		}
		void Instrumentator::StopWatch_Stop( )
		{
			m_End = std::chrono::steady_clock::now( );
		}

		float Instrumentator::Elapsed_In( time_mode mode )
		{  // clang-format off
			switch( mode ) {
				case time_mode::us:
					return std::chrono::duration_cast<pMicro<float>>( m_End - m_Start ).count( );
					break;
				case time_mode::ms:
					return std::chrono::duration_cast<pMilli<float>>( m_End - m_Start ).count( );
					break;
				case time_mode::sec: 
					return std::chrono::duration_cast<pSec<float>>( m_End - m_Start ).count( ); 
					break;
				case time_mode::min: 
					return std::chrono::duration_cast<pMin<float>>( m_End - m_Start ).count( ); 
					break;
				case time_mode::hr: 
					return std::chrono::duration_cast<pHour<float>>( m_End - m_Start ).count( ); 
					break;
				default: 
					return 0; 
					break;
			}
		}  // clang-format on

		void *Instrumentator::operator new( std::size_t n )
		{
			mem_tracker.Allocated += n;
			return malloc( n );
		}
		void Instrumentator::operator delete( void *p ) throw( )
		{
			mem_tracker.Freed -= sizeof( p );
			free( p );
		}

		uint64_t Allocation_Statistics::Memory_Usage( )
		{
			return ( Allocated - Freed );
		}

		Instrumentator::~Instrumentator( ) { }

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
		namespace file_utils_results
		{
			std::vector<std::filesystem::directory_entry> retrieve_dir_entries::retrievedItems;
		}
		// clang-format off
		/*
		 * This Regex Pattern will allow any filename other than those that contain these in their name:
		 * -/	-|	-<	->	-:	-\	-?	-*	-"                                                       // Invalid Windows File Name Characters 
		 * -CON, PRN, AUX, NUL, COM1, COM2, COM3, COM4, COM5, COM6, COM7, COM8, COM9, LPT1, LPT2, LPT3,  // ------------------------------------>
		 * LPT4, LPT5, LPT6, LPT7, LPT8, LPT9                                                           // Windows Reserved File Names
		 *
		 * Otherwise The Only Banned Characters Are:
		 * -\	-NUL	-:   -/    // Only Restrictions On Linux/Unix And Forcing ':' And '/' To Be Restricted For MacOS And Sanity Respectively
	     */
		// clang-format on

		bool const ValidateFileName( std::string fileName )
		{
			std::smatch match;
#if WIN32
			std::regex validateFile(
			"^[\\/:\"*?<>|]|CON|PRN|AUX|NUL|COM1|COM2|COM3|COM4|COM5|COM6|COM7|"
			"COM8|COM9|LPT1|LPT2|LPT3|LPT4|LPT5|LPT6|LPT7|LPT8|LPT9+$" );
#else
			std::regex validateFile( "^[\\/:]|NUL+$" );
#endif
			return ( std::regex_search( fileName, match, validateFile ) ) ? false : true;
		}

		bool const ValidateExtension( std::string fileName )
		{
			std::smatch match;
			std::regex  validExtension( "^\\.[a-zA-Z]{7}$" );  // making 7 a thing here due to files like .config
			return ( std::regex_search( fileName, match, validExtension ) ) ? false : true;
		}

		bool const CompareExtensions( std::string oldFile, std::string newFile )
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

		bool RenameFile( std::filesystem::path oldFile, std::filesystem::path newFile )
		{
			if( std::filesystem::exists( newFile ) ) {
				if( newFile == oldFile ) {
					std::filesystem::remove( oldFile);
				}
				else {
					std::filesystem::remove( newFile );
					std::filesystem::rename( oldFile, newFile );
				}
				return true;
			}

			std::error_code ec;
			try {
				file_utils::ValidateFileName( newFile.filename( ).string( ) );
			}
			catch( std::exception &fileName_err ) {
				printf( "Could Not Rename %s To %s\nReason: %s\n",
						oldFile.filename( ).string( ).c_str( ),
						newFile.filename( ).string( ).c_str( ),
						fileName_err.what( ) );
				return false;
			}

			if( !file_utils::ValidateExtension( newFile.extension( ).string( ) ) ) {
				printf( "Could Not Rename %s To %s\tReason: Not A Valid Extension String\n",
						oldFile.filename( ).string( ).c_str( ),
						newFile.filename( ).string( ).c_str( ) );
				return false;
			}
			else {
				try {
					if( !( file_utils::CompareExtensions( oldFile.extension( ).string( ), newFile.extension( ).string( ) ) ) ) {
						printf(
						"WARNING:\t"
						"New File Name: [ %s ] Does Not Have The Same Extension As Old File: [ %s ]\n",
						newFile.filename( ).string( ).c_str( ),
						oldFile.filename( ).string( ).c_str( ) );
						std::filesystem::rename( oldFile, newFile );
					}
					else {
						std::filesystem::rename( oldFile, newFile );
					}
				}
				catch( const std::filesystem::filesystem_error &err ) {
					printf( "\nException Caught In RenameFile():\n%s\n", err.what( ) );
					return false;
				}
			}

			if( ( oldFile.filename( ) == newFile.filename( ) ) &&
				( std::filesystem::file_size( oldFile ) == std::filesystem::file_size( newFile ) ) )
			{
				return true;
			}
			else {
				return false;
			}
		}

		file_utils_results::retrieve_dir_entries const RetrieveDirEntries( std::filesystem::path &path, bool recursive )
		{
			std::vector<std::filesystem::directory_entry> dirEntries;
			std::size_t                                   pathSize = path.string( ).size( );
			file_utils_results::retrieve_dir_entries      results;
#if INSTRUMENTATION_ENABLED
			se_utils::Instrumentator timer;
#endif
			dirEntries.clear( );
			int fileCount { 0 };

			if( path.has_extension( ) ) {
				printf(
				"ERROR: Path To Retrieve Directory Entries Points To A File Instead Of A Directory\nPath: "
				"%s\n",
				path.string( ).c_str( ) );
				results.retrievedItems = dirEntries;
				results.success        = false;
			}
#if INSTRUMENTATION_ENABLED

			timer.StopWatch_Reset( );
#endif
			if( recursive ) {
				for( const std::filesystem::directory_entry &entry : std::filesystem::recursive_directory_iterator( path ) ) {
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
		file_utils_results::search_dir_entries const
		SearchDirEntries( std::vector<std::filesystem::directory_entry> &dirEntries, std::string searchString )
		{
			std::vector<std::filesystem::directory_entry> matchedResults;
			bool                                          containsMatch { false };
			file_utils_results::search_dir_entries        results;
#if INSTRUMENTATION_ENABLED
			se_utils::Instrumentator timer;
			timer.StopWatch_Reset( );
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

		// TODO: Return The Object Specified (Not Fully Implemented Yet)
		// ########################################################################################################################################
		std::filesystem::directory_entry const RetrieveDirObject( std::filesystem::directory_entry &entry )
		{
			std::filesystem::directory_entry temp { };
			using prevRetrieved = file_utils_results::retrieve_dir_entries;

			file_utils_results::search_dir_entries results;
			results = SearchDirEntries( prevRetrieved::retrievedItems, entry.path( ).string( ) );
			if( results.fileFound ) {
				temp = results.matchedResults.front( );
			}
			return temp;
		}

		bool CreateDir( std::filesystem::path dirPath )
		{
			std::filesystem::directory_entry entry { dirPath };
			try {
				if( entry.exists( ) ) {
					return true;
				}
				else {
					std::filesystem::create_directories( entry );
					return true;
				}
			}
			catch( const std::exception &e ) {
				printf( "Exception In CreateDir():\n%s\n", e.what( ) );
				return false;
			}
		}

		bool RemoveEntry( std::filesystem::path entry )
		{
			try {
				if( !std::filesystem::exists( entry ) ) {
					return true;
				}
				else {
					std::filesystem::remove( entry );
					return true;
				}
			}
			catch( const std::exception &e ) {
				printf( "Exception In RemoveEntry():\n%s\n", e.what( ) );
				return false;
			}
		}

		bool ChangeDir( std::filesystem::path dirPath )
		{
			std::error_code ec;
			try {
				std::filesystem::current_path( dirPath, ec );
				return true;
			}
			catch( std::filesystem::filesystem_error &e ) {
				printf( "EXCEPTION CAUGHT:\n%s", e.what( ) );
				return false;
			}
		}

		bool CopyContents( std::filesystem::path source, std::filesystem::path destination )
		{
			std::ifstream inputFile( source );
			std::ofstream outputFile( destination, std::ios_base::app );
			std::string   line;
			// If input file isn't empty, try to read from input and write to destination
			if( !( inputFile.peek( ) == std::ifstream::traits_type::eof( ) ) ) {
				try {
					if( inputFile && outputFile ) {
						while( std::getline( inputFile, line ) ) {
							outputFile << line << "\n";
						}
						inputFile.close( );
						outputFile.close( );
					}
				}
				catch( const std::exception &e ) {
					printf( "Failed To Copy File Contents:\n%s\n", e.what( ) );
					inputFile.close( );
					outputFile.close( );
					return false;
				}
			}
			return true;
		}

		bool OpenFile( std::filesystem::path file, bool truncate )
		{
			namespace fs = std::filesystem;
			std::fstream openFile;
			int          mode;
			// By Default Create A File With Full Permissions If File Doesn't Already Exist
			if( !fs::exists( file ) ) {
				try {
					fs::permissions( file, fs::perms::all, fs::perm_options::add );
				}
				catch( const fs::filesystem_error &e ) {
					printf( "Permissions Error In OpenFile():\n%s\n", e.what( ) );
					return false;
				}
			}
			else {
				// By Default, Only Checking If The File Has Any Permissions To Open (Could Use Work Here)
				if( fs::status( file ).permissions( ) == ( fs::perms::none ) ) {
					printf( "Error In OpenFile():\nUnable To Open File Due To Insufficient Permissions." );
					return false;
				}
			}
			if( truncate ) {
				mode = std::ios_base::trunc | std::ios_base::out;
			}
			else {
				mode = std::ios_base::app;
			}
			try {
				openFile.open( file, mode );
			}
			catch( const std::exception &e ) {
				printf( "Exception Caught In OpenFile():\n%s\n", e.what( ) );
				return false;
			}
			return true;
		}

		bool CloseFile( std::filesystem::path file )
		{
			namespace fs = std::filesystem;
			std::fstream closeFile( file );
			// By Default, Just Checking That The File Has Some Permission To Close
			if( fs::status( file ).permissions( ) == fs::perms::none ) {
				printf( "Error In CloseFile():\nInsufficient Permissions\n" );
				return false;
			}
			else {
				try {
					if( !fs::exists( file ) ) {
						return true;
					}
					else {
						closeFile.flush( );
						closeFile.close( );
					}
				}
				catch( const std::exception &e ) {
					printf( "Error In CloseFile():\n%s\n", e.what( ) );
					return false;
				}
			}
			return true;
		}

	}  // namespace file_utils
}  // namespace serenity