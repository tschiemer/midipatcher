cmake_minimum_required(VERSION 3.3)
project(midipatcher)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

link_directories("/usr/local/lib/")

#########

set(RTMIDI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/rtmidi)
set(MIDIMESSAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/midimessage)
set(ASIO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/asio/asio)


########

add_subdirectory(${MIDIMESSAGE_DIR} EXCLUDE_FROM_ALL)
set(MIDIMESSAGE_INCLUDES ${MIDIMESSAGE_DIR}/include)


########

set(RTMIDI_BUILD_STATIC_LIBS true)

add_subdirectory(${RTMIDI_DIR} EXCLUDE_FROM_ALL)
set(RTMIDI_INCLUDES ${RTMIDI_DIR})

#########


set(ENABLE_SSL false)

set(ASIO_INCLUDES ${ASIO_DIR}/include)
set(ASIO_SRC
  "${ASIO_DIR}/src/asio.cpp"
)

if(ENABLE_SSL)
  message(STATUS "SSL enabled")

  list(APPEND ASIO_SRC "${ASIO_DIR}/src/asio_ssl.cpp")

  if(APPLE)
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
  else()
    set(OPENSSL_ROOT_DIR /usr/local/ssl)
  endif()

  find_package(openssl REQUIRED)

  message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
  message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

else()
  message(STATUS "SSL disabled (momentarily not in use anyways)")
endif()


add_library(asio STATIC ${ASIO_SRC})
target_compile_definitions(asio PUBLIC ASIO_SEPARATE_COMPILATION ASIO_STANDALONE)
#target_compile_options(asio PUBLIC ASIO_STANDALONE)
target_include_directories(asio PUBLIC "${ASIO_INCLUDES}" "${OPENSSL_INCLUDE_DIR}")
target_link_libraries(asio "${OPENSSL_LIBRARIES}")  # Using ASIO requires you link your final executable/library with your system's threading library (e.g. pthread on linux)

###########

file(GLOB MIDIPATCHER_INCLUDES
    "include/midipatcher/*.h"
    "include/midipatcher/*.hpp"
    "include/midipatcher/Port/*.h"
    "include/midipatcher/Port/*.hpp"
)

file(GLOB MIDIPATCHER_SRC
    "src/*.c"
    "src/*.cpp"
    "src/Port/*.c"
    "src/Port/*.cpp"
)

add_library(midipatch STATIC ${MIDIPATCHER_INCLUDES} ${MIDIPATCHER_SRC})
target_include_directories(midipatch PUBLIC
    "include/midipatcher"
    "${RTMIDI_INCLUDES}"
    "${MIDIMESSAGE_INCLUDES}"
    "${ASIO_INCLUDES}"
)
target_link_libraries(midipatch rtmidi midimessage asio)

#############

add_executable(midipatcher applications/main.hpp applications/main.cpp)
#target_compile_definitions(midipatcher PUBLIC ASIO_SEPARATE_COMPILATION ASIO_STANDALONE)
target_include_directories(midipatcher PUBLIC
    "include"
)
target_link_libraries(midipatcher midipatch)


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
