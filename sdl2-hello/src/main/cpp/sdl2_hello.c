
#include <jni.h>
#include <stdio.h>
#include <time.h>
#include "SDL.h"
#include "SDL_log.h"
#include "SDL_main.h"

#ifdef ANDROID

#include <android/log.h>

#define LOG_TAG    "SDL2Hello"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#endif


int main(int argc, char *argv[]) {

    struct SDL_Window *window = NULL;
    struct SDL_Renderer *render = NULL;
    struct SDL_Surface *bmp = NULL;
    struct SDL_Texture *texture = NULL;

    char *filepath = "/sdcard/Android/data/com.onzhou.sdl2.hello/files/input.bmp";

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == -1) {
        LOGE("SDL_Init failed %s", SDL_GetError());
    }

    window = SDL_CreateWindow("SDL HelloWorld!", 100, 100, 320, 480,
                              SDL_WINDOW_SHOWN);
    if (window == NULL) {
        LOGE("SDL_CreateWindow failed  %s", SDL_GetError());
    }

    render = SDL_CreateRenderer(window, -1,
                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (render == NULL) {
        LOGE("SDL_CreateRenderer failed  %s", SDL_GetError());
    }

    bmp = SDL_LoadBMP(filepath);
    if (bmp == NULL) {
        LOGE("SDL_LoadBMP failed: %s", SDL_GetError());
    }

    texture = SDL_CreateTextureFromSurface(render, bmp);
    SDL_FreeSurface(bmp);

    SDL_RenderClear(render);
    SDL_RenderCopy(render, texture, NULL, NULL);
    SDL_RenderPresent(render);

    SDL_Delay(10000);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);

    //Quit SDL
    SDL_Quit();
    return 0;
}