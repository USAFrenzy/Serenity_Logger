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

	void Sink::SetSinks( std::vector<SinkType> sinks )
	{
		m_sinkInfo.sinks = sinks;
	}

	const std::vector<SinkType> Sink::GetSinkTypes( )
	{
		return m_sinkInfo.sinks;
	}

	// ###################################################################################################################################################

	dist_sink_info::dist_sink_info( std::vector<SinkType> sinks ) : m_sinks( sinks ) { }

	void dist_sink_info::AddSink( SinkType sink )
	{
		m_sinks.emplace_back( sink );
	}

	void dist_sink_info::RemoveSink( SinkType sink )
	{
		m_sinks.erase( std::remove( m_sinks.begin( ), m_sinks.end( ), sink ), m_sinks.end( ) );
	}

	void dist_sink_info::SetSinks( std::vector<SinkType> sinks )
	{
		m_sinks = std::move( sinks );
	}

	std::vector<SinkType> dist_sink_info::GetSinks( )
	{
		return m_sinks;
	}
	// ###################################################################################################################################################

	void Sink::CreateDistSink( base_sink_info &infoStruct )
	{
		// temporary storage of sinks/sink types
		auto prevSinkHandles = sinkVector;
		auto prevSinkTypes   = infoStruct.sinks;

		ClearSinks( );  // empty the sink handles and sink types
		infoStruct.sinks = infoStruct.dist_sink->GetSinks( );
		CreateSink( infoStruct );

		auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_mt>( );
		dist_sink->set_sinks( sinkVector );
		dist_sink->set_pattern( infoStruct.formatStr );

		// Reset To Original State And Move The Newly Created Distributed Sink Handle Into The Logger's Sinks Handle
		ClearSinks( );
		infoStruct.sinks = std::move( prevSinkTypes );
		sinkVector       = std::move( prevSinkHandles );
		// and finally add the newly created dist_sink handle to the sinkVector
		sinkVector.emplace_back( std::move( dist_sink ) );
	}
	// ###################################################################################################################################################

	void Sink::CreateSink( base_sink_info &infoStruct )
	{
		// sink vector doesn't contain console sinks -> has a file handle
		// Still set hasFileHandle = true in individual file sinks if it happens to contain a console sink & file sink
		// Kind Of Easier To Do This Than Explicity Search For Sinks That Have A File Handle In The Console Sinks
		if( !( ( FindSink( m_sinkInfo.sinks.begin( ), m_sinkInfo.sinks.begin( ), SinkType::stdout_color_mt ) ) &&
		       ( FindSink( m_sinkInfo.sinks.begin( ), m_sinkInfo.sinks.begin( ), SinkType::stderr_color_mt ) ) ) )
		{
			m_sinkInfo.hasFileHandle = true;
		}

		for( auto const &sink : infoStruct.sinks ) {
			switch( sink ) {
				case SinkType::basic_file_mt:
					{
						// For Readability
						auto              logDirPath = infoStruct.base_info.logDir.path( ).string( );
						auto              logName    = infoStruct.base_info.logName;
						file_helper::path filePath   = logDirPath + "/" + logName;
						filePath.make_preferred( );

						auto basic_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
						  filePath.string( ), infoStruct.truncateFile );
						basic_logger->set_pattern( infoStruct.formatStr );
						sinkVector.emplace_back( std::move( basic_logger ) );
						m_sinkInfo.hasFileHandle = true;
					}
					break;
				case SinkType::stdout_color_mt:
					{
						auto console_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>( );
						console_logger->set_pattern( infoStruct.formatStr );
						sinkVector.emplace_back( std::move( console_logger ) );
					}
					break;
				case SinkType::stderr_color_mt:
					{
						auto console_logger = std::make_shared<spdlog::sinks::stderr_color_sink_mt>( );
						console_logger->set_pattern( infoStruct.formatStr );
						sinkVector.emplace_back( std::move( console_logger ) );
					}
					break;
				case SinkType::std_split_mt:
					{
						// TODO: To Be implemented in derived sink class
					}
					break;
				case SinkType::rotating_mt:
					{
						auto              logPath  = infoStruct.base_info.logDir.path( ).string( );
						auto              fileName = infoStruct.base_info.logName;
						file_helper::path filePath = logPath + "/" + fileName;
						filePath.make_preferred( );

						auto rotating_logger = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
						  filePath.string( ), infoStruct.rotate_sink.maxFileNum,
						  infoStruct.rotate_sink.maxFileSize, infoStruct.rotate_sink.rotateWhenOpened );
						rotating_logger->set_pattern( infoStruct.formatStr );
						sinkVector.emplace_back( std::move( rotating_logger ) );
						m_sinkInfo.hasFileHandle = true;
					}
					break;
				case SinkType::daily_file_sink_mt:
					{
						auto              logPath  = infoStruct.base_info.logDir.path( ).string( );
						auto              fileName = infoStruct.base_info.logName;
						file_helper::path filePath = logPath + "/" + fileName;
						filePath.make_preferred( );

						auto daily_logger = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
						  filePath.string( ), infoStruct.daily_sink.hour, infoStruct.daily_sink.min,
						  infoStruct.truncateFile );
						daily_logger->set_pattern( infoStruct.formatStr );
						sinkVector.emplace_back( std::move( daily_logger ) );
						m_sinkInfo.hasFileHandle = true;
					}
					break;
				case SinkType::dist_sink_mt:
					{
						// This one is a little special just due to how CreateSinks() is implemented
						// and how dist_sink_mt works
						CreateDistSink( infoStruct );
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

	void Sink::ClearSinks( )
	{
		sinkVector.clear( );
	}

	const base_sink_info *Sink::BasicInfo( )
	{
		return &m_sinkInfo;
	}

	bool Sink::FindSink( SinkIterator first, SinkIterator last, const SinkType &value )
	{
		while( first != last ) {
			if( *first == value ) {
				return false;
			}
			++first;
		}
		return true;
	}

	std::vector<spdlog::sink_ptr> Sink::GetSinkHandles( )
	{
		return sinkVector;
	}


}  // namespace serenity