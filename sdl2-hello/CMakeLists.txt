
cmake_minimum_required(VERSION 3.4.1)


add_library(main
           SHARED
           src/main/cpp/sdl2_hello.c)

find_library(log-lib
            log)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)

add_library(sdl2
           SHARED
           IMPORTED)

set_target_properties(sdl2
                    PROPERTIES IMPORTED_LOCATION
                    ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libSDL2.so
                    )

#头文件
include_directories(${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/include)

target_link_libraries(main sdl2 ${log-lib})
