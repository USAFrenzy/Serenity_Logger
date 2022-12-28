#pragma once

// Clang format messes with the automated version update
// clang-format off

#define SERENITY_VERSION_MAJOR 1
#define SERENITY_VERSION_MINOR 0
#define SERENITY_VERSION_REVISION 2

// clang-format on

#define VERSION_STRING_FORMAT(major, minor, revision) #major "." #minor "." #revision
#define VERSION_NUMBER(maj, min, rev)                 VERSION_STRING_FORMAT(maj, min, rev)

// Last Built Statistics
#define SE_DAY                                        28
#define SE_MONTH                                      12
#define SE_YEAR                                       2022
#define SE_LAST_BUILT                                 "14:40:43"
