#include <serenity/Sinks/Sinks.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace serenity
{
	Sink::Sink( ) { }

	Sink::Sink( const Sink &sink )
	{
		m_sinkType = sink.m_sinkType;
		sinkVector = sink.sinkVector;
	}


	void Sink::set_sink_type( SinkType sinkType )
	{
		m_sinkType = sinkType;
	}

	Sink::SinkType Sink::get_sink_type( )
	{
		return m_sinkType;
	}


	void Sink::CreateSink( SinkType sink, logger_info &infoStruct )
	{
		sinkVector.clear( );
		switch( sink ) {
			case SinkType::basic_file_mt:
				{
					auto basic_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
					  infoStruct.logDir.path( ).string( ).append( "\\" + infoStruct.logName ), false );
					basic_logger->set_pattern( "[%T] [%l] %n: %v" );
					sinkVector.emplace_back( basic_logger );
				}
				break;
			case SinkType::stdout_color_mt:
				{
					auto console_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>( );
					console_logger->set_pattern( "%^[%T] %n: %v%$" );
					sinkVector.emplace_back( console_logger );
				}
				break;
			default:
				{
					throw std::runtime_error( "Invalid Sink Type\n" );
				}
				break;
		}
	}

}  // namespace serenity