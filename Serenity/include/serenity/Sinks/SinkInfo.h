#pragma once


#include <serenity/Defines.h>
#include <serenity/Common.h>

#include <spdlog/sinks/dist_sink.h>

#include <iostream>

namespace serenity
{
	namespace sinks
	{
		// mt designated in case I add wrapper support for the single thread versions later on
		enum class SinkType
		{
			stdout_color_mt,
			stderr_color_mt,
			std_split_mt,  // error/fatal to stderr, else stdout -> to be implemented
			basic_file_mt,
			rotating_mt,
			daily_file_sink_mt,
			dist_sink_mt,
			unknown
		};

		struct rotating_sink_info
		{
			int  maxFileSize { 1024 * 1024 * 5 };
			int  maxFileNum { 5 };
			bool rotateWhenOpened { false };
		};
		struct daily_sink_info
		{
			int      hour { 0 };
			int      min { 0 };
			uint16_t maxFiles { 0 };
		};

		// class that separates it's messages based on message log level
		template <typename Mutex> class std_split : public spdlog::sinks::base_sink<Mutex>
		{
		      protected:
			void sink_it_( const spdlog::details::log_msg &msg ) override
			{
				if( msg.level > se_utils::ToMappedLevel( LoggerLevel::warning ) ) {
					// TODO: logic for warn, err, fatal to stderr
				}
				else {
					// TODO: logic for trace, info, debug to stdout
				}
				// If needed (very likely but not mandatory), the sink formats the message before sending it to its
				// final destination:
				spdlog::memory_buf_t formatted;
				spdlog::sinks::base_sink<Mutex>::formatter_->format( msg, formatted );
				std::cout << fmt::to_string( formatted );
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


		struct dist_sink_info : private spdlog::sinks::dist_sink_mt
		{
			explicit dist_sink_info( std::vector<SinkType> sinks );
			void                  AddSink( SinkType sink );
			void                  RemoveSink( SinkType sink );
			void                  SetSinks( std::vector<SinkType> sinks );
			std::vector<SinkType> GetSinks( );

		      private:
			std::vector<SinkType> m_sinks;
		};


		struct logger_info
		{
			std::string                  loggerName = DEFAULT_LOGGER_NAME;
			std::string                  logName    = DEFAULT_LOG;
			LoggerLevel                  level      = LoggerLevel::trace;
			LoggerLevel                  flushLevel = LoggerLevel::trace;
			file_helper::directory_entry logDir { file_helper::current_path( ) /= "Logs" };
		};

		struct base_sink_info
		{
			std::string           formatStr = "%^[%T] %n: %v%$";
			std::vector<SinkType> sinks;
			bool                  truncateFile { false };
			bool                  hasFileHandle { false };
			logger_info           base_info   = { };
			rotating_sink_info    rotate_sink = { };
			daily_sink_info       daily_sink  = { };
			dist_sink_info *      dist_sink;
		};

		struct internal_logger_info
		{
			internal_logger_info( );
			std::string                  loggerName = INTERNAL_DEFAULT_NAME;
			std::string                  logName    = INTERNAL_DEFAULT_LOG;
			LoggerLevel                  level      = LoggerLevel::trace;
			LoggerLevel                  flushLevel = LoggerLevel::trace;
			file_helper::directory_entry logDir { file_helper::current_path( ) /= "Logs/Internal" };
			std::string                  internalFormatStr = "%^[%L][%T] %n:%v%$";
			sinks::base_sink_info        sink_info         = { };
		};
	}  // namespace sinks
}  // namespace serenity
