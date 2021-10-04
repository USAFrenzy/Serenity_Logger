
#include <serenity/Sinks/Sinks.h>
#include <serenity/Sinks/StdSplitSink.h>
#include <serenity/Logger.h>  // For Setting The sinksLogger to the logger's instance of the internal logger

#pragma warning( push, 0 )
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#pragma warning( pop )

#include <algorithm>

namespace serenity
{
	namespace sinks
	{
		std::shared_ptr<InternalLibLogger> Sink::sinksLogger;

		Sink::Sink( )
		{
			sinksLogger = Logger::InternalLogger( );
			m_sinkInfo  = { };
			if( sinksLogger != nullptr ) sinksLogger->trace( "Sink Object Created" );
		}

		Sink::Sink( const Sink &sink )
		{
			if( sinksLogger != nullptr ) sinksLogger->trace( "Copying Sinks..." );
			sinkVector = sink.sinkVector;
			if( sinksLogger != nullptr ) sinksLogger->info( "Copying Sinks Was Successful" );
		}

		void Sink::SetSinks( std::vector<SinkType> sinks )
		{
			if( sinksLogger != nullptr ) sinksLogger->trace( "Currently Setting Sinks..." );
			m_sinkInfo.sinks = sinks;
			if( sinksLogger != nullptr ) sinksLogger->info( "Sinks Were Successfully Set" );
		}

		const std::vector<SinkType> Sink::GetSinkTypes( )
		{
			if( sinksLogger != nullptr ) {
				sinksLogger->trace( "Retrieving List Of SinkTypes" );
				sinksLogger->trace( "Current List Of SinkTypes Contains {} Sinks", m_sinkInfo.sinks.size( ) );
			}
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


		void Sink::CreateSink( base_sink_info &infoStruct )
		{
			for( auto const &sink : infoStruct.sinks ) {
				switch( sink ) {
					case SinkType::basic_file_mt:
						{
							if( sinksLogger != nullptr )
								sinksLogger->trace( "Creating \"basic_file_mt\" Sink..." );

							auto              logDirPath = infoStruct.base_info.logDir.path( ).string( );
							auto              logName    = infoStruct.base_info.logName;
							file_helper::path filePath   = logDirPath + "/" + logName;
							filePath.make_preferred( );

							if( sinksLogger != nullptr ) {
								sinksLogger->trace(
								  "File Path For \"basic_file_mt\" Sink Has Been Set To "
								  "[{}]",
								  filePath );
								sinksLogger->trace(
								  "\"basic_file_mt\" Sink's Truncate Value Has Been Set To: [{}]",
								  infoStruct.truncateFile );
							}
							auto basic_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
							  filePath.string( ), infoStruct.truncateFile );

							if( sinksLogger != nullptr )
								sinksLogger->info(
								  "\"basic_file_mt\" Sink Has Been Successfully Created" );

							basic_sink->set_pattern( infoStruct.formatStr );

							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"basic_file_mt\" Sink's Format String has Been Set To: [{}]",
								  infoStruct.formatStr );

							sinkVector.emplace_back( std::move( basic_sink ) );

							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"basic_file_mt\" Sink Has Been Moved To Sinks List" );
						}
						break;
					case SinkType::stdout_color_mt:
						{
							auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>( );
							if( sinksLogger != nullptr )
								sinksLogger->info(
								  "\"stdout_color_mt\" Sink Has Been Successfully Created " );
							console_sink->set_pattern( infoStruct.formatStr );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"stdout_color_mt\" Sinks' Format Pattern Has Been Set To: [{}]",
								  infoStruct.formatStr );
							sinkVector.emplace_back( std::move( console_sink ) );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"stdout_color_mt\" Sink Has Been Moved To Sinks List" );
						}
						break;
					case SinkType::stderr_color_mt:
						{
							auto err_console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>( );
							if( sinksLogger != nullptr )
								sinksLogger->info(
								  "\"stderr_color_mt\" Sink Has Been Successfully Created " );
							err_console_sink->set_pattern( infoStruct.formatStr );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"stderr_color_mt\" Sinks' Format Pattern Has Been Set To: [{}]",
								  infoStruct.formatStr );
							sinkVector.emplace_back( std::move( err_console_sink ) );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"stderr_color_mt\" Sink Has Been Moved To Sinks List" );
						}
						break;
					case SinkType::std_split_mt:
						{
							auto std_split_sink = std::make_shared<serenity::sinks::std_split_sink_mt>( );
							if( sinksLogger != nullptr )
								sinksLogger->info(
								  "\"std_split_mt\" Sink Has Been Successfully Created " );
							std_split_sink->set_pattern( infoStruct.formatStr );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"std_split_mt\" Sinks' Format Pattern Has Been Set To: [{}]",
								  infoStruct.formatStr );
							sinkVector.emplace_back( std::move( std_split_sink ) );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"std_split_mt\" Sink Has Been Moved To Sinks List" );
						}
						break;
					case SinkType::rotating_mt:
						{
							auto              logPath  = infoStruct.base_info.logDir.path( ).string( );
							auto              fileName = infoStruct.base_info.logName;
							file_helper::path filePath = logPath + "/" + fileName;
							filePath.make_preferred( );

							if( sinksLogger != nullptr ) {
								sinksLogger->trace(
								  "File Path For \"rotating_mt\" Sink Has Been Set To: "
								  "[{}]",
								  filePath );
								sinksLogger->trace(
								  "\"rotating_mt\" Sink's Max Number Of Files Has Been Set To: [{}]",
								  infoStruct.rotate_sink.maxFileNum );
								sinksLogger->trace(
								  "\"rotating_mt\" Sink's Max File Size Has Been Set To: [{}]",
								  infoStruct.rotate_sink.maxFileSize );
								sinksLogger->trace(
								  "\"rotating_mt\" Sink's Rotate When Opened Value Has Been Set To: "
								  "[{}]",
								  infoStruct.rotate_sink.rotateWhenOpened );
							}
							auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
							  filePath.string( ), infoStruct.rotate_sink.maxFileNum,
							  infoStruct.rotate_sink.maxFileSize, infoStruct.rotate_sink.rotateWhenOpened );
							if( sinksLogger != nullptr )
								sinksLogger->info(
								  "\"rotating_mt\" Sink Has Been Successfully Created" );
							rotating_sink->set_pattern( infoStruct.formatStr );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"rotating_mt\" Sink's Format Pattern Has Been Set To: [{}]",
								  infoStruct.formatStr );
							sinkVector.emplace_back( std::move( rotating_sink ) );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"rotating_mt\" Sink Has Been Moved To Sinks List" );
						}
						break;
					case SinkType::daily_file_sink_mt:
						{
							auto              logPath  = infoStruct.base_info.logDir.path( ).string( );
							auto              fileName = infoStruct.base_info.logName;
							file_helper::path filePath = logPath + "/" + fileName;
							filePath.make_preferred( );

							if( sinksLogger != nullptr ) {
								sinksLogger->trace(
								  "File Path For \"daily_file_sink_mt\" Sink Has Been Set To: "
								  "[{}]",
								  filePath );
								sinksLogger->trace(
								  "\"daily_file_sink_mt\" Sink's Time Has Been Set To: [{0}:{1}]",
								  infoStruct.daily_sink.hour, infoStruct.daily_sink.min );
								sinksLogger->trace(
								  "\"daily_file_sink_mt\" Sink's Truncate Value Has Been Set To: [{}]",
								  infoStruct.truncateFile );
							}
							auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
							  filePath.string( ), infoStruct.daily_sink.hour, infoStruct.daily_sink.min,
							  infoStruct.truncateFile );

							if( sinksLogger != nullptr )
								sinksLogger->info(
								  "\"daily_file_sink_mt\" Sink Has Been Successfully Created" );
							daily_sink->set_pattern( infoStruct.formatStr );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"daily_file_sink_mt\" Sink's Format Pattern Has Been Set To: [{}]",
								  infoStruct.formatStr );
							sinkVector.emplace_back( std::move( daily_sink ) );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"daily_file_sink_mt\" Sink Has Been Moved To Sinks List" );
						}
						break;
					case SinkType::dist_sink_mt:
						{
							// This one is a little special just due to how CreateSinks() is implemented
							// and how dist_sink_mt works

							// temporary storage of sinks/sink types
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "Storing Old Sink List And Sink Types In Temp Variables" );
							auto prevSinkHandles = sinkVector;
							auto prevSinkTypes   = infoStruct.sinks;
							// temporary storage of sinks/sink types
							if( sinksLogger != nullptr )
								sinksLogger->trace( "Clearing Old Sinks List And Sink Types" );
							ClearSinks( );  // empty the sink handles and sink types
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "Setting New Sink List Set To \"dist_sink_mt\" Sinks Sink List" );
							infoStruct.sinks = infoStruct.dist_sink->GetSinks( );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "Creating Sink Handles For The \"dist_sink_mt\" Sink" );
							CreateSink( infoStruct );  // for the dist sinks that were just set
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "Successfully Created All Sink Handles For \"dist_sink_mt\" Sink" );

							auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_mt>( );
							if( sinksLogger != nullptr )
								sinksLogger->info(
								  "\"dist_sink_mt\" Sink Has Been Successfully Created" );

							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "Setting The Sink Handles To The Newly Created \"dist_sink_mt\" "
								  "Sink" );
							dist_sink->set_sinks( sinkVector );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"dist_sink_mt\" Sink Now Holds A Reference To Sink Handles" );

							// Reset To Original State And Move The Newly Created Distributed Sink Handle
							// Into The Logger's Sinks Handle
							ClearSinks( );

							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "Resetting Sinks List And Sink Types To Originals" );
							infoStruct.sinks = std::move( prevSinkTypes );
							sinkVector       = std::move( prevSinkHandles );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "Original Sinks List And Sink Types Have Been Restored" );

							// and finally add the newly created dist_sink handle to the sinkVector
							sinkVector.emplace_back( std::move( dist_sink ) );
							if( sinksLogger != nullptr )
								sinksLogger->trace(
								  "\"dist_sink_mt\" Sink Has Been Moved To Sinks List" );
						}
						break;
					default:
						{
							sinksLogger->error( "Invalid Sink Type\n" );
						}
						break;
				}
			}
		}

		void Sink::ClearSinks( )
		{
			if( sinksLogger != nullptr ) sinksLogger->trace( "Clearing Sinks List And Sink Types" );
			sinkVector.clear( );
			m_sinkInfo.sinks.clear( );
			if( sinksLogger != nullptr ) sinksLogger->trace( "Sinks List And Sink Types Have Been Cleared" );
		}

		const base_sink_info *Sink::BasicInfo( )
		{
			if( sinksLogger != nullptr ) sinksLogger->trace( "Retrieving Basic Info..." );
			return &m_sinkInfo;
		}

		bool Sink::FindSink( SinkIterator first, SinkIterator last, const SinkType &value )
		{
			if( sinksLogger != nullptr ) sinksLogger->trace( "Searching For Specified Sink..." );
			while( first != last ) {
				if( *first == value ) {
					if( sinksLogger != nullptr ) sinksLogger->info( "Specified Sink Has Been Found" );
					return true;
				}
				++first;
			}
			if( sinksLogger != nullptr ) sinksLogger->info( "Specified Sink Was Not Found" );
			return false;
		}

		std::vector<spdlog::sink_ptr> Sink::GetSinkHandles( )
		{
			if( sinksLogger != nullptr ) {
				sinksLogger->trace( "Retrieving Sink List" );
				sinksLogger->trace( "Sink List Contains [{}] Sinks", sinkVector.size( ) );
			}
			return sinkVector;
		}
	}  // namespace sinks
}  // namespace serenity