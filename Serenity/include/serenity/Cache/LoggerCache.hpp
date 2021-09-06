#pragma once

#include <memory>
#include <string>
#include <filesystem>

#include <serenity/Common.hpp>

namespace serenity
{
	struct cache_logger
	{
		explicit cache_logger( );
		cache_logger *instance( );
		// std::unique_ptr<serenity::Logger> GetNewLogger( );

	      public:
		std::unique_ptr<spdlog::logger> cacheInternalLogger;
		std::unique_ptr<spdlog::logger> cacheClientLogger;
		// std::shared_ptr<Logger>         cacheNewLogger;

		std::string                   cacheLogName;
		std::string                   cacheLoggerName;
		MappedLevel                   cacheLevel;
		file_helper::path             cacheLogPath;
		file_helper::path             cacheLogDirPath;
		file_helper::path             cachePath;
		std::vector<spdlog::sink_ptr> cacheSinks;
		logger_info                   cacheInitInfo;


	      private:
		void cache_instance( cache_logger *cacheObj );

	      private:
		cache_logger *m_instance;
	};

}  // namespace serenity