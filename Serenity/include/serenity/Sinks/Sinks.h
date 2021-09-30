#pragma once

#include <serenity/Defines.h>
#include <serenity/Common.h>

#include <vector>
#include <memory>

#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/dist_sink.h>

namespace serenity
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

	struct dist_sink_info : private spdlog::sinks::dist_sink_mt
	{
		// explicitly just wrapping the functionality from spdlogs dist sink class into a format used by this project
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
		std::string           formatStr         = "%^[%T] %n: %v%$";
		std::string           internalFormatStr = "%^[%L][%T] %n:%v%$";
		std::vector<SinkType> sinks;
		bool                  truncateFile { false };
		bool                  hasFileHandle { false };
		logger_info           base_info   = { };
		rotating_sink_info    rotate_sink = { };
		daily_sink_info       daily_sink  = { };
		dist_sink_info *      dist_sink   = { };
	};


	class Sink
	{
	      public:
		using SinkIterator = std::vector<SinkType>::iterator;

		Sink( );
		~Sink( ) = default;
		Sink( const Sink &sink );

		void                          SetSinks( std::vector<SinkType> sinks );
		const std::vector<SinkType>   GetSinkTypes( );
		std::vector<spdlog::sink_ptr> GetSinkHandles( );
		void                          CreateSink( base_sink_info &infoStruct );
		void                          CreateDistSink( base_sink_info &infoStruct );
		void                          ClearSinks( );
		const base_sink_info *        BasicInfo( );
		bool                          FindSink( SinkIterator first, SinkIterator last, const SinkType &value );


	      private:
		base_sink_info                m_sinkInfo;
		std::vector<spdlog::sink_ptr> dist_sink_sinks;
		std::vector<spdlog::sink_ptr> sinkVector;
	};


}  // namespace serenity