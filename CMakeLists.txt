cmake_minimum_required (VERSION 2.6)
project (cpp-demos)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

add_executable(pointer-to-vector-danger 
               pointer-to-vector-danger/main.cpp)

add_executable(virtual-destructor-synchronization-bug
               virtual-destructor-synchronization-bug/main.cpp)

if(WIN32)
else()
target_link_libraries(virtual-destructor-synchronization-bug pthread)
endif()

