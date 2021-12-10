cmake_minimum_required(VERSION 3.19)
project(SportsTrack)

set(CMAKE_CXX_STANDARD 20)
find_package(SEAL REQUIRED)

if(APPLE)
    link_directories(/usr/local/lib)
    link_libraries(libhttpserver.dylib libcurlpp.dylib curl jsoncpp sqlite3)
else()
    link_libraries(libhttpserver libcurlpp curl jsoncpp sqlite3)
endif()

if(TARGET SEAL::seal)
    target_link_libraries(SEAL::seal)
elseif(TARGET SEAL::seal_shared)
    link_libraries(SEAL::seal_shared)
else()
    message(FATAL_ERROR "Cannot find target SEAL::seal or SEAL::seal_shared")
endif()

add_executable(client client.cpp)
add_executable(server server.cpp Database.cpp)