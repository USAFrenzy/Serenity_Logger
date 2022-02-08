#pragma once

#include <serenity/Targets/Target.h>

#include <fstream>

namespace serenity::targets
{
	class FileTarget : public TargetBase
	{
	  public:
		FileTarget( );
		explicit FileTarget( std::string_view fileName, bool replaceIfExists = false );
		explicit FileTarget( std::string_view name, std::string_view filePath, bool replaceIfExists = false );
		explicit FileTarget( std::string_view name, std::string_view formatPattern, std::string_view filePath,
							 bool replaceIfExists = false );
		FileTarget( const FileTarget & ) = delete;
		FileTarget &operator=( const FileTarget & ) = delete;
		~FileTarget( );
		const std::string FilePath( );
		const std::string FileName( );
		void              EraseContents( );
		virtual bool      RenameFile( std::string_view newFileName );
		bool              OpenFile( bool truncate = false );
		bool              CloseFile( );
		void              Flush( );

	  private:
		LoggerLevel   logLevel;
		serenity::experimental::Flush_Policy &policy;

	  protected:
		std::ofstream    fileHandle;
		FileSettings     fileOptions;
		BackgroundThread flushWorker;
		void             PolicyFlushOn( ) override;
		void             PrintMessage( std::string_view formatted ) override;
	};
}  // namespace serenity::experimental::targets