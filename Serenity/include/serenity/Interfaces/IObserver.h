#pragma once

#include <serenity/Common.h>

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
		virtual void        SetLogLevel( LoggerLevel logLevel )     = 0;
		virtual void        SetFlushLevel( LoggerLevel flushLevel ) = 0;
		virtual void        UpdateInfo( )                           = 0;
		virtual bool        ShouldLog( )                            = 0;
		virtual std::string LogLevelToStr( LoggerLevel level )      = 0;
	};

}  // namespace serenity