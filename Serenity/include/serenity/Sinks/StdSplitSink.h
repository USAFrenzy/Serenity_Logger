#pragma once

#include <spdlog/sinks/base_sink.h>

namespace serenity
{
	namespace sinks
	{
		// class that separates it's messages based on message log level
		// TODO: Add Color Support (Probably Just Need To Inherit From Those Respective Sinks Instead)
		template <typename Mutex> class std_split : public spdlog::sinks::base_sink<Mutex>
		{
		      protected:
			void sink_it_( const spdlog::details::log_msg &msg ) override
			{
				spdlog::memory_buf_t formatted;
				spdlog::sinks::base_sink<Mutex>::formatter_->format( msg, formatted );

				if( msg.level != se_utils::ToMappedLevel( LoggerLevel::off ) ) {
					if( msg.level >= se_utils::ToMappedLevel( LoggerLevel::warning ) ) {
						std::cerr << fmt::to_string( formatted );
					}
					else {
						std::cout << fmt::to_string( formatted );
					}
				}
			}

			void flush_( ) override
			{
				std::cout << std::flush;
			}
		};
#include <spdlog/details/null_mutex.h>
#include <mutex>
		using std_split_sink_mt = std_split<std::mutex>;
		using std_split_sink_st = std_split<spdlog::details::null_mutex>;
	}  // namespace sinks
}  // namespace serenity