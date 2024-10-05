#pragma once

#include "all_3rd.h"

extern SDL_Texture* image_load_from_file(const char* filename);
extern SDL_Texture* image_load_from_memory(const char* data, long int size);