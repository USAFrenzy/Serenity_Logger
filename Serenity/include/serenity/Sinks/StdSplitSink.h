#pragma once

#include <serenity/Common.h>

#include <spdlog/sinks/base_sink.h>

#include <iostream>

namespace serenity
{
	namespace sinks
	{
		// class that separates it's messages based on message log level (Useful For Piping Based On Level)
		template <typename Mutex> class std_split : public spdlog::sinks::base_sink<Mutex>
		{
		      public:
			std_split( )
			{
#ifdef PLATFORM_WINDOWS
				se_colors::win_colors color;
				msgLevelColors[ spdlog::level::trace ]    = color.WHITE;
				msgLevelColors[ spdlog::level::debug ]    = color.CYAN;
				msgLevelColors[ spdlog::level::info ]     = color.GREEN;
				msgLevelColors[ spdlog::level::warn ]     = color.YELLOW | color.BOLD;
				msgLevelColors[ spdlog::level::err ]      = color.RED | color.BOLD;
				msgLevelColors[ spdlog::level::critical ] = color.BG_RED | color.WHITE | color.BOLD;
				msgLevelColors[ spdlog::level::off ]      = 0;

#else
				se_colors::ansi_colors color;
				msgLevelColors[ spdlog::level::trace ]    = colorStr( color.white );
				msgLevelColors[ spdlog::level::debug ]    = colorStr( color.cyan );
				msgLevelColors[ spdlog::level::info ]     = colorStr( color.green );
				msgLevelColors[ spdlog::level::warn ]     = colorStr( color.yellow_bold );
				msgLevelColors[ spdlog::level::err ]      = colorStr( color.red_bold );
				msgLevelColors[ spdlog::level::critical ] = colorStr( color.bold_on_red );
				msgLevelColors[ spdlog::level::off ]      = colorStr( color.reset );

#endif
			}
			// directly pulled from spdlog's to_strin_() in ansicolor_sink-inl.h (Just Changed Func Name To Avoid Any
			// Possible Name Collisions)
			template <typename Mutex> inline std::string colorStr( const basic_sv &sv )
			{
				return std::string( sv.data( ), sv.size( ) );
			}

		      protected:
			void sink_it_( const spdlog::details::log_msg &msg ) override
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

			void flush_( ) override
			{
				std::cout << std::flush;
			}

#ifdef PLATFORM_WINDOWS
			std::unordered_map<spdlog::level::level_enum, se_colors::WORD> msgLevelColors;
			se_colors::HANDLE                                              outHandle;
#else
			std::unordered_map<spdlog::level::level_enum, se_colors::basic_sv> msgLevelColors;
#endif  // PLATFORM_WINDOWS
		};
#include <spdlog/details/null_mutex.h>
#include <mutex>
		using std_split_sink_mt = std_split<std::mutex>;
		using std_split_sink_st = std_split<spdlog::details::null_mutex>;
	}  // namespace sinks
}  // namespace serenity