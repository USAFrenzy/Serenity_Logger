
#include "serenity/Logger.hpp"
#include <iostream>

int main( )
{
	PrintHello( );
	Logger log;
	// LoggerInfo loggerInfo = { };
	log.Init("Logs/General_Log.txt", LoggerLevel::trace);
	SE_INTERNAL_INFO("Hello From The True Library!\n- Serenity Library Info: \n\t- Version: {}",
			 serenity::GetSerenityVerStr( ));
	SE_INTERNAL_TRACE("This Is A General Trace Message - Time To Iterate Through The Log Levels!");
	std::string A = "Hello A";
	SE_INTERNAL_DEBUG("Oh Hey, A Debug Message! Variable Substitution for 'A' should be 'Hello A': {}", A);
	SE_INTERNAL_WARN("What Do Ya Know? It's A Warning Message! Best Take Care With These!");
	SE_INTERNAL_ERROR("Say Hello To The Darkness - This Is An Error Message. You Done Fucked Up");
	SE_INTERNAL_FATAL("Last But Not Least, This Is A Fatal Message - Your Application Is Crashing My "
			  "Friend...");
	SE_INTERNAL_INFO("To Help With Those Pesky Fatal Messages, We Have The Assert Messages!\n- For Instance: "
			 "\n");
	int a = 5;
	int b = 76;
	SE_INTERNAL_ASSERT((a > b), "ACTUAL VALUES:\na: {}\nb:{}", a, b);
}