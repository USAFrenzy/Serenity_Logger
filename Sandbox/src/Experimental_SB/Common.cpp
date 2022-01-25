#include "Common.h"

namespace serenity::expiremental
{
	void RotateSettings::SetOriginalSettings( const std::filesystem::path &filePath )
	{
		path = filePath;
		ext  = filePath.filename( ).extension( ).string( );
		auto fName { filePath.filename( ) };
		fileName = fName.replace_extension( ).string( );
		auto dir { filePath };
		directory = dir.remove_filename( );
	}

	const std::filesystem::path &RotateSettings::OriginalPath( )
	{
		return path;
	}

	const std::filesystem::path &RotateSettings::OriginalDirectory( )
	{
		return directory;
	}

	const std::string &RotateSettings::OriginalName( )
	{
		return fileName;
	}

	void RotateSettings::SetCurrentFileSize( size_t currentSize )
	{
		currentFileSize = currentSize;
	}

	const std::string &RotateSettings::OriginalExtension( )
	{
		return ext;
	}

	const size_t &RotateSettings::FileSize( )
	{
		return currentFileSize;
	}

}  // namespace serenity::expiremental