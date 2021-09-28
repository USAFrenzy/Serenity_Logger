#pragma once

#include <vector>
#include <memory>

#include <spdlog/sinks/sink.h>

namespace serenity
{
	// mt designated in case I add support for the single thread versions later on
	enum class SinkType
	{
		stdout_color_mt,
		stderr_color_mt,
		std_split_mt,  // error/fatal to stderr, else stdout -> to be implemented
		basic_file_mt,
		rotating_mt,
		daily_file_sink_mt,
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

	struct base_sink_info
	{
		std::string           formatStr         = "%^[%T] %n: %v%$";
		std::string           internalFormatStr = "%^[%L][%T] %n:%v%$";
		std::vector<SinkType> sinks;
		bool                  truncateFile { false };
		bool                  hasFileHandle { false };
		rotating_sink_info *  rotate_sink = nullptr;
		daily_sink_info *     daily_sink  = nullptr;
	};

	// forward declaration
	struct logger_info;

	class Sink
	{
	      public:
		using SinkIterator = std::vector<SinkType>::iterator;

		Sink( );
		~Sink( ) = default;
		Sink( const Sink &sink );

		void                        set_sinks( std::vector<SinkType> sinks );
		const std::vector<SinkType> get_sinks( );
		void                        CreateSink( logger_info &infoStruct );
		void                        clear_sinks( );
		const base_sink_info        basic_info( );
		bool                        find_sink( SinkIterator first, SinkIterator last, const SinkType &value );


	      private:
		base_sink_info m_sinkInfo;

	      public:
		std::vector<spdlog::sink_ptr> sinkVector;
	};


}  // namespace serenity