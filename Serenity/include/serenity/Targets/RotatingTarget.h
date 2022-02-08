#pragma once

#include <serenity/Targets/FileTarget.h>

namespace serenity::experimental::targets
{
	class RotatingTarget : public serenity::targets::FileTarget
	{
	  public:
		RotatingTarget( );
		explicit RotatingTarget( std::string_view name, std::string_view filePath, bool replaceIfExists = false );
		explicit RotatingTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath,
								 bool replaceIfExists = false );
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
		bool           rotateFile;
		RotateSettings rotateSettings;
	};

}  // namespace serenity::experimental::targets