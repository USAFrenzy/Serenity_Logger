#pragma once

#include <vector>
#include <memory>

#include <spdlog/spdlog.h>

namespace serenity
{
	// fwd decl
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

	struct base_sink_info
	{
		std::string           formatStr = "%^[%T]%n:%v%$";
		std::vector<SinkType> sinks;
	};

	class Sink
	{
	      public:
		// Default Just For The Time being
		Sink( );
		~Sink( ) = default;
		Sink( const Sink &sink );

		void                  set_sinks( std::vector<SinkType> sinks );
		std::vector<SinkType> get_sinks( );
		void                  CreateSink( logger_info &infoStruct );
		void                  clear_sinks( );


	      private:
		base_sink_info m_sinkInfo;
		SinkType       m_sinkType = SinkType::unknown;

	      public:
		std::vector<spdlog::sink_ptr> sinkVector;
	};
}  // namespace serenity