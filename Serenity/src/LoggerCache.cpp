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
}  // namespace serenity