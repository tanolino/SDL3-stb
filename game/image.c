#include "image.h"
#include "files.h"
#include "main.h"
//#include "all_3rd.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
// #define STBI_ONLY_GIF
// #dedine STBI_ONLY_BMP
#include <stb_image.h>

static const char* MODULE_NAME = "SDL_image";

static SDL_Texture* image_load_from_sti_image(
    stbi_uc* buffer, int width, int height, int comp)
{
    // Special stuff not yet implemented
    if (comp < 3 && comp > 4) return NULL;
    SDL_PixelFormat format;
    switch (comp)
    {
    case 3:
        format = SDL_PIXELFORMAT_RGB24;
        break;
    case 4:
        format = SDL_PIXELFORMAT_RGBA32;
        break;
    default:
        printf("Unsupported channel amount: %d", comp);
        return NULL;
    }
    SDL_Surface* surface = SDL_CreateSurfaceFrom(
        width, height, format, (void*)buffer, (width * comp) );

    SDL_Texture* res = SDL_CreateTextureFromSurface(
        global_renderer, surface);

    SDL_DestroySurface(surface);
    return res;
}

SDL_Texture* image_load_from_file(const char* filename)
{
    files_memory_buffer buffer = files_read_file(filename);
    SDL_Texture* res = image_load_from_memory(buffer.data, buffer.len);
    files_free_memory_buffer(&buffer);
    return res;
}

SDL_Texture* image_load_from_memory(const char* data, long int size)
{
    if (!data || !size)
        return NULL;

    int width, height, comp;
    stbi_uc* stbi_mem = stbi_load_from_memory(
        (const stbi_uc*)data, (int)size, &width, &height, &comp, STBI_default);
    if (!stbi_mem)
        return NULL;

    SDL_Texture* res = image_load_from_sti_image(stbi_mem, width, height, comp);
    stbi_image_free(stbi_mem);
    return res;
}