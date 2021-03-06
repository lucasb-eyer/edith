# This is the first version where PROTOBUF_IMPORT_DIRS is available.
# Older versions have quite a broken FindProtobuf file AFAICT
# (at least on my ubuntu machines.)
cmake_minimum_required(VERSION 2.8.8)

project(edith)

if (APPLE)
       SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
       SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++")
elseif (UNIX)
       SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x -mmmx -msse2")
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

find_package(Snappy)
include_directories(${SNAPPY_INCLUDE_DIR})

file(GLOB edith_PROTOS "${PROJECT_SOURCE_DIR}/src/proto/*.proto")
set(PROTOBUF_IMPORT_DIRS ${PROTOBUF_INCLUDE_DIRS})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${edith_PROTOS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB edith_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(edith ${edith_SOURCES} ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(edith ${PROTOBUF_LIBRARY} ${SNAPPY_LIBRARY})
include_directories("${PROJECT_SOURCE_DIR}/src/")

# Add all the dumpers
add_executable(death_recording examples/death_recording.cpp)
target_link_libraries(death_recording edith)

add_executable(ALL_the_things examples/ALL_the_things.cpp)
target_link_libraries(ALL_the_things edith)

add_executable(hero_tracking examples/hero_tracking.cpp)
target_link_libraries(hero_tracking edith)

add_executable(ALL_hero_things examples/ALL_hero_things.cpp)
target_link_libraries(ALL_hero_things edith)

add_executable(hero_distances examples/hero_distances.cpp)
target_link_libraries(hero_distances edith)
