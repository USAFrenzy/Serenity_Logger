#pragma once

#include <chrono>
#include <thread>
#include <future>
#include <filesystem>
#include <mutex>


namespace serenity
{
	namespace se_utils
	{
		enum class time_mode
		{
			ms,
			sec,
			min,
			hr,
		};

		class Instrumentor
		{
		      public:
			Instrumentor( );

			void  StopWatch_Start( );
			void  StopWatch_Stop( );
			float Elapsed_In( time_mode mode );

			~Instrumentor( );

		      private:
			std::chrono::time_point<std::chrono::high_resolution_clock> m_Start, m_End;
		};
		/// <summary>
		/// A wrapper for thread sleeping
		/// </summary>
		static void SleepFor( time_mode mode, int time );
	}  // namespace se_utils

	namespace file_utils
	{
		namespace file_utils_results
		{
			struct search_dir_entries
			{
				std::vector<std::filesystem::directory_entry> matchedResults;
				bool                                          fileFound { false };
				float                                         elapsedTime { 0.f };
			};
			struct retrieve_dir_entries
			{
				int                                           fileCount { 0 };
				bool                                          success { false };
				std::vector<std::filesystem::directory_entry> retrievedItems;
				float                                         elapsedTime { 0.f };
			};
		}  // namespace file_utils_results

		/* clang-format off */
		/// <summary>
		/// Takes Input Path's Directory And Retrieves All Files In That Directory. If bool recursive = true, then Performs
		/// A Recursive Retrieval For That Directory And Its Sub-directories. Results Can Be Accessed Through The Return's
		/// Struct Variables (i.e. auto results = RetrieveDirEntries(); results.success; results.retrievedItems;)
		/// </summary>
		file_utils_results::retrieve_dir_entries RetrieveDirEntries( std::filesystem::path &path, bool recursive );
		/* clang-format on */
		/* clang-format off */
		/// <summary>
		/// Searches Input Directory Vector For A Match And Returns 'true' If Found As Well As The Path To The File, Otherwise,
		/// Returns False. Values Can Be Accessed Via The Return Value (i.e. auto result = RetrieveDirEntries();
		/// result.fileFound; result.matchedResults;)
		/// </summary> 
		file_utils_results::search_dir_entries
		  SearchDirEntries( std::vector<std::filesystem::directory_entry> &dirEntries, std::string searchString );
		/* clang-format on */
		/* clang-format off */
		/// <summary>
		/// Not Yet Implemented
		/// </summary>
		/// <param name="entry"></param>
		/// <returns></returns>
		std::filesystem::directory_entry RetrieveDirObject( std::filesystem::directory_entry &entry );
		/* clang-format on */
		/* clang-format off */
		/// <summary>
		/// Validates That The File Name Doesn't Contain Any Illegal Characters. On Windows, This Includes Reserved File
		/// Names. On Linux/Unix This Includes The Addition Of ":" And "\". Throws The File Name If It Does Contain Illegal
		/// Characters
		/// </summary>
		bool ValidateFileName( std::string fileName );
		/* clang-format on */
		/// <summary>
		/// Validates The Extension String Follows A "." And Is At Most, Seven Characters Long
		/// </summary>
		bool ValidateExtension( std::string fileName );
		/// <summary>
		/// Compares The Input Extensions And Returns True If Identical Or False If Not.
		/// </summary>
		bool CompareExtensions( std::string oldFile, std::string newFile );
		/// <summary>Takes A Path To The Old File To Be Renamed And A Path To The New File To Be Renamed.
		/// Renames The Old Path To The New Path. </summary>
		/// <returns>Returns False If The Old Path Doesn't Exist, If New Path Contains Illegal Characters In
		/// The File Name Or In The Extension, Or If The File Size After Renaming Changed.
		/// Returns True On Success Or If The New Path Already Exists.</returns>
		bool RenameFile( std::filesystem::path oldFile, std::filesystem::path newFile );
	}  // namespace file_utils

	// ########################################### WIP ###########################################
	namespace se_thread
	{
		struct se_mutex_guard
		{
			void acquire_lock( )
			{
				std::unique_lock<std::mutex> local_lock( m_mutex );
				fileLock = std::move( local_lock );
			}
			void release_lock( )
			{
				std::unique_lock<std::mutex> local_lock = std::move( fileLock );
				local_lock.unlock( );
			}

		      private:
			std::unique_lock<std::mutex> fileLock;
			std::mutex                   m_mutex;
		};
	}  // namespace se_thread
}  // namespace serenity