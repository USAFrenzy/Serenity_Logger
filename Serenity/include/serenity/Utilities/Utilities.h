#pragma once

#include <chrono>
#include <filesystem>
#include <mutex>

namespace serenity {
	namespace se_utils {
		enum class time_mode
		{
			us,
			ms,
			sec,
			min,
			hr,
		};

		// for more precision
		template<class T> using pMicro = std::chrono::duration<T, std::micro>;
		template<class T> using pMilli = std::chrono::duration<T, std::milli>;
		template<class T> using pSec   = std::chrono::duration<T, std::ratio<1>>;
		template<class T> using pMin   = std::chrono::duration<T, std::ratio<1, 60>>;
		template<class T> using pHour  = std::chrono::duration<T, std::ratio<1, 3600>>;

		struct Allocation_Statistics
		{
				uint64_t Allocated { 0 };
				uint64_t Freed { 0 };
				uint64_t Memory_Usage();
		};

		class Instrumentator
		{
			public:
				Instrumentator();

				void StopWatch_Reset();
				void StopWatch_Stop();
				float Elapsed_In(time_mode mode);

				void* operator new(std::size_t n);
				void operator delete(void* p) throw();

				~Instrumentator();

			public:
				static Allocation_Statistics mem_tracker;

			private:
				std::chrono::time_point<std::chrono::steady_clock> m_Start, m_End;
		};

		/// <summary>
		/// A wrapper for thread sleeping
		/// </summary>
		void SleepFor(time_mode mode, int time);
	}    // namespace se_utils

	namespace file_utils {
		namespace file_utils_results {
			struct search_dir_entries
			{
					std::vector<std::filesystem::directory_entry> matchedResults;
					bool fileFound { false };
					float elapsedTime { 0 };
			};

			struct retrieve_dir_entries
			{
					int fileCount { 0 };
					bool success { false };
					static std::vector<std::filesystem::directory_entry> retrievedItems;
					float elapsedTime { 0 };
			};
		}    // namespace file_utils_results

		/* clang-format off */
		/// <summary>
		/// Takes Input Path's Directory And Retrieves All Files In That Directory. If bool recursive = true, then Performs
		/// A Recursive Retrieval For That Directory And Its Sub-directories. Results Can Be Accessed Through The Return's
		/// Struct Variables (i.e. auto results = RetrieveDirEntries(); results.success; results.retrievedItems;)
		/// </summary>
		file_utils_results::retrieve_dir_entries const RetrieveDirEntries( std::filesystem::path &path, bool recursive );
		/* clang-format on */
		/* clang-format off */
		/// <summary>
		/// Searches Input Directory Vector For A Match And Returns 'true' If Found As Well As The Path To The File, Otherwise,
		/// Returns False. Values Can Be Accessed Via The Return Value (i.e. auto result = RetrieveDirEntries();
		/// result.fileFound; result.matchedResults;)
		/// </summary> 
		file_utils_results::search_dir_entries const
		  SearchDirEntries( std::vector<std::filesystem::directory_entry> &dirEntries, std::string searchString );
		/* clang-format on */
		/* clang-format off */
		/// <summary>
		/// (Not Fully Implemented Yet) RetrieveDirEntries() Must Be Called Before This Function. Searches The 
		/// Result Vector Of RetrieveDirEntries() For A Match Against The Param entry.
		/// </summary>
		/// <returns>If Entry Has Been Found, Returns The Entry Object, Otherwise, Returns Empty Entry Object</returns>
		std::filesystem::directory_entry const RetrieveDirObject( std::filesystem::directory_entry &entry );
		/* clang-format on */
		/* clang-format off */
		/// <summary>
		/// Validates That The File Name Doesn't Contain Any Illegal Characters. On Windows, This Includes Reserved File
		/// Names. On Linux/Unix This Includes The Addition Of ":" And "\". Throws The File Name If It Does Contain Illegal
		/// Characters
		/// </summary>
		bool const ValidateFileName( std::string fileName );
		/* clang-format on */
		/// <summary>
		/// Validates The Extension String Follows A "." And Is At Most, Seven
		/// Characters Long
		/// </summary>
		bool const ValidateExtension(std::string fileName);
		/// <summary>
		/// Compares The Input Extensions And Returns True If Identical Or False If Not.
		/// </summary>
		bool const CompareExtensions(std::string oldFile, std::string newFile);
		/// <summary>Takes A Path To The Old File To Be Renamed And A Path To The New
		/// File To Be Renamed. Renames The Old Path To The New Path. </summary>
		/// <returns>Returns False If The Old Path Doesn't Exist, If New Path Contains
		/// Illegal Characters In The File Name Or In The Extension, Or If The File Size
		/// After Renaming Changed. Returns True On Success Or If The New Path Already
		/// Exists.</returns>
		bool RenameFile(std::filesystem::path oldFile, std::filesystem::path newFile);
		/// <returns>Returns True On Success Or If Already Exists, False
		/// Otherwise</returns>
		bool CreateDir(std::filesystem::path dirPath);
		/// <returns>Returns True On Success Or If 'entry' Doesn't Exist, False
		/// Otherwise</returns>
		bool RemoveEntry(std::filesystem::path entry);
		/// <returns>Returns True On Success, False Otherwise. Catches And Prints
		/// Exceptions Thrown From This Function, If Any, To The Console</returns>
		bool ChangeDir(std::filesystem::path dirPath);
		/// <returns>Returns True On Success, False Otherwise. Catches And Prints
		/// Exceptions Thrown From This Function, If Any, To The Console</returns>
		bool CopyContents(std::filesystem::path source, std::filesystem::path destination);
		/// <returns>Returns True On Success, False Otherwise. Catches And Prints
		/// Exceptions Thrown From This Function, If Any, To The Console. By Default,
		/// Creates A File With Full Permissions If It Doesn't Exist And Opens It In
		/// Append Mode</returns>
		bool OpenFile(std::filesystem::path file, bool truncate);
		/// <returns>Returns True On Success, False Otherwise. Catches And Prints
		/// Exceptions Thrown From This Function, If Any, To The Console</returns>
		bool CloseFile(std::filesystem::path file);
	}    // namespace file_utils
}    // namespace serenity