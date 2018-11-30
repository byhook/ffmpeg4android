
cmake_minimum_required(VERSION 3.4.1)

file(GLOB native_files "src/main/cpp/*.cpp")

add_library(native-swscale
           SHARED
           ${native_files})

#获取上级目录
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
set(LIBRARY_DIR ${PARENT_DIR}/ffmpeg-single)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)

add_library(ffmpeg-single
           SHARED
           IMPORTED)

set_target_properties(ffmpeg-single
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/libffmpeg.so)

#头文件
include_directories(${LIBRARY_DIR}/libs/${ANDROID_ABI}/include)

target_link_libraries(native-swscale ffmpeg-single log android)
