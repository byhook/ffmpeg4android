
cmake_minimum_required(VERSION 3.4.1)


add_library(ffmpeg-hello
           SHARED
           src/main/cpp/ffmpeg-hello.c)

find_library(log-lib
            log)

#获取上级目录
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
set(LIBRARY_DIR ${PARENT_DIR}/ffmpeg-multiple)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)

add_library(avcodec
           SHARED
           IMPORTED)
add_library(avdevice
           SHARED 
           IMPORTED)
add_library(avfilter
           SHARED
           IMPORTED)
add_library(avformat
           SHARED
           IMPORTED)
add_library(avutil
           SHARED
           IMPORTED)
add_library(postproc
           SHARED
           IMPORTED)
add_library(swscale
           SHARED
           IMPORTED)

set_target_properties(avcodec
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/libavcodec.so
                    )
set_target_properties(avdevice
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/libavdevice.so
                    )
set_target_properties(avfilter
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}//libs/${ANDROID_ABI}/libavfilter.so
                    )
set_target_properties(avformat
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/libavformat.so
                    )
set_target_properties(avutil
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/libavutil.so
                    )
set_target_properties(postproc
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/libpostproc.so
                    )
set_target_properties(swscale
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/libswscale.so
                    )
#头文件
include_directories(${LIBRARY_DIR}/libs/${ANDROID_ABI}/include)

target_link_libraries(ffmpeg-hello avcodec avdevice avfilter avformat avutil swscale postproc ${log-lib})
