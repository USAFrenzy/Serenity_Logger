#include <serenity/Sinks/Sinks.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>

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

	/*
	 * Currently Hard-coding the format...
	 * Would Be Better To Use A Default Format And Just Call A Formatter Function, Possibly Similar To What spdlog Does, But Just
	 * Validates The Format (Maybe? Parsing Is A Royal Pain...) And Sets The Format Given Whether One Was Passed In Or Not:
	 * i.e. CreateLogger(sink, loggerInfo, stampFmt, isInternal);
	 *      CreateSink(sink, infoStruct, stampFmt); -> Check if empty, use default if so, otherwise possibly validate and then use
	 *      fmt passed in?
	 * std::optional is one route I could take, was trying that out on other functions early on and it seemed flexible enough
	 */
	void Sink::CreateSink( SinkType sink, logger_info &infoStruct )
	{
		sinkVector.clear( );
		switch( sink ) {
			case SinkType::basic_file_mt:
				{
					auto basic_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
					  infoStruct.logDir.path( ).string( ).append( "\\" + infoStruct.logName ), false );
					basic_logger->set_pattern( "[%T] [%l] %n: %v" );
					sinkVector.emplace_back( std::move(basic_logger ));
				}
				break;
			case SinkType::stdout_color_mt:
				{
					auto console_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>( );
					console_logger->set_pattern( "%^[%T] %n: %v%$" );
					sinkVector.emplace_back( std::move(console_logger ));
				}
				break;
				// Literally No Idea Why THIS is now throwing a compiling error where it wasn't before..
			case SinkType::rotating_mt:
				{
					auto rotating_logger = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
					  infoStruct.loggerName, infoStruct.logDir.path( ).string( ).append( "\\" + infoStruct.logName ),
					  infoStruct.rotate_sink->maxFileNum, infoStruct.rotate_sink->maxFileSize,
					  infoStruct.rotate_sink->rotateWhenOpened );
					rotating_logger->set_pattern( "[%T][%l]%n:%v" );
					sinkVector.emplace_back( std::move(rotating_logger ));
				}
				break;
			case SinkType::daily_file_sink_mt:
				{
					auto daily_logger = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
					  infoStruct.logDir.path( ).string( ).append( "\\" + infoStruct.logName ),
					  infoStruct.daily_sink->hour, infoStruct.daily_sink->min, infoStruct.daily_sink->truncate,
					  infoStruct.daily_sink->truncate );
					daily_logger->set_pattern( "[%T][%l]%n:%v" );
					sinkVector.emplace_back( std::move(daily_logger ));
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