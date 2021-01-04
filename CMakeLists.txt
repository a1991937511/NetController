cmake_minimum_required(VERSION 3.16)
project(NetController)

set(CMAKE_CXX_STANDARD 11)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
AUX_SOURCE_DIRECTORY(proxy/ PROXY_SRC)
add_executable(NetController main.cpp Acceptor.cpp Acceptor.h Session.cpp Session.h Utils.h ${PROXY_SRC})

target_include_directories(NetController PRIVATE include)

find_package(OpenSSL 1.1.0 REQUIRED)
target_include_directories(NetController PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(NetController ${OPENSSL_LIBRARIES})
if(OPENSSL_VERSION VERSION_GREATER_EQUAL 1.1.1)
    option(ENABLE_SSL_KEYLOG "Build with SSL KeyLog support" ON)
    if(ENABLE_SSL_KEYLOG)
        add_definitions(-DENABLE_SSL_KEYLOG)
    endif()

    option(ENABLE_TLS13_CIPHERSUITES "Build with TLS1.3 ciphersuites support" ON)
    if(ENABLE_TLS13_CIPHERSUITES)
        add_definitions(-DENABLE_TLS13_CIPHERSUITES)
    endif()
endif()

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
