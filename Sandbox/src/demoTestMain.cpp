#include <serenity/Logger.h>
#include <serenity/Utilities/Utilities.h>

#include <fstream>

void PrintReminder( );

/*
   After Dabbling With The std_split Sink, I've Noticed How Fun This Project Has Been - I Kind Of Would Like To See If I Can
   Change This Project Around To Be More Personally Written While Being Inspired By spdlog Rather Than A Wrapper/Re-implementing
   spdlog's functionality. Given That This Is A Learning Project For Me Anyways, I Can't Credit spdlog Enough For The Learning And
   Insights I've Gained Just From Dabbling In This Project And Looking Through What Their Team Has Put Together Here.
*/
/*
   My Thoughts On The Whole Struct-of-Structs implementation for initializing sink types was inspired by some of the early vulkan
   tutorials I've only recently begun (which differs from how spdlog does things) but I would also like to be able to do an
   initializer list for sinks for convenience's sake, like spdlog provides. I also would like to clean this section up to be more
   concise in the way things are populated and passed around. Another thing influenced by messing around with vulkan was the use
   of "validation layers". The Internal Library Logger is meant to mimic that functionality and provide a guard for the user (including
   myself) from misusing the library.

   As for the console colors, I want to see what colors are fully supported by terminal types and create structs for those as well
   as default to the most commonly used color types. spdlog has quite a number of color options already, but I would like to see
   if it's worth trying to expand on this or not and whether there's a clean way to do so. If possible with my skillset as a novice, I
   would like to see if I can provide a color tagging system for the pattern string, where if one so chooses, a section can be one
   color, while another section is a different color (or even every other word being a separate color - probably no real use case
   for this bit, but it could definitely prove to be fun I think). Another thing to add in tandem with this, is the pattern string
   itself. I would like to understand how spdlog is able to interpret the pattern strings so effieciently as I really like this
   use-case a lot and think it could come in handy for this project and for future projects (whether it's applied similarly as here or
   not).

   I am, so far, totally satisfied with my LogFileHelper and Utilities files, though I would like to clean these up and flesh them
   out as well. The LibLogger and the Logger classes will have to be changed here and there if I'm to switch over to not using spdlog's
   functions, but once some basic things are set (mainly the idea of "sinks"), I don't believe this will actually prove to be all that
   difficult to do. I really like spdlog's idea of sinks and have already done a slight wrapper around some of the ones I believe I'll
   use in the future, but I also want to expand on this area by adding an HTML and XML type of sink to create web-based logs (something
   that isn't present in spdlog) as well as possibly shred XML and log aspects from this and being able to redirect logs from a DB to
   other forms, such as XML, HTML, CSV, and JSON (Something that may be out-of-scope for this project, but was inspired by talking to
   my dad about what might be useful in DB logging).

   I DO believe that the sinks area will be the more time-consuming area of this, since they are basically the foundation to how the
   individual logger is setup. From what I saw, spdlog uses templates heavily for sinks, but since I'm rather new to templating things,
   I may steer clear from this route. Although, implementing the level-based logging functions in Logger.h/Logger-impl.h was heavily
   influenced by some of the concepts spdlog uses in this regard. It did also prove to be very flexible - so there's a chance I can use
   this as a templating learning opportunity and mess around with the idea of templating sinks.

   I definitely need to dig deeper into the whole "console_nullmutex", "console_mutex", etc to understand what this is doing as it
   seems to be key point to how spdlog determines multi-threaded or single-threaded sinks.  Given that I haven't done any real
   multi-threaded projects before, I'm wondering if I can't just simply default to multi-threaded construction for simplicity's sake
   (None of the use cases I can think of where I would be using this library would involve needing a highly performance based logging
   mechanism, so locking speeds really wouldn't be a deciding factor here - the issue is whether or not to allocate more memory for
   message buffers before flushing to disk or flushing more often). On top of which, multi-threading has always held a lot of interest
   for me, so I'm fairly certain that most of my projects in the future will use multi-threading in one form or another to learn more
   about it.

   As A last note, I'm unsure if I want to keep the whole interface pattern I initially started out with in this project for the Logger
   and LogFileHelper classes. It seems that what I initially wanted to achieve can be done using some atomic variables and writing
   conditional logic around that. The only upside I can see is the use for maybe not using the interfaces as a pure interface, but
   abstract interfaces, in which case I think I would be better off just having a base logger class and having the main logger and
   library logger classes inherit the very basic funtionality common to both from that abstract class

   Something else that is entirely out-of-scope for this project, but would be incredibly fun to achieve, is if I have enough time
   before life starts picking up again - I would like to see if I can extend this project into making a simple JSON or XML
   serializer/deserializer as that concept will come in very handy in the near future for saving states. The main reason for this
   project is to be added as a module in a larger project of a small game engine, so if I can take what is being learned here and apply
   it to some other aspects of that project, I would like to do so before things pull me away from being able to mess around with
   programming as often.
*/
int main( )
{
	PrintReminder( );
	using namespace serenity;


	file_helper::path const originalPath = file_helper::current_path( );

	auto logDirPath = originalPath;
	logDirPath /= "Logs";
	file_helper::directory_entry logDir { logDirPath };


	sinks::logger_info initInfo = { };
	initInfo.loggerName         = "Filesystem Logger";
	initInfo.logName            = "File_System.txt";
	initInfo.logDir             = logDir;

	std::vector<sinks::SinkType> dist_sink_handles;
	dist_sink_handles.emplace_back( sinks::SinkType::basic_file_mt );
	dist_sink_handles.emplace_back( sinks::SinkType::stdout_color_mt );
	sinks::dist_sink_info dist_sink( dist_sink_handles );

	sinks::base_sink_info sink_info = { };
	sink_info.base_info             = initInfo;
	sink_info.sinks.emplace_back( sinks::SinkType::dist_sink_mt );
	sink_info.dist_sink = &dist_sink;


	InternalLibLogger::EnableInternalLogging( );
	// Works As Intended [X]
	se_globals::SetGlobalLevel( LoggerLevel::trace );
	Logger logTwo( sink_info );

	logTwo.se_trace( "Message Before Changing Stuff In Internal Logger" );

	sinks::internal_logger_info changeOptions = { };
	changeOptions.sink_info.sinks.emplace_back( sinks::SinkType::basic_file_mt );
	changeOptions.sink_info.truncateFile = true;
	logTwo.ChangeInternalLoggerOptions( changeOptions );

	// se_globals::SetGlobalLevel( LoggerLevel::warning );  // Subsequent Calls Will Set Logger && Global Levels

	logTwo.SetLogLevel( LoggerLevel::trace );  // Still Can Set Level On A Logger-To-Logger Basis

	logTwo.se_info( "RenameLog() Section:" );
	auto spdDir     = logDirPath;
	auto spdLogDest = spdDir.string( ).append( "/RenamedSpdlogLog.txt" );
	logTwo.se_debug( "spdLogDest: {}\n", spdLogDest );
	logTwo.InternalLogger( )->SetLogLevel( LoggerLevel::info );
	logTwo.RenameLog( spdLogDest, false );
	logTwo.InternalLogger( )->SetLogLevel( LoggerLevel::trace );
	logTwo.se_debug( "BACK IN MAIN!\n" );

	logTwo.se_info( "Testing Swapping To New Log Instead Of Renaming...\n" );
	logTwo.WriteToNewLog( "\\WriteToNewLog/CreateNewLog.txt" );

	logTwo.se_debug( "File Path: [{}]", logTwo.FileHelperHandle( )->LogFilePath( ) );
	logTwo.se_debug( "Relative File Path: [{}]", logTwo.FileHelperHandle( )->RelativePathToLog( ) );
	logTwo.se_debug( "Log Directory: [{}]", logTwo.FileHelperHandle( )->LogDir( ).path( ) );
	logTwo.se_debug( "File Name: [{}]\n", logTwo.FileHelperHandle( )->LogName( ) );


#if 1
	// Just For Fun - Possibly Have A HTML Logger? Might Make A Sink For That =P
	sinks::base_sink_info secondSink = { };
	secondSink.base_info             = initInfo;
	secondSink.base_info.loggerName  = "Second_Logger";
	secondSink.base_info.logName     = "Secondary_Logger_Text.html";
	secondSink.sinks.emplace_back( sinks::SinkType::basic_file_st );
	secondSink.sinks.emplace_back( sinks::SinkType::stdout_color_st );
	Logger second_logger( secondSink );

	std::fstream file;
	file.open( second_logger.FileHelperHandle( )->LogFilePath( ) );
	file << "<!DOCTYPE html>\n<html>\n<body>\n";
	file.close( );
	for( int i = 0; i < 100; i++ ) {
		second_logger.se_info( "Test Message Number {} </br>", i + 1 );
	}
	file.open( second_logger.FileHelperHandle( )->LogFilePath( ) );
	file << "</body>\n</html>";
	file.close( );
#endif

	sinks::base_sink_info std_split = { };
	std_split.base_info             = initInfo;
	std_split.base_info.loggerName  = "Split_Console_Logger";
	std_split.sinks.emplace_back( sinks::SinkType::std_split_mt );
	Logger console( std_split );
	console.se_trace( "Trace" );
	console.se_debug( "Debug" );
	console.se_info( "Info" );
	console.se_warn( "warning" );
	console.se_error( "Error" );
	console.se_fatal( "Fatal" );

	// Next Step Now Is To Add More Sink Support, Wrap The Explicit Utilities Functions Into LogFileHelper Class Functions, And
	// Clean Up Any Messy Code. Then Write A Test Suite For Each Funtion And Call It Done =P
}


void PrintReminder( )
{
	auto day   = "15";
	auto month = "OCT";
	auto year  = "21";
	printf( "\n\t\t\t#############################\n" );
	printf( "\t\t\t#  Library Version: %s  #\n", serenity::se_utils::GetSerenityVerStr( ).c_str( ) );
	printf( "\t\t\t#  Date: %s %s %s          #\n", day, month, year );
	printf( "\t\t\t#############################\n\n" );
}