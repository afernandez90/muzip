cmake_minimum_required (VERSION 2.6)

if (WIN32)
	set (BOOST_DIR "C:/Program Files/boost/boost_1_47" CACHE TYPE STRING)
else (WIN32)
	set (CMAKE_CXX_FLAGS -O2)
endif (WIN32)

				
project (muzip)
include_directories (muzip ${BOOST_DIR})
include_directories (muzip src)
					
file (GLOB_RECURSE sources src/*.cc)
add_executable (muzip ${sources})
