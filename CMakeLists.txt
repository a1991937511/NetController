cmake_minimum_required(VERSION 3.16)
project(NetController)

set(CMAKE_CXX_STANDARD 11)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
add_executable(NetController main.cpp Acceptor.cpp Acceptor.h Session.cpp Session.h Utils.h proxy/Proxy.h proxy/Socks5.cpp proxy/Socks5.h proxy/Direct.cpp proxy/Direct.h)

target_include_directories(NetController PRIVATE include)

find_package(Boost REQUIRED)
target_include_directories(NetController PRIVATE ${Boost_INCLUDE_DIR})
message(Boost version: ${Boost_VERSION_STRING})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(NetController Threads::Threads)

set(BUILD_TESTING OFF)
add_subdirectory(libmaxminddb)

add_executable(mmdb_text mmdb_text.cpp)
target_link_libraries(mmdb_text PRIVATE maxminddb)
target_include_directories(mmdb_text PRIVATE libmaxminddb/include)
