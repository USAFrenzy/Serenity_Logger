#pragma once

#define SERENITY_VERSION_MAJOR 0
#define SERENITY_VERSION_MINOR 2
#define SERENITY_VERSION_REVISION 93

#define VERSION_STRING_FORMAT( major, minor, revision ) #major "." #minor "." #revision
#define VERSION_NUMBER( maj, min, rev )                 VERSION_STRING_FORMAT( maj, min, rev )

// Last Built Statistics
#define SE_DAY             01
#define SE_MONTH           02
#define SE_YEAR            2022
#define SE_LAST_BUILT   "19:45:18"
