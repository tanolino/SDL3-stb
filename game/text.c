#include "text.h"
#include "files.h"
#include "main.h"
#include "zlib.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

static const char* MODULE_NAME = "text";
static stbtt_fontinfo font_info;
static files_memory_buffer font_mem_buffer = { 0 };

#include "../resources/resources_font.h"
#include "../resources/resources_font_gz.h"

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

void text_init()
{
    unsigned char* out_buffer = malloc(fonts_Unitblock_JpJma_ttf_len * sizeof(char));
    z_stream stream = { 0 };
    if (inflateInit2(&stream, 32 + MAX_WBITS) != Z_OK) {
        printf("inflateInit2(...) failed\n"); exit(1);
    }
    stream.avail_in = Unitblock_JpJma_ttf_gz_len;
    stream.next_in = (Bytef*)Unitblock_JpJma_ttf_gz;
    stream.avail_out = fonts_Unitblock_JpJma_ttf_len;
    stream.next_out = (Bytef*)out_buffer;
    int res = inflate(&stream, Z_NO_FLUSH);
    if (res != Z_STREAM_END) {
        printf("inflate(..) failed with %d .\n", res); exit(1);
    }
    inflateEnd(&stream);
    text_init_with_memory(out_buffer, fonts_Unitblock_JpJma_ttf_len);
    // text_init_with_memory((const unsigned char*)fonts_Unitblock_JpJma_ttf, fonts_Unitblock_JpJma_ttf_len);
}

void text_quit()
{
    files_free_memory_buffer(&font_mem_buffer);
}

static int get_text_width(const char* text)
{
    int advanceWidth, leftSideBearing;
    int full_width = 0;
    while (*text)
    {
        stbtt_GetCodepointHMetrics(&font_info, *text++, &advanceWidth, &leftSideBearing);
        full_width += advanceWidth + leftSideBearing;
    }
    return full_width;
}

struct text_measure
{
    int width;
    int height;
    int ascent;
    float scale;
};
typedef struct text_measure text_measure;

static text_measure take_measure(const char* text)
{
    int ascent, descent, line_gap;
    stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);

    float scale = stbtt_ScaleForPixelHeight(&font_info, 30.f);
    text_measure result = {
        .scale = scale,
        .width = (int)ceilf((float)get_text_width(text) * scale),
        .height = (int)ceilf((float)(ascent - descent) * scale),
        .ascent = (int)ceilf((float)ascent * scale)
    };
    return result;
}

static SDL_Texture* raw_data_into_texture(int width, int height, unsigned char* data)
{
    SDL_Surface* sdl_surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, (void*)data, width * 4);
    if (!sdl_surface)
    {
        printf("Failed to SDL_CreateSurfaceFrom");
        return NULL;
    }
    SDL_Texture* sdl_texture = SDL_CreateTextureFromSurface(global_renderer, sdl_surface);
    SDL_DestroySurface(sdl_surface);
    if (!sdl_texture)
        printf("Failed to SDL_CreateTextureFromSurface(...)");
    return sdl_texture;
}

static void write_text_into_8bit_buffer(const text_measure* measure, unsigned char* data, const char* text)
{
    int written_offset = 0;
    while (*text)
    {
        int cp = (int)(*text++);
        int x0, x1, y0, y1;
        int offset = (int)floorf(written_offset * measure->scale);
        stbtt_GetCodepointBitmapBox(&font_info, cp, measure->scale, measure->scale, &x0, &y0, &x1, &y1);

        int top_buffer = measure->ascent + y0;
        stbtt_MakeCodepointBitmap(&font_info, data + offset + (top_buffer * measure->width),
            x1 - x0, y1 - y0, measure->width,
            measure->scale, measure->scale, cp);
        int advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(&font_info, cp, &advanceWidth, &leftSideBearing);
        written_offset += advanceWidth + leftSideBearing;
    }
}

SDL_Texture* text_render(const char* text)
{
    if (!text || !text[0] || !font_mem_buffer.data)
        return NULL;

    text_measure measure = take_measure(text);

    unsigned char* raw_data = calloc(4 * measure.width * measure.height, sizeof(unsigned char));
    if (!raw_data)
    {
        printf("Out of memory during text_render(...)");
        return NULL;
    }

    write_text_into_8bit_buffer(&measure, raw_data, text);

    // We have to unroll 8 to 32 bit, start from the rear
    for (long int i = (measure.width * measure.height) - 1; i >= 0; --i)
    {
        raw_data[4 * i + 3] = raw_data[i]; // Alpha
        raw_data[4 * i + 2] = 0xFF; // B
        raw_data[4 * i + 1] = 0xFF; // G
        raw_data[4 * i + 0] = 0xFF; // R
    }

    SDL_Texture* result = raw_data_into_texture(measure.width, measure.height, raw_data);
    free(raw_data);
    return result;
}

unsigned char max_uc(unsigned char a, unsigned char b)
{
    return a > b ? a : b;
}

SDL_Texture* text_render_with_border(const char* text, int black_border)
{
    if (!text || !text[0] || !font_mem_buffer.data)
        return NULL;

    text_measure measure = take_measure(text);

    int buff_width = measure.width + black_border + black_border;
    int buff_height = measure.height + black_border + black_border;
    int buff_size_8bit = measure.width * measure.height;
    int buff_size_32bit = buff_width * buff_height * 4; // 4 = RGBA
    unsigned char* raw1 = calloc(buff_size_8bit + buff_size_32bit, sizeof(unsigned char));
    if (!raw1)
    {
        printf("Out of memory during text_render(...)");
        return NULL;
    }

    // we use the memory like [ ... ... 32 bit image ... ... | .. 8 bit image ..]
    unsigned char* raw2 = raw1 + buff_size_32bit;
    write_text_into_8bit_buffer(&measure, raw2, text);

    // We have to unroll 8 to 32 bit,
    // map [measure.width, measure.height] to [buff_width, buff_height]
    // calloc sets everything 0, which is nice
    for (int y = 0; y < measure.height; ++y)
    {
        for (int x = 0; x < measure.width; ++x)
        {
            unsigned char val = raw2[y * measure.width + x];

            int y_raw1 = buff_width * (y + black_border);
            int x_raw1 = x + black_border;
            unsigned char* ptr = raw1 + (4 * (y_raw1 + x_raw1));
            ptr[0] = val; // R
            ptr[1] = val; // G
            ptr[2] = val; // B
            // ptr[3] = val; // Alpha
            for (int y2 = -black_border+1; y2 < black_border-1; ++y2)
            {
                int left = black_border - abs(y2);
                for (int x2 = -left; x2 < left; ++x2)
                {
                    unsigned char* ptr2 = ptr + (4 * (y2*buff_width + x2));
                    ptr2[3] = max_uc(ptr2[3], val);
                }
            }
        }
    }

    SDL_Texture* result = raw_data_into_texture(buff_width, buff_height, raw1);
    free(raw1);
    return result;
}