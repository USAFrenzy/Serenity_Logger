#pragma once

#include <vector>
#include <memory>

#pragma warning( push, 0 )
#include <spdlog/spdlog.h>
#pragma warning( pop )

namespace serenity
{
	// forward declaration
	struct logger_info;

	// mt designated in case I add support for the single thread versions later on
	enum class SinkType
	{
		stdout_color_mt,
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

	class Sink
	{
	      public:
		Sink( );
		~Sink( ) = default;
		Sink( const Sink &sink );

		void                        set_sinks( std::vector<SinkType> sinks );
		const std::vector<SinkType> get_sinks( );
		void                        CreateSink( logger_info &infoStruct );
		void                        clear_sinks( );
		const SinkType              sink_type( );
		const base_sink_info        basic_info( );


	      private:
		base_sink_info m_sinkInfo;
		SinkType       m_sinkType = SinkType::unknown;

	      public:
		std::vector<spdlog::sink_ptr> sinkVector;
	};
}  // namespace serenity