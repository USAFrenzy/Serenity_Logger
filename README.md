[![Build status](https://ci.appveyor.com/api/projects/status/3yywxhc5bc4gq6hr/branch/dev?svg=true)](https://ci.appveyor.com/project/USAFrenzy/serenity-logger/branch/dev)


### For all current details about the library, check out the documentation [HERE](https://usafrenzy.github.io/Serenity_Logger/) [ DEPRECATED - SEE THIRD POINT BELOW ] ###

- NOTE: THE DOCUMENTATION IS NOT UP-TO-DATE WITH THE CURRENT PROGRESS OF SERENITY AT THE MOMENT <br>
- The READ-ME is slightly more up-to-date and can still be used as a quick reference.
- ### For the work-in-progress documentation that is being written for the current status of this repo, please visit  [HERE](https://usafrenzy.github.io/Serenity_Custom_Docs/) ###

<br><br>


--------------------------------------
<h1 align="center"> Requirements And Features Explicitly Used</h1>


## For Out Of The Box Usage ##
- C++20
  - C++20 features used
    - the new method of lambda ```this``` capture
    - using enum
    - template constraints with ```requires``` and ```concept``` usage
    - ```<source_location>``` and, if ```USE_STD_FORMAT``` is enabled, ```<format>```
    - large use of ```std::remove_cvref_t```
    - jthread and stop_tokens
- CMake
- MSVC toolchain if using the option ```USE_STD_FORMAT``` (Currently only MSVC has an implementation of libfmt).
  - This library will be updated to support clang and g++ once libfmt is <br>
  implemented on those compilers.
- ```fmtlib``` by Viktor Zverovich if using the option ```UST_FMTLIB```
- If not using the above two options, you're all set! This logging framework includes a stripped down but fully<br>functional
  version of the formatting interface via the ```ArgFormatter.h``` header file and is enabled by default<br>if the other options
  aren't defined.
  - The only caveats to the built-in version is that there's no type erasure of arguments captured<br>and no utf-8 support as of yet.
- With the addition of the ```ArgFormatter.h``` header, the goal in the future is to fully drop the requirements<br>down to C++17
  and only require C++20 when ```USE_STD_FORMAT``` has been defined, however, for now anyways, <br>C++20 is the default requirement.

--------------------------------------
<h1 align="center"> Motivation </h1>

<p align="center">Starting from commit 037a0d2, which was relatively early on, this project shifted from being solely a wrapper to one of being a home-brewed logging framework.
As for the motivation, one often doesn't think of logging as a fun endeavor, but my goal here is to have just that - a logger that's highly
efficient, fast, customizable, fun, and easy to use. I wanted to make a logger that had several color choices to pick from
(More than a user may ever need, while still modularizing components for choices a user may not want). I plan on extending
this to cover some basic RGB color combos as well and add user defined formatting call backs and a way to add user defined flags. </p>

<p align="center">This project is honestly more of a learning project meant for practicing. This is for a multitude of different reasons - the major
 one being that my main aim is to have a project that was completed from start to finish while learning more on performance oriented designs and and some
minor stepping stones into the world of templates (before this project, I had never written anything templated). </p>

<p align="center">For now, my goal is simply to achieve a multi-threaded and single-threaded version of a console target, color console target,
file target, rotating file target, some form of an XML target capable of shredding, and lastly the possibility of a very basic
HTML target. I would like to add many more built-in capabilities than just these, but I feel that this would be a strong start
for this project. I did at one point try to use function pointers for formatting functions, however that route ended up being
slower than the standard I'm aiming to beat - so for now these are in the form of formatting structs.</p>

--------------------------------------
<h1 align="center"> Basic Overview </h1>

<p align="center">Currently, this library only provides classes to allow for basic file logging, file sized based rotational logging, and terminal logging.
Before going forward, all built-in logging classes inherit from the base class, TargetBase. This base class holds some common functions
and settings used by all the derived targets. </p>


--------------------------------------------------------------------------------------
<p align="left"> The TargetBase class exposes the following:

**The TargetBase class can be found in the ```<serenity/Targets/Target.h>``` header.**
- The ability to set a user specified flush policy
  - This determines how often log messages should be flushed to disk
  - Current policies control a wall-clock time interval based flushing system and a
    message level threshold based flushing system.
    - The time interval policy utilizes a background thread to keep track of elapsed
      time <br>
      and will protect against data races when flushing the file to disk.
    - Therefore, this policy will block log messages only when the time elapsed is
      greater <br>
      than or equal to the interval limit that was set by calling ```Flush()```.
- Current flush policies expanded:
  - If interval based, can flush the file to disk every 5 seconds for example (time can be customized).
  - If message threshold based, can flush the file to disk every time a message with a level of warning or <br>
  higher (warning, error, and fatal) is logged as an example (level threshold can be customized).
- The ability to set a user-specified format pattern.
  - If no prepended pattern is wanted, use ```SetPattern()``` with an empty string argument.
  - Currently supported flags can be found in the ```<serenity/Common.h>``` header under the ```allValidFlags``` <br>
  look up table and is a part of the SERENITY_LUTS namespace.
- The ability to reset the format pattern to its default pattern.
- The default pattern is in the form of:
  - ```[Message Level] wkday_name DDMMMYY HH:MM:SS [Logger]: message to log + end_of_line```
  - Example: ```[Trace] Mon 25Mar22 13:23:02 [Example_Logger]: message to log \n```
- Ability to set the log level via ```SetLogLevel()``` which determines what messages are actually logged.
- Ability to enable/disable writing to a buffer instead of a file during the process.
  - The buffer reserves 512KB but can dynamically resized if contents get too large.
  - The buffer is flushed to disk when ```Flush()``` is explicitly called or when the
    destructor is called.
  - Note: this buffer is different from the buffer used by the file handle itself in derived classes.
- Ability to set the logger's name via ```SetLoggerName()```.
- This class is also what currently holds the explicit logging funtions:
  - ```Trace()```, ```Info()```, ```Debug()```, ```Warn()```, ```Error()```, and ```Fatal()```
- Exposes the functions ```PrintMessage()```, ```PolicyFlushOn()```, ```MsgFmt()```, ```MsgInfo()```
  to derived <br>
  classes to be re-implemented by the user.
</p>


--------------------------------------
<p align="left"> The FileTarget class is capable of the following:

**File logging occurs via the FileTarget class found in the ```<serenity/Targets/FileTarget.h>``` header.**
- Creating a log file if it doesn't exist
  - This also applies to the log directory
- Able to rename the file being logged to (this does block logs until rename completes)
- Ability to explicitly open, close, and flush the logging file.
- The ability to inherit from this target to specialize the ```PolicyFlushOn()```, ```PrintMessage()```, and ```RenameFile()``` functions.
- This gives the user some flexibility over how they may want to implement flushing and logging type functions.
- ```RenameFile()``` is also a virtual function so that the user can define if they want a specific routine for<br>
this purpose or if they don't want this function to do anything when called.
- Change the default buffer size held by the file handle by altering the DEFAULT_BUFFER_SIZE macro.
  - Default value is the system's pagefile size or 64KB, depending on the case
- Inherits the utility of the TargetBase class functions as well.
</p>

------------------------------------------------------
<p align="left"> The RotatingTarget class is capable of the following:

**Rotational logging occurs via the RotatingTarget class found in the ```<serenity/Targets/RotatingTarget.h>``` header.**
- Enable/Disble rotation if desired
  - If disabled, this class effectively mirrors FileTarget class functionality.
- Set specific rotation settings
  - The current rotation settings only implement file-size based rotations
  - Current rotation settings are:
    - enable/disable rotating initially
    - set the maximum number of files to create and rotate through <br>
    when the file size limit has been reached
    - set the file size limit for files being logged to
- Caching the file path and its components as well as the file's base name and
  setting up the current file for <br>
  rotation via ```RenameFileForRotation()```
  - This is done automatically in the constructors
  - This is also done when ```RenameFile()``` is called AND rotation is enabled.
  - If rotation was initially disabled but re-enabled after renaming the file, future logs will be named to <br>
  this base name during their rotation
    - Previous logs are unaffected until the next time they are selected in the rotation queue.
- Inherits from FileTarget functions, TargetBase functions and both of their utilities.
</p>

---------------------------------------------------------
<p align="left"> The ColorConsoleTarget class is capable of the following: </p>

**Terminal logging occurs via the ColorConsoleTarget class found in the ```<serenity/Targets/ColorConsoleTarget.h>``` header.** <br>

- Setting independant colors to message levels
 - Default colors for the message levels are:
   |  Level   |         Color          |
   | -------- | ---------------------- |
   | Trace    |      Bright White      |
   | Info     |      Bright Green      |
   | Debug    |      Bright Cyan       |
   | Warning  |      Bright Yellow     |
   | Error    |      Basic Red         |
   | Fatal    | Bright Yellow On Red   |
- Setting the terminal output mode using the console_interface enum options
  - These options mirror the standard outputs
- Enabling/Disabling color output for messages
- Resetting to default colors through ```SetOriginalColors()```
- Ability to check if an output handle is valid (```IsValidHandle()```) and <br>
  if the output handle is a terminal device (```IsTerminalType()```)
- If terminal output is being piped to another location instead:
  - If Windows platform, ```WriteFile()``` will be called instead of the default ```WriteConsole()```
  - Output will be flushed when output handle's internal buffer is full or when ```Flush()``` <br>
    is explicitly called. Will also call ```Flush()``` in the destructor to ensure contents are <br>
    present in the pipe.
    - Note: there is currently no built-in piping method, however, logging with this class will detect <br>
            if it has been piped and behave accordingly
- If Windows platform, utilizes the windows macro ENABLE_VIRTUAL_TERMINAL_PROCESSING <br>
  for ansi color codes.
  - If not defined for the windows platform, will automatically define this macro
  - If enabled, the destructor will reset the terminal mode to its defaults
- All basic 8 bit colors and combinations have been wrapped for ease-of-use in the ```<serenity/Colors/Color.h>``` <br>
header under the se_colors namespace.
- example usage for setting colors with this helper header:


```cpp
namespace color = serenity::se_colors::bright_colors;
namespace target = serenity::targets;
// Create the console target with default format pattern and user-defined name
target::ColorConsoleTarget example("Example_Logger");
// Set the message level to any ansi-supported code (using helper header here)
example.SetMsgColor(LoggerLevel::trace, color::foreground::grey );
```

</p>

--------------------------------------------------------------------------------------
<p align="center">TODO: Rename class to ConsoleTarget and edit documentation to reflect this.<br>
Reasoning: Current naming suggests only color logging is supplied by this class...</p>



------------------------
<h1 align="center"> Usage Examples </h1>

<br>

TODO: Document examples here

<br>

---------------------------
<h1 align="center"> Statistics </h1>

### For A Very Crude And Simple Benchmark Test Of A 400 byte C-style string over 1,000,000 iterations (Single Threaded) ###
- [Note]: Current Benchmark timings are accurate as of 05Jul22
- [Note]: For the console targets, message outputting was disabled, so the timings for Serenity ColorConsole<br>
        and Spdlog's Console Sink reflect that difference compared to the rest of the benched targets.
    ___________________________________________________________________________________________
    |      Logging Sink/Target      |      Logging Speed       |      Logging Throughput      |
    |-------------------------------|--------------------------|------------------------------|
    | Serenity Console Target       |        5.656 ns          |         67437.398 MB/s       |
    | Spdlog Console Sink           |       10.837 ns          |         35197.914 MB/s       |
    | Serenity File Target          |      215.981 ns          |          1766.213 MB/s       |
    | Spdlog Basic File Sink        |     1125.735 ns          |           365.318 MB/s       |
    | Serenity Rotating Target      |      627.968 ns          |           607.467 MB/s       |
    | Spdlog Rotating Sink          |     2919.572 ns          |           130.659 MB/s       |
    -------------------------------------------------------------------------------------------
- Serenity's console target is ~40%-42% faster than spdlog's current counterpart.
- Serenity's file target is ~79%-81% faster than spdlog's current counterpart.
- Serenity's rotating Target is ~78%-79% faster than spdlog's current counterpart.



<br>