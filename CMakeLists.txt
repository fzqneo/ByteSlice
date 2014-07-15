cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project (ByteSlice)


set(warnings "-Wall -Werror")
set(misc "-mavx2 -m64 -std=c++11 -fopenmp")

# Set default build type as debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Default build type: Debug." FORCE)
endif()

if(NOT CONFIGURED_ONCE)
    set(CMAKE_CXX_FLAGS "${warnings} ${misc}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O0" 
        CACHE STRING "Flags used by the complier during debug build type." FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -D NDEBUG" 
        CACHE STRING "Flags used by the compiler during release build type." FORCE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -ggdb -D NDEBUG" 
        CACHE STRING "Flags used by the compiler during relwithdebinfo build type." FORCE)
endif()


include_directories("${CMAKE_SOURCE_DIR}")
add_subdirectory(src)
add_subdirectory(example)


#############################
##  Set up test
############################
enable_testing()
SET(testlog "tests/tests.log")
add_custom_target(check-build)
add_custom_target(check 
    COMMAND ${CMAKE_CTEST_COMMAND} 
        --output-on-failure 
        --output-log ${testlog}
    COMMENT "Test log is written to ${testlog}"
)
add_dependencies(check check-build)
add_subdirectory(third-party/googletest/googletest EXCLUDE_FROM_ALL)
add_subdirectory(tests EXCLUDE_FROM_ALL)  


set(CONFIGURED_ONCE TRUE CACHE INTERNAL "A flag showing that CMake has configured at least once.")

