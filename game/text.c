#include "text.h"
#include "files.h"
#include "main.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

static const char* MODULE_NAME = "text";
static stbtt_fontinfo font_info;
static files_memory_buffer font_mem_buffer = { 0 };

static void text_init_with_memory_internal()
{
    int font_id = stbtt_InitFont(
        &font_info,
        (const unsigned char*)font_mem_buffer.data, 0);
}

void text_init_with_file(const char* filename)
{
    font_mem_buffer = files_read_file(filename);
    if (font_mem_buffer.data)
        text_init_with_memory_internal();
}

void text_init_with_memory(const char* data, long int size)
{
    font_mem_buffer.len = size;
    font_mem_buffer.data = malloc(size);
    if (font_mem_buffer.data)
    {
        memcpy(font_mem_buffer.data, data, size);
        text_init_with_memory_internal();
    }
}

void text_quit()
{
    files_free_memory_buffer(&font_mem_buffer);
}

SDL_Texture* text_render(const char* text)
{
    if (!text || !text[0] || !font_mem_buffer.data)
        return NULL;

    float scale = stbtt_ScaleForPixelHeight(&font_info, 30.f);
    int advanceWidth, leftSideBearing;
    const char* ptr = text;
    int full_width = 0;
    while (*ptr)
    {
        stbtt_GetCodepointHMetrics(&font_info, *ptr++, &advanceWidth, &leftSideBearing);
        full_width += advanceWidth + leftSideBearing;
    }
    int ascent, descent, line_gap;
    stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);
    int full_height = (ascent - descent) * scale;
    full_width *= scale;
    ascent *= scale;

    unsigned char* raw_data = calloc(
        4 * full_width * full_height, sizeof(unsigned char));
    if (!raw_data)
    {
        printf("Out of memory during text_render(...)");
        return NULL;
    }

    ptr = text;
    raw_data;
    int written_offset = 0;
    while (*ptr)
    {
        int cp = (int)(*ptr++);
        int x0, x1, y0, y1;
        stbtt_GetCodepointBitmapBox(&font_info, cp, scale, scale, &x0, &y0, &x1, &y1);
        int top_buffer = ascent + y0;
        stbtt_MakeCodepointBitmap(&font_info, raw_data + (int)(written_offset*scale) + (top_buffer * full_width),
            x1 - x0, y1 - y0, full_width,
            scale, scale, cp);
        stbtt_GetCodepointHMetrics(&font_info, cp, &advanceWidth, &leftSideBearing);
        written_offset += advanceWidth + leftSideBearing;
    }

    // We have to unroll 8 to 32 bit, start from the rear
    for (long int i = (full_width * full_height) - 1; i >= 0; --i)
    {
        raw_data[4 * i + 3] = raw_data[i];
        raw_data[4 * i + 2] = 0xff;
        raw_data[4 * i + 1] = 0xff;
        raw_data[4 * i + 0] = 0xff;
    }

    SDL_Surface* sdl_surface = SDL_CreateSurfaceFrom(full_width, full_height, SDL_PIXELFORMAT_RGBA32, (void*)raw_data, full_width * 4);
    if (!sdl_surface)
    {
        free(raw_data);
        printf("Failed to SDL_CreateSurfaceFrom");
        return NULL;
    }
    SDL_Texture* sdl_texture = SDL_CreateTextureFromSurface(global_renderer, sdl_surface);
    SDL_DestroySurface(sdl_surface);
    free(raw_data);
    if (!sdl_texture)
        printf("Failed to SDL_CreateTextureFromSurface(...)");
    return sdl_texture;
}