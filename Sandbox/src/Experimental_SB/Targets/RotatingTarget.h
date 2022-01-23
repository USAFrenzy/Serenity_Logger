#include "FileTarget.h"

namespace serenity::expiremental::targets
{
	class RotatingTarget : public FileTarget
	{
	  public:
		struct RotateSettings
		{
			// Will add an interval based setting later
			// (something like a weekly basis on specified day and a daily setting)
			bool   rotateOnFileSize { false };
			size_t maxNumberOfFiles { 5 };
			size_t fileSizeLimit { 512 * KB };
			size_t currentFileSize { 0 };

			void                         SetOriginalSettings( const std::filesystem::path &filePath );
			const std::filesystem::path &OriginalPath( );
			const std::filesystem::path &OriginalDirectory( );
			const std::string &          OriginalName( );
			const std::string &          OriginalExtension( );

		  private:
			std::string           ext, fileName;
			std::filesystem::path path, directory;
		};

		RotatingTarget( );  // default that will just write to a "Generic_Log.txt"
		RotatingTarget( std::string_view filePath, bool replaceIfExists = false );
		RotatingTarget( const RotatingTarget & ) = delete;
		RotatingTarget &operator=( const RotatingTarget & ) = delete;
		~RotatingTarget( );

		void ShouldRotateFile( bool shouldRotate = true );
		void SetRotateSettings( RotateSettings settings );
		void RenameFileForRotation( );
		void RotateFileOnSize( );
		bool RenameFile( std::string_view newFileName ) override;
		void PrintMessage( std::string_view formatted ) override;

	  private:
		bool           rotateFile { false };
		RotateSettings rotateSettings = { };
	};

}  // namespace serenity::expiremental::targets