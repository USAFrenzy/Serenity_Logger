set(HEADER_FILE "Serenity/include/serenity/Version.h")
set(CACHE_FILE  "Incremental_Cache/Incremental_Build_Cache.txt")



function(update_version_info ver_maj ver_min ver_rev)
 # Get The Current Time And Date  
    string(TIMESTAMP DATE "%d")
    string(TIMESTAMP MONTH "%m")
    string(TIMESTAMP YEAR "%Y")
    string(TIMESTAMP LAST_TIME_COMPILED "%H:%M:%S")
#Create, Or Update If Exists, The Version Header
    file(WRITE "${HEADER_FILE}" 
    "#pragma once\n\n"
    "#define SERENITY_VERSION_MAJOR ${ver_maj}\n"
    "#define SERENITY_VERSION_MINOR ${ver_min}\n"
    "#define SERENITY_VERSION_REVISION ${ver_rev}\n\n"

    "#define VERSION_STRING_FORMAT( major, minor, revision ) #major \".\" #minor \".\" #revision\n"
    "#define VERSION_NUMBER( maj, min, rev )                 VERSION_STRING_FORMAT( maj, min, rev )\n\n"

    "// Last Built Statistics\n"
    "#define SE_DAY         ${DATE}\n"
    "#define SE_MONTH       ${MONTH}\n"
    "#define SE_YEAR        ${YEAR}\n"
    "#define SE_LAST_BUILT  \"${LAST_TIME_COMPILED}\"\n"
    )
endfunction()

# If The Version.h File Doesn't Exist, Create It; If It Does Exist,
# Read Version.h In, Increment Revision Number, And Update Version.h.
function(get_and_increment_version)

if(EXISTS ${HEADER_FILE})
file(READ "${HEADER_FILE}" VERSION_FILE)
string(REGEX MATCH "VERSION_MAJOR ([0-9]*)" _ "${VERSION_FILE}")
    set(version_major ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_MINOR ([0-9]*)" _ "${VERSION_FILE}")
    set(version_minor ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_REVISION ([0-9]*)" _ "${VERSION_FILE}")
    set(version_revision ${CMAKE_MATCH_1})
    math(EXPR version_revision "${version_revision}+1")
update_version_info(
    "${version_major}" 
    "${version_minor}"
    "${version_revision}"
)
set(SERENITY_FULL_VERSION "${version_major}.${version_minor}.${version_revision}" PARENT_SCOPE)
    else()
        set(version_major "0")
        set(version_minor "2")
        set(version_revision "0")
        update_version_info(
            "${version_major}" 
            "${version_minor}" 
            "${version_revision}"
        )
        set(SERENITY_FULL_VERSION "${version_major}.${version_minor}.${version_revision}" PARENT_SCOPE)
    endif()
endfunction()