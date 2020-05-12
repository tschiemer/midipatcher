cmake_minimum_required(VERSION 3.3)
project(midipatcher)


set(RTMIDI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/rtmidi)
set(MIDIMESSAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/midimessage)
set(ASIO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/asio/asio)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


link_directories("/usr/local/lib/")


add_subdirectory(${RTMIDI_DIR} EXCLUDE_FROM_ALL)
set(RTMIDI_INCLUDES ${RTMIDI_DIR})

add_subdirectory(${MIDIMESSAGE_DIR} EXCLUDE_FROM_ALL)
set(MIDIMESSAGE_INCLUDES ${MIDIMESSAGE_DIR}/include)


set(ASIO_INCLUDES ${ASIO_DIR}/include)
set(ASIO_SRC
  "${ASIO_DIR}/src/asio.cpp"
  "${ASIO_DIR}/src/asio_ssl.cpp"
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

if(APPLE)
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif()

find_package(openssl REQUIRED)

message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

add_library(asio STATIC ${ASIO_SRC})
target_compile_definitions(asio PUBLIC ASIO_SEPARATE_COMPILATION ASIO_STANDALONE)
#target_compile_options(asio PUBLIC ASIO_STANDALONE)
target_include_directories(asio PUBLIC "${ASIO_INCLUDES}" "${OPENSSL_INCLUDE_DIR}")
target_link_libraries(asio "${OPENSSL_LIBRARIES}")  # Using ASIO requires you link your final executable/library with your system's threading library (e.g. pthread on linux)


add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
target_compile_definitions(${PROJECT_NAME} PUBLIC ASIO_SEPARATE_COMPILATION ASIO_STANDALONE)
target_include_directories(${PROJECT_NAME} PUBLIC
    "include"
    "${RTMIDI_INCLUDES}"
    "${MIDIMESSAGE_INCLUDES}"
    "${ASIO_INCLUDES}"
)
target_link_libraries(${PROJECT_NAME} rtmidi midimessage asio)


##################

#add_executable(rtmidi-bug rtmidi-bug.cpp)
#target_link_libraries(rtmidi-bug rtmidi)



  #list(APPEND API_DEFS "-D__MACOSX_CORE__")
  #list(APPEND API_LIST "coremidi")
  #list(APPEND LINKLIBS "-framework CoreServices")
  #list(APPEND LINKLIBS "-framework CoreAudio")
  #list(APPEND LINKLIBS "-framework CoreMIDI")
  #list(APPEND LINKLIBS "-framework CoreFoundation")
  #list(APPEND LINKFLAGS "-Wl,-F/Library/Frameworks")

  #add_executable(coremidi-bug coremidi-bug.cpp)
  #target_link_libraries(coremidi-bug ${LINKLIBS})
