#pragma once

#include <serenity/Common.h>

#include <filesystem>


namespace serenity
{
	class IFileHelper
	{
	      protected:
		virtual void UpdateFileInfo( std::filesystem::path pathToFile ) = 0;
		virtual void NotifyLogger( )                                    = 0;
	};

	class ILogger
	{
	      public:
		virtual void        UpdateFileInfo( )                  = 0;
		virtual bool        ShouldLog( )                       = 0;
		virtual std::string LogLevelToStr( LoggerLevel level ) = 0;
	};

}  // namespace serenity