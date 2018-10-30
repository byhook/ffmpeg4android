
cmake_minimum_required(VERSION 3.4.1)


add_library(ffmpeg-cmd
           SHARED
           src/main/cpp/ffmpeg_cmd.c
           src/main/cpp/cmdutils.c
           src/main/cpp/ffmpeg_filter.c
           src/main/cpp/ffmpeg_opt.c
           src/main/cpp/ffmpeg_mod.c)

find_library(log-lib
            log)

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
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/libffmpeg.so
                    )

#头文件
include_directories(${LIBRARY_DIR}/libs/${ANDROID_ABI}/include src/main/cpp)

target_link_libraries(ffmpeg-cmd ffmpeg-single ${log-lib})
