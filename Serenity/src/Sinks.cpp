#include <serenity/Sinks/Sinks.h>
#include <serenity/Common.h>

#pragma warning( push, 0 )
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#pragma warning( pop )

namespace serenity
{
	Sink::Sink( )
	{
		m_sinkInfo = { };
		m_sinkType = SinkType::stdout_color_mt;
	}

	Sink::Sink( const Sink &sink )
	{
		m_sinkType = sink.m_sinkType;
		sinkVector = sink.sinkVector;
	}


	void Sink::set_sinks( std::vector<SinkType> sinks )
	{
		m_sinkInfo.sinks = sinks;
	}

	std::vector<SinkType> Sink::get_sinks( )
	{
		return m_sinkInfo.sinks;
	}

	/*
	 * Currently Hard-coding the format...
	 * Would Be Better To Use A Default Format And Just Call A Formatter Function, Possibly Similar To What spdlog Does, But Just
	 * Validates The Format (Maybe? Parsing Is A Royal Pain...) And Sets The Format Given Whether One Was Passed In Or Not:
	 * i.e. CreateLogger(sink, loggerInfo, stampFmt, isInternal);
	 *      CreateSink(sink, infoStruct, stampFmt); -> Check if empty, use default if so, otherwise possibly validate and then use
	 *      fmt passed in?
	 * std::optional is one route I could take, was trying that out on other functions early on and it seemed flexible enough
	 * EDIT: Accomplished this, more or less, by just having a sink_info type of struct with a format string and sink field
	 * - To Be honest, it's a bit more streamlined this route with the whole:
	 *	- CreateLogger(loggerInfo, isInternal) -> CreateSink(loggerInfo)->loggerInfo.sink_info->formatStr for the format stamp
	 * Still debating on the validation parsing deal (Should check to see if spdlog does this already anyways)
	 */

	void Sink::CreateSink( logger_info &infoStruct )
	{
		sinkVector.clear( );
		for( auto const &sink : infoStruct.sink_info.sinks ) {
			switch( sink ) {
				case SinkType::basic_file_mt:
					{
						auto basic_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
						  infoStruct.logDir.path( ).string( ).append( "\\" + infoStruct.logName ), false );
						basic_logger->set_pattern( infoStruct.sink_info.formatStr );
						sinkVector.emplace_back( std::move( basic_logger ) );
					}
					break;
				case SinkType::stdout_color_mt:
					{
						auto console_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>( );
						console_logger->set_pattern( infoStruct.sink_info.formatStr );
						sinkVector.emplace_back( std::move( console_logger ) );
					}
					break;
				case SinkType::rotating_mt:
					{
						auto rotating_logger = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
						  infoStruct.logDir.path( ).string( ).append( "\\" + infoStruct.logName ),
						  infoStruct.rotate_sink->maxFileNum, infoStruct.rotate_sink->maxFileSize,
						  infoStruct.rotate_sink->rotateWhenOpened );
						rotating_logger->set_pattern( infoStruct.sink_info.formatStr );
						sinkVector.emplace_back( std::move( rotating_logger ) );
					}
					break;
				case SinkType::daily_file_sink_mt:
					{
						auto daily_logger = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
						  infoStruct.logDir.path( ).string( ).append( "\\" + infoStruct.logName ),
						  infoStruct.daily_sink->hour, infoStruct.daily_sink->min,
						  infoStruct.daily_sink->truncate, infoStruct.daily_sink->truncate );
						daily_logger->set_pattern( infoStruct.sink_info.formatStr );
						sinkVector.emplace_back( std::move( daily_logger ) );
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


}  // namespace serenity