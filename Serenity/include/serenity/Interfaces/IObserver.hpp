#pragma once

#include <filesystem>
#include "serenity/Common.hpp"

class IFileHelper
{
      public:
	using MappedLevel = serenity::MappedLevel;

      protected:
	virtual void UpdateFileInfo( std::filesystem::path pathToFile ) = 0;
	virtual void NotifyLogger( )                                    = 0;
};

class ILogger
{
      public:
	virtual void UpdateFileInfo( ) = 0;
};
