#pragma once

#include <spdlog/sinks/base_sink.h>

#include <iostream>

// TODO: Add Color Support (Probably Just Need To Inherit From Those Respective Sinks Instead Or Implement A Color Range)
// Lazy Implementation: could just create a std_out and std_err sink on creation of this sink and call them in the respective condition
// checks...loads of overhead with that idea though sooooo probably not going that route lol
namespace serenity
{
	namespace sinks
	{
		// class that separates it's messages based on message log level (Useful For Piping Based On Level)
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