#pragma once

#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>

// TODO: Add Color Support (Probably Just Need To Inherit From Those Respective Sinks Instead Or Implement A Color Range)
// Lazy Implementation: could just create a std_out and std_err sink on creation of this sink and call them in the respective condition
// checks...loads of overhead with that idea though sooooo probably not going that route lol
namespace serenity
{
	namespace sinks
	{
		// class that separates it's messages based on message log level (Useful For Piping Based On Level)
		template <typename Mutex> class std_split : public spdlog::sinks::wincolor_stdout_sink<Mutex>
		{
		      public:
			// Currently (Just A Guess ATM W/O Stepping Through Debugger Yet) I'm pretty sure that this isn't even looked
			// at... pretty certain that due to how i call the logging functions over in sandbox, it just bypasses this
			// class's logic and uses the default spdlog logic - hence why the simple debug print statements don't execute..
			void console_log( const spdlog::details::log_msg &msg )
			{
				spdlog::memory_buf_t formatted;
				spdlog::sinks::base_sink<Mutex>::formatter_->format( msg, formatted );

				if( msg.level != se_utils::ToMappedLevel( LoggerLevel::off ) ) {
					if( msg.level >= se_utils::ToMappedLevel( LoggerLevel::warning ) ) {
						std::cerr << fmt::to_string( formatted );
						std::cerr << "Printed To std::cerr\n";  // debug
					}
					else {
						std::cout << fmt::to_string( formatted );
						std::cerr << "Printed To std::cout\n";  // debug
					}
				}
			}
		};
#include <spdlog/details/null_mutex.h>
#include <mutex>
		using std_split_sink_mt = std_split<spdlog::details::console_mutex>;
		using std_split_sink_st = std_split<spdlog::details::console_nullmutex>;
	}  // namespace sinks
}  // namespace serenity