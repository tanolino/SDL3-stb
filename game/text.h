#pragma once

#include "all_3rd.h"

extern void text_init_with_file(const char* filename);
extern void text_init_with_memory(const char* data, long int size);
extern void text_init();
extern void text_quit();

extern SDL_Texture* text_render(const char* text);
extern SDL_Texture* text_render_with_border(const char* text, int black_border);