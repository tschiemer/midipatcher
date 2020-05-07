cmake_minimum_required(VERSION 3.3)
project(midipatcher)


set(RTMIDI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/rtmidi)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

link_directories("/usr/local/lib/")

add_subdirectory(${RTMIDI_DIR} EXCLUDE_FROM_ALL)
set(RTMIDI_INCLUDES ${RTMIDI_DIR})


include_directories(
    "include"
    "${RTMIDI_INCLUDES}"
)

file(GLOB MAIN_INC
    "include/*.h"
    "include/*.hpp"
    "src/Port/*.h"
    "src/Port/*.hpp"
)

file(GLOB MAIN_SRC
    "src/*.c"
    "src/*.cpp"
    "src/Port/*.c"
    "src/Port/*.cpp"
)


set(HEADER_FILES
    ${MAIN_INC}
)

set(SOURCE_FILES
  ${MAIN_SRC}
)


add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} rtmidi)
