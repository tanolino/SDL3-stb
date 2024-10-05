#pragma once

#include "all_3rd.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

extern SDL_Window* global_window;
extern SDL_Renderer* global_renderer;

extern SDL_AppResult print_sdl_error(const char* module, const char* function);