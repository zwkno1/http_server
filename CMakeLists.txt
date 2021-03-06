cmake_minimum_required (VERSION 2.8)

project(http_server)

include_directories(src)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -g -O2")

find_package(Boost REQUIRED COMPONENTS
    regex
    system
    filesystem
    )

link_directories(
    ${Boost_LIBRARY_DIRS}
    )

find_package(OpenSSL)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

set(SRC_LIST
    src/main.cpp
    src/detail/impl/http_parser.c
    )
aux_source_directory(src/tgbot SRC_LIST)
aux_source_directory(src/tgbot/types SRC_LIST)

add_executable(http_server
    ${SRC_LIST}
    )

target_link_libraries(http_server
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    -lstdc++fs
    )

