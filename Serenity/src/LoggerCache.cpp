#include <serenity/Cache/LoggerCache.hpp>


namespace serenity
{
	cache_logger::cache_logger( )
	{
		cache_instance(this);
	}

	cache_logger *cache_logger::instance( )
	{
		return m_instance;
	}


	void cache_logger::cache_instance(cache_logger *cacheObj )
	{
		m_instance = std::move(cacheObj);
	}

	// Probs Be Something Like CloneLogger() Once Set Up
	//std::unique_ptr<Logger> cache_logger::GetNewLogger( )
	//{
	//	std::unique_ptr<Logger> log = std::make_unique<Logger>( cacheInitInfo );
	//	std::copy(log.get(), log.get(), cacheNewLogger);
	//	return log;
	//}
}  // namespace serenity