//
// Created by byhook on 18-11-26.
//

#ifndef FFMPEG4ANDROID_LOGGER_H
#define FFMPEG4ANDROID_LOGGER_H

#ifdef ANDROID

#include <android/log.h>

#define LOG_TAG    "OpenSLES"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#endif

#endif //FFMPEG4ANDROID_LOGGER_H
