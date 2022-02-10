#include <serenity/Common.h>

#include <iostream>

namespace serenity::experimental
{
	void RotateSettings::CacheOriginalPathComponents( const std::filesystem::path &filePath )
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

	void RotateSettings::InitFirstRotation( bool enabled )
	{
		initalRotationEnabled = enabled;
	}

	const bool RotateSettings::IsIntervalRotationEnabled( )
	{
		return initalRotationEnabled;
	}
}  // namespace serenity::experimental