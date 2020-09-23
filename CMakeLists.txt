CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
project(MakeAppImageTorrent VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)

# Let cmake know that this is a release build.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3")

# Include Directories.
include_directories(${CMAKE_BINARY_DIR})

if(GIT_COMMIT)
	add_definitions(-DGIT_COMMIT="${GIT_COMMIT}")
endif()
if(BUILD_TIME)
	add_definitions(-DBUILD_TIME="${BUILD_TIME}")
endif()

execute_process(COMMAND ./gen_trackers_header.py ${CMAKE_BINARY_DIR} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(MakeAppImageTorrent MakeAppImageTorrent.cc ${CMAKE_BINARY_DIR}/Trackers.hpp)

target_link_libraries(MakeAppImageTorrent PUBLIC torrent-rasterbar pthread boost_system)

install(
	TARGETS MakeAppImageTorrent
	RUNTIME DESTINATION bin
)
