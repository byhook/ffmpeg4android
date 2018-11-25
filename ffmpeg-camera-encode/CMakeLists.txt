
cmake_minimum_required(VERSION 3.4.1)


add_library(native-frame
           SHARED
           src/main/cpp/native_frame.cpp)

find_library(log-lib
            log)

find_library(android-lib
            android)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)


#获取上级目录
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
set(LIBRARY_DIR ${PARENT_DIR}/ffmpeg-single)

add_library(ffmpeg-single
           SHARED
           IMPORTED)

set_target_properties(ffmpeg-single
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/libffmpeg.so
                    )

add_library(libyuv
           SHARED
           IMPORTED)

set_target_properties(libyuv
                    PROPERTIES IMPORTED_LOCATION
                    ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libyuv.so
                    )

include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include)

#头文件
include_directories(${LIBRARY_DIR}/libs/${ANDROID_ABI}/include)

target_link_libraries(native-frame
                   libyuv
                   android
                   log)
