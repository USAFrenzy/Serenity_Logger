#include <serenity/Sinks/Sinks.h>
#include <serenity/Common.h>

#pragma warning( push, 0 )
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#pragma warning( pop )

#include <algorithm>

namespace serenity
{
	Sink::Sink( )
	{
		m_sinkInfo = { };
	}

	Sink::Sink( const Sink &sink )
	{
		sinkVector = sink.sinkVector;
	}

	void Sink::set_sinks( std::vector<SinkType> sinks )
	{
		m_sinkInfo.sinks = sinks;
	}

	const std::vector<SinkType> Sink::get_sinks( )
	{
		return m_sinkInfo.sinks;
	}

	void Sink::CreateSink( logger_info &infoStruct )
	{
		// sink vector doesn't contain console sinks -> has a file handle
		// Still set hasFileHandle = true in individual file sinks if it happens to contain a console sink & file sink
		// Kind Of Easier To Do This Than Explicity Search For Sinks That Have A File Handle In The Console Sinks
		if( !( ( find_sink( m_sinkInfo.sinks.begin( ), m_sinkInfo.sinks.begin( ), SinkType::stdout_color_mt ) ) &&
		       ( find_sink( m_sinkInfo.sinks.begin( ), m_sinkInfo.sinks.begin( ), SinkType::stderr_color_mt ) ) ) )
		{
			m_sinkInfo.hasFileHandle = true;
		}

		for( auto const &sink : infoStruct.sink_info.sinks ) {
			switch( sink ) {
				case SinkType::basic_file_mt:
					{
						auto basic_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
						  infoStruct.logDir.path( ).string( ).append( "\\" + infoStruct.logName ),
						  infoStruct.sink_info.truncateFile );
						basic_logger->set_pattern( infoStruct.sink_info.formatStr );
						sinkVector.emplace_back( std::move( basic_logger ) );
						m_sinkInfo.hasFileHandle = true;
					}
					break;
				case SinkType::stdout_color_mt:
					{
						auto console_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>( );
						console_logger->set_pattern( infoStruct.sink_info.formatStr );
						sinkVector.emplace_back( std::move( console_logger ) );
					}
					break;
				case SinkType::stderr_color_mt:
					{
						auto console_logger = std::make_shared<spdlog::sinks::stderr_color_sink_mt>( );
						console_logger->set_pattern( infoStruct.sink_info.formatStr );
						sinkVector.emplace_back( std::move( console_logger ) );
					}
					break;
				case SinkType::std_split_mt:
					{
						// To Be implemented in derived sink class
					}
					break;
				case SinkType::rotating_mt:
					{
						auto rotating_logger = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
						  infoStruct.logDir.path( ).string( ).append( "\\" + infoStruct.logName ),
						  infoStruct.sink_info.rotate_sink->maxFileNum,
						  infoStruct.sink_info.rotate_sink->maxFileSize,
						  infoStruct.sink_info.rotate_sink->rotateWhenOpened );
						rotating_logger->set_pattern( infoStruct.sink_info.formatStr );
						sinkVector.emplace_back( std::move( rotating_logger ) );
						m_sinkInfo.hasFileHandle = true;
					}
					break;
				case SinkType::daily_file_sink_mt:
					{
						auto daily_logger = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
						  infoStruct.logDir.path( ).string( ).append( "\\" + infoStruct.logName ),
						  infoStruct.sink_info.daily_sink->hour, infoStruct.sink_info.daily_sink->min,
						  infoStruct.sink_info.truncateFile );
						daily_logger->set_pattern( infoStruct.sink_info.formatStr );
						sinkVector.emplace_back( std::move( daily_logger ) );
						m_sinkInfo.hasFileHandle = true;
					}
					break;
				default:
					{
						throw std::runtime_error( "Invalid Sink Type\n" );
					}
					break;
			}
		}
	}

	void Sink::clear_sinks( )
	{
		sinkVector.clear( );
		m_sinkInfo.sinks.clear( );
	}

	const base_sink_info Sink::basic_info( )
	{
		return m_sinkInfo;
	}

	bool Sink::find_sink( SinkIterator first, SinkIterator last, const SinkType &value )
	{
		while( first != last ) {
			if( *first == value ) {
				return false;
			}
			++first;
		}
		return true;
	}
}  // namespace serenity