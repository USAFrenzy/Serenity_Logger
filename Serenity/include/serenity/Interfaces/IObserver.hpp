#pragma once

#include <optional>
#include <filesystem>
#include "serenity/Common.hpp"

class IFileHelper
{
      public:
	using optPath     = std::optional<std::filesystem::path>;
	using MappedLevel = serenity::MappedLevel;

      protected:
	virtual void UpdateFileInfo( std::filesystem::path pathToFile ) = 0;
	virtual void NotifyLogger( )                                    = 0;
};

class ILogger
{
      public:
	virtual void UpdateLoggerFileInfo( ) = 0;
};
