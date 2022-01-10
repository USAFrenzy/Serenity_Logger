# Serenity Logger

## Requirements ##
##### For Out Of The Box Usage #####
- C++20 
- CMake
- Visual Studio (Currently only MSVC has an implementation of libfmt).
##### Relatively Easy Alternative #####
- C++17 (filesystem, string_view, templated auto (in SetMessage()), std::data, std::empty, std::size)
- Remove the ```#include <format>``` and drop in fmt library headers.
  - Replace any instances of std::format related functions with fmt equivalents 
- CMake
##### C++14 And Earlier #####
- No Support As Of Yet
<br><br>

## Motivation ##
What started off as a fun project to be a wrapper around spdlog's library for other projects soon turned into a goal of making my own home-brewed logging library.
Spdlog is a fantastic logging library and is highly efficient, safe, and fast - really fast. With the addition of {fmt}'s style of formatting, spdlog is without a doubt a top contender for fast, low-latency logging.
The customization spdlog provides is also easily extendable to make a logger that suits your particular need in a project while also having some awesome features for their built-in sinks.
While working on the wrapper for spdlog, I noticed that there were some things I couldn't do with ease though and that some things, such as color choices, were limited. My Goal for Serenity is to have a fun,
highly customizable, yet very fast and efficient logging framework that can be just as easily extendable to user's needs if that user somehow stumbles upon this. I wanted to make a logger that had several color 
choices to pick from (More than a user may ever need, while still modularizing components for choices a user may not want). I plan on extending this to cover some basic RGB color combos as well. I also wanted to 
extend the types of sinks available in a similar fashion to how spdlog allows a user to extend theirs, but with more customization options on top. For now, my goal is simply to achieve a multi-threaded and single-threaded 
version of a console target, color console target, file target, rotating file target, a XML target capable of shredding, and lastly the possibility of a very basic HTML target.

## Statistics ##
I still have to properly benchmark among other logging frameworks as well as spdlog, but in crude timing benches of a 400 byte C-style string in 1,000,000 iterations 
on my Intel I9-10900k cpu. While my library still lacks a lot of the features and safety that spdlog has in place, Serenity's console target is ~31% faster while 
Serenity's file target is ~35% faster than spdlog's current counterparts. (Compiled with /utf-8 /O2; No noticable change in speed with /O1).
### For A Very Crude And Simple Benchmark Test Of A 400 byte C-style string over 1,000,000 iterations (Single Threaded) ###
    __________________________________________________________________________________________________________
	| Serenity Console Target |    Spdlog Console Sink    | Serenity File Target | Spdlog Basic File Sink    |
    | ----------------------- | ------------------------- | -------------------- | --------------------------|
    |       57.573 us         |        84.394 us          |       0.735 us       |          1.118 us         |
    | ----------------------- | ------------------------- | -------------------- | --------------------------|

*This Will Be Updated Later When I Have More Time*


