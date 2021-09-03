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



	      public:
		std::unique_ptr<spdlog::logger> cacheInternalLogger;
		std::unique_ptr<spdlog::logger> cacheClientLogger;
		std::string       cacheLogName;
		std::string       cacheLoggerName;
		MappedLevel       cacheLevel;
		file_helper::path cacheLogPath;
		file_helper::path cacheLogDirPath;
		file_helper::path cachePath;

	      private:
		void cache_instance(cache_logger *cacheObj );

	      private:
		cache_logger *m_instance;
	};

}  // namespace serenity