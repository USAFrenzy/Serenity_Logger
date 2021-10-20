#pragma once

#define SERENITY_VERSION_MAJOR 0
#define SERENITY_VERSION_MINOR 2
#define SERENITY_VERSION_REVISION 27

#define VERSION_STRING_FORMAT( major, minor, revision ) #major "." #minor "." #revision
#define VERSION_NUMBER( maj, min, rev )                 VERSION_STRING_FORMAT( maj, min, rev )

// Last Built Statistics
#define SE_DAY             20
#define SE_MONTH           10
#define SE_YEAR            2021
#define SE_LAST_BUILT   "02:24:42"
