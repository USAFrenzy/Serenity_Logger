#include "RotatingTarget.h"

#include <iostream>

namespace serenity::expiremental::targets
{
	RotatingTarget::RotatingTarget( ) : FileTarget( "Rotating_Log.txt", true ), rotateFile( true ), rotateSettings( RotateSettings( ) )
	{
		rotateSettings.SetOriginalSettings( fileOptions.filePath );
		RenameFileForRotation( );
		SetLoggerName( "Rotating_Logger" );
	}

	RotatingTarget::RotatingTarget( std::string_view name, std::string_view filePath, bool replaceIfExists )
	  : FileTarget( name, filePath, replaceIfExists ), rotateFile( true ), rotateSettings( RotateSettings( ) )
	{
		rotateSettings.SetOriginalSettings( filePath );
		RenameFileForRotation( );
		SetLoggerName( "Rotating_Logger" );
	}

	RotatingTarget::RotatingTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath,
									bool replaceIfExists )
	  : FileTarget( name, formatPattern, filePath, replaceIfExists ), rotateFile( true ), rotateSettings( RotateSettings( ) )
	{
		rotateSettings.SetOriginalSettings( filePath );
		RenameFileForRotation( );
		SetLoggerName( "Rotating_Logger" );
	}

	RotatingTarget::~RotatingTarget( )
	{
		CloseFile( );
	}

	void RotatingTarget::RotateSettings::SetOriginalSettings( const std::filesystem::path &filePath )
	{
		path = filePath;
		ext  = filePath.filename( ).extension( ).string( );
		auto fName { filePath.filename( ) };
		fileName = fName.replace_extension( ).string( );
		auto dir { filePath };
		directory = dir.remove_filename( );
	}

	const std::filesystem::path &RotatingTarget::RotateSettings::OriginalPath( )
	{
		return path;
	}

	const std::filesystem::path &RotatingTarget::RotateSettings::OriginalDirectory( )
	{
		return directory;
	}

	const std::string &RotatingTarget::RotateSettings::OriginalName( )
	{
		return fileName;
	}
	const std::string &RotatingTarget::RotateSettings::OriginalExtension( )
	{
		return ext;
	}

	bool RotatingTarget::RenameFile( std::string_view newFileName )
	{
		try {
			// make copy for old file conversion
			std::filesystem::path newFile { fileOptions.filePath };
			newFile.replace_filename( newFileName );
			CloseFile( );
			file_utils::RenameFile( fileOptions.filePath, newFile );
			fileOptions.filePath = std::move( newFile );

			rotateSettings.SetOriginalSettings( fileOptions.filePath );
			RenameFileForRotation( );

			if( !fileHandle.is_open( ) ) {
				OpenFile( );
			}
			return true;
		}
		catch( const std::exception &e ) {
			std::cerr << "Error In Renaming File:\n";
			std::cerr << e.what( );
			return false;
		}
	}

	void RotatingTarget::ShouldRotateFile( bool shouldRotate )
	{
		rotateFile = shouldRotate;
	}

	void RotatingTarget::RenameFileForRotation( )
	{
		auto       extension { fileOptions.filePath.extension( ).string( ) };
		const auto oldFile { fileOptions.filePath };
		// need to make copy to avoid changing old file so that we can rename it
		auto        rotateFile { fileOptions.filePath };
		std::string fileName { rotateFile.replace_extension( ).string( ) };
		rotateFile.replace_filename( fileName.append( "_" ).append( "01" ).append( extension ) );
		if( fileHandle.is_open( ) ) {
			CloseFile( );
		}
		try {
			std::filesystem::rename( oldFile, rotateFile );
		}
		catch( const std::exception &e ) {
			std::cerr << e.what( );
		}
		fileOptions.filePath = std::move( rotateFile );
		if( !fileHandle.is_open( ) ) {
			OpenFile( );
		}
	}

	void RotatingTarget::SetRotateSettings( RotateSettings settings )
	{
		rotateSettings.currentFileSize  = settings.currentFileSize;
		rotateSettings.fileSizeLimit    = settings.fileSizeLimit;
		rotateSettings.maxNumberOfFiles = settings.maxNumberOfFiles;
		rotateSettings.rotateOnFileSize = settings.rotateOnFileSize;
		rotateFile                      = rotateSettings.rotateOnFileSize;
	}

	void RotatingTarget::RotateFileOnSize( )
	{
		if( !rotateFile ) return;

		CloseFile( );
		bool rotateSuccessful { false };
		// make local copies of originals
		auto newFilePath { rotateSettings.OriginalPath( ) };
		auto originalFile { rotateSettings.OriginalName( ) };
		auto extension { rotateSettings.OriginalExtension( ) };
		auto numberOfFiles { rotateSettings.maxNumberOfFiles };

		for( size_t fileNumber { 1 }; fileNumber <= numberOfFiles; ++fileNumber ) {
			std::string newFile { originalFile };  // effectively reset each loop iteration
			newFile.append( "_" ).append( SE_LUTS::numberStr[ fileNumber ] ).append( extension );
			newFilePath.replace_filename( newFile );
			if( !std::filesystem::exists( newFilePath ) ) {
				fileOptions.filePath = std::move( newFilePath );
				if( OpenFile( true ) ) {
					rotateSuccessful = true;
					break;
				}
			}
		}

		if( !rotateSuccessful ) {
			auto logDirectory { std::filesystem::directory_iterator( rotateSettings.OriginalDirectory( ) ) };
			std::filesystem::file_time_type oldestWriteTime = { std::chrono::file_clock::now( ) };
			std::string                     fileNameToFind { rotateSettings.OriginalName( ) };
			std::filesystem::path           fileToReplace;
			for( auto &file : logDirectory ) {
				if( file.is_regular_file( ) ) {
					if( file.path( ).filename( ).string( ).find( fileNameToFind ) != std::string::npos ) {
						if( file.last_write_time( ) < oldestWriteTime ) {
							oldestWriteTime = file.last_write_time( );
							fileToReplace   = file.path( );
						}
					}
				}
			}
			std::filesystem::remove( fileToReplace );
			auto previousFile { fileOptions.filePath.filename( ).string( ) };

			if( !fileToReplace.empty( ) ) {
				fileOptions.filePath = std::move( fileToReplace );
			}
			else {
				std::cerr << std::vformat(
				"Warning: Unable To Locate Oldest File With Base Name \"{}\". Opening And Truncating Previous File, \"{}\"\n",
				std::make_format_args( originalFile, previousFile ) );
			}

			if( !FileTarget::OpenFile( true ) ) {
				if( fileToReplace != previousFile ) {
					std::cerr << std::vformat( "Error: Unable To Finish Rotating From File \"{}\" To File \"{}\"\n",
											   std::make_format_args( previousFile, fileOptions.filePath.filename( ).string( ) ) );
				}
				else {
					std::cerr
					<< std::vformat( "Error: Unable To Open And Truncate File \"{}\"\n", std::make_format_args( previousFile ) );
				}
			}
		}
	}

	void RotatingTarget::PrintMessage( std::string_view formatted )
	{
		auto flushThread { flushWorker.flushThreadEnabled.load( std::memory_order::relaxed ) };
		if( flushThread ) {
			while( !flushWorker.readWriteMutex.try_lock( ) ) {
				std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
			}
		}
		if( rotateFile ) {
			if( rotateSettings.currentFileSize >= rotateSettings.fileSizeLimit ) {
				RotateFileOnSize( );
				rotateSettings.currentFileSize = 0;
			}
		}
		rotateSettings.currentFileSize += formatted.size( );

		fileHandle.rdbuf( )->sputn( formatted.data( ), formatted.size( ) );
		if( Policy( ).SubSetting( ) == PeriodicOptions::memUsage ) fileOptions.fileBufOccupied += formatted.size( );
		if( flushThread ) {
			flushWorker.readWriteMutex.unlock( );
		}
	}

}  // namespace serenity::expiremental::targets