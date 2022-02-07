# Serenity Logger

## Requirements ##
##### For Out Of The Box Usage #####
- C++20 
- CMake
- Visual Studio (Currently only MSVC has an implementation of libfmt).
##### More Involved Alternative #####
- No Built-In Support As Of Yet
- C++17 (filesystem, string_view, templated auto (in SetMessage()), std::data, std::empty, std::size)
- Remove the ```#include <format>``` and drop in fmt library headers.
  - Replace any instances of std::format related functions with fmt equivalents 
- CMake
##### C++14 And Earlier #####
- No Support As Of Yet
<br><br>

### For all current details about the library, check out the documentation [HERE](https://usafrenzy.github.io/Serenity_Logger/) ###
<br>

## Motivation ##
<p>   The motivation for this project is to have a logger framework that's faster than one of my to-date favorite frameworks (spdlog) while being easy to use and extensible. One often doesn't think of logging as a fun endeavor, but my goal here is to have just that - a logger that's highly efficient, fast, customizable, fun, and easy to use. I wanted to make a logger that had several color choices to pick from (More than a user may ever need, while still modularizing components for choices a user may not want). I plan on extending this to cover some basic RGB color combos as well and add user defined formatting call backs and a way to add user defined flags. </p> 
<p>This project is honestly more of a learning project meant for practicing. This is for a multitude of different reasons - the major one being that my main aim is to have a project that was completed from start to finish. </p>
<p>For now, my goal is simply to achieve a multi-threaded and single-threaded version of a console target, color console target, file target, rotating file target, a XML target capable of shredding, and lastly the possibility of a very basic HTML target. I would like to add many more built-in capabilities than just these, but I feel that this would be a strong start for this project. I did at one point try to use function pointers for formatting functions, however that route ended up being slower than the standard I'm aiming to beat - so for now these are in the form of formatting structs.</p>

## Statistics ##
**Sandbox Environment was compiled with /utf-8 /O2 for the below results.** 
<br>

I still have to properly benchmark among other logging frameworks as well as spdlog, but in crude timing benches of a 400 byte C-style string in 1,000,000 iterations 
on my Intel I9-10900k cpu:
- Serenity's console target is ~40%-42% faster than spdlog's current counterpart.
- Serenity's file target is ~30% -35% faster than spdlog's current counterpart. 
- The newly added Rotating Target is ~45%-50% faster than spdlog's current counterpart.
 <br>

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
*This Will Be Updated Later When I Have More Time*


