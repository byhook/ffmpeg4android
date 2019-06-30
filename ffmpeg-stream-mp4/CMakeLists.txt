
cmake_minimum_required(VERSION 3.4.1)


add_library(native-stream
           SHARED
           src/main/cpp/native_stream.cpp
           src/main/cpp/mp4_stream.cpp)

#获取上级目录
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
set(LIBRARY_DIR ${PARENT_DIR}/common-static)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)

add_library(avcodec
           STATIC
           IMPORTED)

set_target_properties(avcodec
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libavcodec.a
                    )

add_library(avdevice
           STATIC
           IMPORTED)

set_target_properties(avdevice
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libavdevice.a
                    )

add_library(avfilter
           STATIC
           IMPORTED)

set_target_properties(avfilter
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libavfilter.a
                    )

add_library(avformat
           STATIC
           IMPORTED)

set_target_properties(avformat
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libavformat.a
                    )



add_library(avresample
           STATIC
           IMPORTED)

set_target_properties(avresample
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libavresample.a
                    )

add_library(avutil
           STATIC
           IMPORTED)

set_target_properties(avutil
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libavutil.a
                    )

add_library(postproc
           STATIC
           IMPORTED)

set_target_properties(postproc
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libpostproc.a
                    )

add_library(swresample
           STATIC
           IMPORTED)

set_target_properties(swresample
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libswresample.a
                    )

add_library(swscale
           STATIC
           IMPORTED)

set_target_properties(swscale
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libswscale.a
                    )

add_library(fdk-aac
           STATIC
           IMPORTED)

set_target_properties(fdk-aac
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libfdk-aac.a
                    )


add_library(x264
           STATIC
           IMPORTED)

set_target_properties(x264
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libx264.a
                    )

#头文件
include_directories(${LIBRARY_DIR}/libs/${ANDROID_ABI}/include)

target_link_libraries(native-stream
                    avformat
                    avdevice
                    avfilter
                    avcodec
                    avutil
                    postproc
                    swresample
                    avresample
                    swscale
                    x264
                    fdk-aac
                    z
                    c
                    m
                    dl
                    log)
