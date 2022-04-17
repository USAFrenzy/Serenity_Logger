[![Build status](https://ci.appveyor.com/api/projects/status/3yywxhc5bc4gq6hr/branch/dev?svg=true)](https://ci.appveyor.com/project/USAFrenzy/serenity-logger/branch/dev)
[![](https://tokei.rs/b1/github/USAFrenzy/Serenity_Logger?category=code)](https://github.com/USAFrenzy/Serenity_Logger/tree/dev)

### For all current details about the library, check out the documentation [HERE](https://usafrenzy.github.io/Serenity_Logger/) ###

--------------------------------------
<h1 align="center"> Requirements And Features Explicitly Used</h1>

## For Out Of The Box Usage ##
- C++20 
  - C++20 features used
    - the new method of lambda ```this``` capture
    - using enum (in the WIP ```template<> std::formatter``` specializations)
    - ```<format>``` library header 
- CMake
- MSVC toolchain (Currently only MSVC has an implementation of libfmt).
  - This library will be updated to support clang and g++ once libfmt is <br>
  implemented on those compilers.
## More Involved Alternative ##
- For C++17 (No Built-In Support As Of Yet)
  - C++17 features used
    - automatic template argument deduction
    - keyword ```auto``` deductions 
    - nested namespaces resolution operator
    - filesystem header
    - string_view
    - std::data
    - std::empty
    - std::size
- CMake
- If you have access to ```fmtlib``` by Viktor Zverovich:
  - Remove the ```#include <format>``` and drop in fmt library headers as libfmt <br>
  is the standard's implementation of his work.
  - Replace any instances of std::format related functions with fmt equivalents 
- Will be adding the libfmt wrapper soon to encapsulate functions that are required by this library <br> 
   in order to add C++17 support for this library by default. <br>
  - This mostly boils down to the usage of:
    - ```std::vformat()```
    - ```std::vformat_to()```
    - ```std::make_format_args()```
    - ```std::basic_format_context<iter, char>```
    - ```template<> std::formatter``` specializations 
    - The template specializations may be abandoned and aren't currently used - manual formatting is<br> 
      currently still faster than the specializations
## C++14 And Earlier ##
- No C++14 Support As Of Yet
  - C++11 will most likely NOT be supported due to the amount of C++14 and above features used
    - C++14 features used
      - Generic lambdas
      - Lambda capture initializers
      - Return type deductions used internally in some functions
      - std::make_unique
      - Usage of move semantics and rvalue references
      - Usage of member functions with move semantics
      - Forwarding references
      - Variadic templates
      - keyword ```auto``` specifier
      - strongly typed enums
      - Explicit virtual overrides
      - Default and Deleted functions 
      - Range-based for loops
      - Some files use inline namespaces instead of C++17's namespace resolution operator
      - The whole right angle bracket clarity on whitespace
      - reference qualified member functions
    - C++11 features used
      - std::move
      - std::forward
      - std::thread
      - std::to_string
      - smart pointers
      - std::chrono
      - std::array
      - std::make_shared
<br><br>

--------------------------------------
<h1 align="center"> Motivation </h1>

<p align="center">Starting from commit 037a0d2, this project shifted from being solely a wrapper to one of being a home-brewed logging framework.
As for the motivation, one often doesn't think of logging as a fun endeavor, but my goal here is to have just that - a logger that's highly 
efficient, fast, customizable, fun, and easy to use. I wanted to make a logger that had several color choices to pick from 
(More than a user may ever need, while still modularizing components for choices a user may not want). I plan on extending 
this to cover some basic RGB color combos as well and add user defined formatting call backs and a way to add user defined flags. </p> 

<p align="center">This project is honestly more of a learning project meant for practicing. This is for a multitude of different reasons - the major 
 one being that my main aim is to have a project that was completed from start to finish. </p>

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
- Being able to erase contents of the file via ```EraseContents()``` call
- Able to rename the file being logged to (this does block logs until rename completes)
- Ability to explicitly open, close, and flush the logging file.
- The ability to inherit from this target to specialize the ```PolicyFlushOn()```, ```PrintMessage()```, and ```RenameFile()``` functions.
- This gives the user some flexibility over how they may want to implement flushing and logging type functions. 
- ```RenameFile()``` is also a virtual function so that the user can define if they want a specific routine for<br>
this purpose or if they don't want this function to do anything when called.
- Change the default buffer size held by the file handle by altering the DEFAULT_BUFFER_SIZE macro.
  - Default value is 64KB
  - NOTE: this is different from the buffer used by the TargetBase class and is used to minimize system <br>
  OS calls for flushing contents to disk, which can be very expensive.
- Inherits the utility of the TargetBase class functions as well.
</p>

------------------------------------------------------
<p align="left"> The RotatingTarget class is capable of the following: 

**Rotational logging occurs via the RotatingTarget class found in the ```<serenity/Targets/RotatingTarget.h>``` header.** 
- The RotatingTarget class is currently still in the experimental namespace due to work still being <br>
done on interval (day/hour based) rotation. File size based rotation is functional however.
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



-----------------------------------------------------------------------------------------------------
<p align="center"> NOTE: If using both FileTarget and RotatingTarget classes, the DEFAULT_BUFFER_SIZE macro will affect both of these classes' file handle buffer. </p>

-----------------------------------------------------------------------------------------------------
 <p align="center">TODO: Make the file handle buffer sizing a function call instead of a macro to separate common classes. </p>



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
<p align="center">
**The Sandbox Environment was compiled with /utf-8 /O2 for the below results.**
</p>

- Serenity's console target is ~40%-42% faster than spdlog's current counterpart.
- Serenity's file target is ~30% -35% faster than spdlog's current counterpart. 
- The newly added Rotating Target is ~45%-50% faster than spdlog's current counterpart.

#### For A Very Crude And Simple Benchmark Test Of A 400 byte C-style string over 1,000,000 iterations (Single Threaded) ####

 ___________________________________________________________________________________________
 |      Logging Sink/Target      |      Logging Speed       |      Logging Throughput      |             
 |-------------------------------|--------------------------|------------------------------|
 | Serenity Console Target       |        49.105 us         |         7.749 MB/s           |
 | Spdlog Console Sink           |        80.523 us         |         4.726 MB/s           |
 | Serenity File Target          |        0.705 us          |         539.359 MB/s         |
 | Spdlog Basic File Sink        |        1.042 us          |         365.318 MB/s         |
 | Serenity Rotating Target      |        1.089 us          |         349.351 MB/s         |
 |  Spdlog Rotating Sink         |        2.128 us          |         178.838 MB/s         |
 -------------------------------------------------------------------------------------------



<br>