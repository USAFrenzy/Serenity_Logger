
#include "serenity/Logger.hpp"
// #include "serenity/WIP/temp2.hpp"

#define LOGGER_MESSAGES_SB   0
#define TEST_ASSERT          0
#define LOGGER_FILESYSTEM_SB 1

int main( )
{
#if LOGGER_MESSAGES_SB

	serenity::file_helper::path logName1 = "MacroLog.txt";
	auto logDirPath1                     = serenity::file_helper::current_path( ) /= "Logs";
	serenity::LogFileHelper logInfo1(logDirPath1, logName1);
	Logger logOne("Macro Test Logger", &logInfo1, LoggerLevel::trace);

	SE_INTERNAL_INFO("Hello From The True Library!\n- Serenity Library Info: \n\t- Version: {}",
			 serenity::GetSerenityVerStr( ));
	SE_INTERNAL_TRACE("This Is A General Trace Message - Time To Iterate Through The Log Levels!");
	std::string A = "Hello A";
	SE_INTERNAL_DEBUG("Oh Hey, A Debug Message! Variable Substitution for 'A' should be 'Hello A': {}", A);
	SE_INTERNAL_WARN("What Do Ya Know? It's A Warning Message! Best Take Care With These!");
	SE_INTERNAL_ERROR("Say Hello To The Darkness - This Is An Error Message. You Done Fucked Up");
	SE_INTERNAL_FATAL("Last But Not Least, This Is A Fatal Message - Your Application Is Crashing My "
			  "Friend...");
	// clang-format off
	#if TEST_ASSERT
		SE_INTERNAL_INFO("To Help With Those Pesky Fatal Messages, We Have The Assert Messages!\n- For Instance:\n");
		int a = 5;
		int b = 76;
		SE_INTERNAL_ASSERT((a > b), "ACTUAL VALUES:\na: {}\nb:{}", a, b);
	#endif
	// clang-format on
#endif

#if LOGGER_FILESYSTEM_SB

	Logger logTwo("Filesystem Logger", "File_System.txt", LoggerLevel::trace);

	SE_INFO("Now Just Testing That The Logger Initialized Name Got Passed Through To The Logs");
	SE_TRACE("Working With File Systems Now\n\n");

	SE_INFO("\nCurrent Path: {} \nLog Dir: {} \nLog Path: {} \nLog Name: {}\n",
		logTwo.GetCurrentDir( ),
		logTwo.GetDirPath( ),
		logTwo.GetFilePath( ),
		logTwo.GetFileName( ));

	SE_TRACE("Now Testing Swapping Log Names:");

	logTwo.RenameFile("New_Log_Test.txt");
	SE_INFO("New File Name: {}\n", logTwo.GetFileName( ));

	SE_INFO("Previous Working Dir: {}", logTwo.GetCurrentDir( ))
	serenity::file_helper::path pathToChangeToOne = "C:\\Users\\mccul\\Desktop\\Logging Project\\build";
	serenity::file_helper::path pathToChangeToTwo = "C:\\Users\\mccul\\Desktop";

	logTwo.ChangeDir(pathToChangeToOne);
	SE_INFO("New Working Dir: {}\n", logTwo.GetCurrentDir( ));
	SE_INFO("\nCurrent Path: {} \nLog Dir: {} \nLog Path: {} \nLog Name: {}\n",
		logTwo.GetCurrentDir( ),
		logTwo.GetDirPath( ),
		logTwo.GetFilePath( ),
		logTwo.GetFileName( ));

	logTwo.ChangeDir(pathToChangeToTwo);
	SE_INFO("New Working Dir: {}\n", logTwo.GetCurrentDir( ));
	SE_INFO("\nCurrent Path: {} \nLog Dir: {} \nLog Path: {} \nLog Name: {}\n",
		logTwo.GetCurrentDir( ),
		logTwo.GetDirPath( ),
		logTwo.GetFilePath( ),
		logTwo.GetFileName( ));

	SE_INFO("Just Visual Representation Of Path Components:{}\n",
		 logTwo.PathComponents_Str(pathToChangeToOne));
	logTwo.RenameFile("lol.jpg.txt");
	auto curD = logTwo.GetFilePath();
	SE_INFO("\n\nSecond Visual Representation Of Path Components:{}\n",
		logTwo.PathComponents_Str(curD) );
#endif
}
