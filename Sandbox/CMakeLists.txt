set (PROJECT_NAME Sandbox)

set (SANDBOX_HEADER_FILES)
set (SANDBOX_SOURCE_FILES "src/demoTestMain.cpp")
set (SANDBOX_ALL_FILES ${SANDBOX_HEADER_FILES} ${SANDBOX_SOURCE_FILES})

add_executable (${PROJECT_NAME} ${SANDBOX_ALL_FILES})
set_target_properties (${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
target_link_libraries (
    ${PROJECT_NAME}
    LINK_PUBLIC
    Serenity
)


set (PROJECT_NAME Experimental_Sandbox)

set (EXPERIMENTAL_SB_HEADER_FILES
"src/Experimental_SB/Common.h"
"src/Experimental_SB/MessageDetails/Message_Info.h"
"src/Experimental_SB/MessageDetails/Message_Time.h"
"src/Experimental_SB/MessageDetails/Message_Formatter.h"
"src/Experimental_SB/MessageDetails/FlushPolicy.h"
"src/Experimental_SB/Targets/Target.h"
"src/Experimental_SB/Targets/Target-impl.h"
"src/Experimental_SB/Targets/ColorConsoleTarget.h"
"src/Experimental_SB/Targets/FileTarget.h"
"src/Experimental_SB/Targets/RotatingTarget.h"

)

set (EXPERIMENTAL_SB_SOURCE_FILES 
"src/Experimental_SB/Experimental_Sandbox.cpp"
"src/Experimental_SB/MessageDetails/Message_Info.cpp"
"src/Experimental_SB/MessageDetails/Message_Time.cpp"
"src/Experimental_SB/MessageDetails/Message_Formatter.cpp"
"src/Experimental_SB/MessageDetails/FlushPolicy.cpp"
"src/Experimental_SB/Targets/Target.cpp"
"src/Experimental_SB/Targets/ColorConsoleTarget.cpp"
"src/Experimental_SB/Targets/FileTarget.cpp"
"src/Experimental_SB/Targets/RotatingTarget.cpp"
)

set (EXPERIMENTAL_SB_ALL_FILES ${EXPERIMENTAL_SB_HEADER_FILES} ${EXPERIMENTAL_SB_SOURCE_FILES})

add_executable (${PROJECT_NAME} ${EXPERIMENTAL_SB_ALL_FILES})
set_target_properties (${PROJECT_NAME} PROPERTIES CXX_STANDARD 23)
target_link_libraries (
    ${PROJECT_NAME}
    LINK_PUBLIC
    Serenity
)