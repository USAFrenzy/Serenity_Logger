#pragma once

#include <serenity/Defines.h>
#include <serenity/Common.h>
#include <serenity/Sinks/SinkInfo.h>

#include <vector>
#include <memory>


namespace serenity
{
	namespace sinks
	{
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
			void                          ClearSinks( );
			const base_sink_info *        BasicInfo( );
			bool                          FindSink( SinkIterator first, SinkIterator last, const SinkType &value );


		      private:
			base_sink_info                m_sinkInfo;
			std::vector<spdlog::sink_ptr> dist_sink_sinks;
			std::vector<spdlog::sink_ptr> sinkVector;
		};
	}  // namespace sinks
}  // namespace serenity