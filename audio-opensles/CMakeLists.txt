
cmake_minimum_required(VERSION 3.4.1)


file(GLOB native_files "src/main/cpp/*.cpp")

add_library(native-recorder
           SHARED
           ${native_files})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)

#头文件
include_directories(${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/include)

target_link_libraries(native-recorder OpenSLES log android)
