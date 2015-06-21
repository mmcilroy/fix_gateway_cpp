cmake_minimum_required( VERSION 2.8 )
project( fixtk_cpp )
set( VERSION_NUMBER "0.0.1" )

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
    set( VERSION "${VERSION_NUMBER}-release" )
else()
    set( VERSION "${VERSION_NUMBER}-debug" )
endif()

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--no-as-needed -g" )

include_directories( include /usr/include/lua5.2 /home/mmcilroy/pubsub_cpp/include )
add_subdirectory( src )
