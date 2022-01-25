# Serenity Logger

## Requirements ##
##### For Out Of The Box Usage #####
- C++20 
- CMake
- Visual Studio (Currently only MSVC has an implementation of libfmt).
##### More Involved Alternative #####
- C++17 (filesystem, string_view, templated auto (in SetMessage()), std::data, std::empty, std::size)
- Remove the ```#include <format>``` and drop in fmt library headers.
  - Replace any instances of std::format related functions with fmt equivalents 
- CMake
##### C++14 And Earlier #####
- No Support As Of Yet
<br><br>

## Motivation ##
The motivation for this project is to have a logger framework that's faster than one of my to-date favorite frameworks (spdlog) while being easy to use and extensible. One often doesn't think of logging as a fun endeavor, but my goal here is to have just that - a logger that's highly efficient, fast, customizable, fun, and easy to use. I wanted to make a logger that had several color 
choices to pick from (More than a user may ever need, while still modularizing components for choices a user may not want). I plan on extending this to cover some basic RGB color combos as well and add user defined formatting call backs and a way to add user defined flags. The initial start of this project is inspired by spdlog since I feel that spdlog as a framework is amazing, however, I want to make this a project that isn't just some carbon-copy of spdlog. For now, my goal is simply to achieve a multi-threaded and single-threaded version of a console target, color console target, file target, rotating file target, a XML target capable of shredding, and lastly the possibility of a very basic HTML target. I would like to add many more built-in capabilities than just these, but I feel that this would be a strong start for this project. I did at one point try to use function pointers for formatting functions, however that route ended up being slower than the standard I'm aiming to beat - so for now these are in the form of formatting structs. The current model is inspired by spdlog in the form of how these different structs are collected to iterate through in the actual formatting, however, I hope to learn more about other methods (such as std::function instead of C-style function pointers) so that I can implement this in my own way and easily integrate user formatting callbacks by just storing the user's function callback into the same container currently being used.

## Statistics ##
I still have to properly benchmark among other logging frameworks as well as spdlog, but in crude timing benches of a 400 byte C-style string in 1,000,000 iterations 
on my Intel I9-10900k cpu. While my library still lacks a lot of the features and safety that spdlog has in place, Serenity's console target is ~40%-42% faster while 
Serenity's file target is ~30% -35% faster than spdlog's current counterparts. The newly added Rotating Target is ~45%-50% faster than spdlog's Rotating File Sink.
- Compiled with /utf-8 /O2
  - No noticable change in speed with any combination of /O1, /Os, /Ot, or /Oi.
 
### For A Very Crude And Simple Benchmark Test Of A 400 byte C-style string over 1,000,000 iterations (Single Threaded) ###
     ______________________________________________________________________________________________________________________________________________________________
	| Serenity Console Target |    Spdlog Console Sink    | Serenity File Target | Spdlog Basic File Sink    |  Serenity Rotating Target |   Spdlog Rotating Sink  |
    | ----------------------- | ------------------------- | -------------------- | --------------------------|--------------------------|--------------------------|
    |       49.105 us         |        80.523 us          |       0.705 us       |          1.042 us         |        1.089 us          |         2.128 us         | 
    | ----------------------- | ------------------------- | -------------------- | --------------------------|--------------------------|--------------------------|
#### Through-put ####
     ______________________________________________________________________________________________________________________________________________________________
	| Serenity Console Target |    Spdlog Console Sink    | Serenity File Target | Spdlog Basic File Sink    |  Serenity Rotating Target |   Spdlog Rotating Sink  | 
    | ----------------------- | ------------------------- | -------------------- | --------------------------|--------------------------|--------------------------|
    |       7.749 MB/s        |        4.726 MB/s         |     539.359 MB/s     |       365.318 MB/s        |       349.351 MB/s       |       178.838 MB/s       |
    | ----------------------- | ------------------------- | -------------------- | --------------------------|--------------------------|--------------------------|


*This Will Be Updated Later When I Have More Time*


