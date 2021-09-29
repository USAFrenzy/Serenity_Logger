#include <serenity/Sinks/Sinks.h>
#include <serenity/Common.h>

#pragma warning( push, 0 )
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
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


	// ###################################################################################################################################################
	struct dist_sink_info : private spdlog::sinks::dist_sink_mt
	{
		// explicitly just wrapping the functionality from spdlogs dist sink class into a format used by this project
		explicit dist_sink_info( std::vector<SinkType> sinks ) : m_sinks( sinks ) { }

		void AddSink( SinkType sink )
		{
			m_sinks.emplace_back( sink );
		}

		void RemoveSink( SinkType sink )
		{
			m_sinks.erase( std::remove( m_sinks.begin( ), m_sinks.end( ), sink ), m_sinks.end( ) );
		}

		void SetSinks( std::vector<SinkType> sinks )
		{
			m_sinks = std::move( sinks );
		}

		std::vector<SinkType> GetSinks( )
		{
			return m_sinks;
		}

		void CreateSinkHandles( )
		{
			dist_sink_info temp( this->m_sinks );
			// auto     dist_sinks = toLoggerInfo( temp );
			// m_sinkHandle.CreateSink( dist_sinks );
		}


	      private:
		std::shared_ptr<spdlog::sink_ptr> dist_sink_ptr;
		std::vector<SinkType>             m_sinks;
		Sink                              m_sinkHandle;
	};


	logger_info base_sink_info::dist_to_logger_info( dist_sink_info *convertFrom )
	{
		logger_info tmp = { };
		tmp.flushLevel  = LoggerLevel::trace;
		tmp.level       = LoggerLevel::trace;
		return tmp;
	}


	// ###################################################################################################################################################

	void Sink::CreateSink( base_sink_info &infoStruct )
	{
		// sink vector doesn't contain console sinks -> has a file handle
		// Still set hasFileHandle = true in individual file sinks if it happens to contain a console sink & file sink
		// Kind Of Easier To Do This Than Explicity Search For Sinks That Have A File Handle In The Console Sinks
		if( !( ( find_sink( m_sinkInfo.sinks.begin( ), m_sinkInfo.sinks.begin( ), SinkType::stdout_color_mt ) ) &&
		       ( find_sink( m_sinkInfo.sinks.begin( ), m_sinkInfo.sinks.begin( ), SinkType::stderr_color_mt ) ) ) )
		{
			m_sinkInfo.hasFileHandle = true;
		}

		if( find_sink( m_sinkInfo.sinks.begin( ), m_sinkInfo.sinks.begin( ), SinkType::dist_sink_mt ) ) {
			// if dist sink is found in the list, copy contents to dist_sinks and create a temporary dist_sink_info onject
			std::vector<SinkType> dist_sinks = infoStruct.sinks;
			dist_sink_info        temp( dist_sinks );


			// if dist_sink has been found AND dist_sink_info isnt empty ->
			// - set a temp vector = to infostructs sink vector
			// - ignore dist_sink in infostruct vector when creating sinks initially
			// - then set the dist sink's vector = to sinkVector and create the sink

			if( m_sinkInfo.dist_sink != nullptr ) {
			}
		}

		for( auto const &sink : infoStruct.sinks ) {
			switch( sink ) {
				case SinkType::basic_file_mt:
					{
						if( infoStruct.base_info != nullptr ) {
							auto basic_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
							  infoStruct.base_info->logDir.path( ).string( ).append(
							    "\\" + infoStruct.base_info->logName ),
							  infoStruct.truncateFile );
							basic_logger->set_pattern( infoStruct.formatStr );
							sinkVector.emplace_back( std::move( basic_logger ) );
							m_sinkInfo.hasFileHandle = true;
						}
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
						// To Be implemented in derived sink class
					}
					break;
				case SinkType::rotating_mt:
					{
						if( ( infoStruct.rotate_sink != nullptr ) && ( infoStruct.base_info != nullptr ) ) {
							auto rotating_logger = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
							  infoStruct.base_info->logDir.path( ).string( ).append(
							    "\\" + infoStruct.base_info->logName ),
							  infoStruct.rotate_sink->maxFileNum, infoStruct.rotate_sink->maxFileSize,
							  infoStruct.rotate_sink->rotateWhenOpened );
							rotating_logger->set_pattern( infoStruct.formatStr );
							sinkVector.emplace_back( std::move( rotating_logger ) );
							m_sinkInfo.hasFileHandle = true;
						}
					}
					break;
				case SinkType::daily_file_sink_mt:
					{
						if( ( infoStruct.daily_sink != nullptr ) && ( infoStruct.base_info != nullptr ) ) {
							auto daily_logger = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
							  infoStruct.base_info->logDir.path( ).string( ).append(
							    "\\" + infoStruct.base_info->logName ),
							  infoStruct.daily_sink->hour, infoStruct.daily_sink->min,
							  infoStruct.truncateFile );
							daily_logger->set_pattern( infoStruct.formatStr );
							sinkVector.emplace_back( std::move( daily_logger ) );
							m_sinkInfo.hasFileHandle = true;
						}
					}
					break;
				case SinkType::dist_sink_mt:
					{
						if( ( infoStruct.dist_sink != nullptr ) && ( infoStruct.base_info != nullptr ) ) {
							auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_mt>( );
							// Implement A Way To Add Sinks To spdlog's version
							// - Initial Thoughts Are This:
							//	- Convert Sinks From SinkType To spdlog::sink_ptr
							//	- Create Sinks From Those
							//	- Call m_sinkInfo.dist_sink->GetSinks( ) here
							// dist_sink->set_sinks( m_sinkInfo.dist_sink->GetSinkHandles( ) );
							dist_sink->set_pattern( infoStruct.formatStr );
							sinkVector.emplace_back( std::move( dist_sink ) );
						}
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


	// void dist_sink_info::add_sink( ) { }

}  // namespace serenity