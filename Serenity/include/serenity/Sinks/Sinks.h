#pragma once

#include <vector>
#include <memory>

#include <serenity/Common.h>


namespace serenity
{
	class Sink
	{
	      public:
		enum class SinkType
		{
			stdout_color_mt,
			basic_file_mt,
			unknown
		};
		// Default Just For The Time being
		Sink( );
		~Sink( ) = default;
		Sink( const Sink &sink );

		void     set_sink_type( SinkType sinkType );
		SinkType get_sink_type( );
		void     CreateSink( SinkType sink, logger_info &infoStruct );


	      public:
		std::vector<spdlog::sink_ptr> sinkVector;

	      private:
		SinkType m_sinkType = SinkType::unknown;
	};
}  // namespace serenity