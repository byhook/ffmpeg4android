
cmake_minimum_required(VERSION 3.4.1)


add_library(lame-encode
           SHARED
           src/main/cpp/lame_encode.cpp
           src/main/cpp/mp3_encode.cpp)

find_library(log-lib
            log)

find_library(android-lib
            android)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)

add_library(mp3-lame
           SHARED
           IMPORTED)

set_target_properties(mp3-lame
                    PROPERTIES IMPORTED_LOCATION
                    ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libmp3lame.so
                    )

#头文件
include_directories(${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/include)

target_link_libraries(lame-encode mp3-lame ${log-lib} ${android-lib})
