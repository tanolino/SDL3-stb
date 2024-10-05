
#define THIS_IS_MAIN
#include "main.h"
#include "files.h"
#include "image.h"
#include "text.h"

SDL_Window* global_window = NULL;
SDL_Renderer* global_renderer = NULL;
static bool do_run = true;
static const char* MODULE_NAME = "SDL3";

// For now we put this here, move somewhere else later
static const char* FONT_FILE_PATH = "Z:\\Dev\\SDL3-stb\\resources\\fonts\\Unitblock-JpJma.ttf";
static SDL_Texture* text_texture = NULL;

static const char* IMG1_FILE_PATH = "C:\\Users\\Nathanael\\Pictures\\DSW7_cottage_pic_uSI9MgL.png";
static files_memory_buffer img1_membuffer = { 0 };
static SDL_Texture* img1_texture = NULL;

static SDL_Thread* thread = NULL;
static bool thread_finished = false;

int run_in_thread(void* ptr)
{
    img1_membuffer = files_read_file((const char*)ptr);
    thread_finished = true;
    return 0;
}

SDL_AppResult print_sdl_error(const char* module, const char* function)
{
    const char* sdl_error = SDL_GetError();
    printf("%s Error at \"%s\" : %s\n", module, function, sdl_error);
    return SDL_APP_FAILURE;
}

SDL_AppResult SDLCALL SDL_AppInit(void** appstate, int argc, char** argv)
{
    SDL_SetMainReady();
    if (!SDL_Init(SDL_INIT_VIDEO))
        return print_sdl_error(MODULE_NAME, "SDL_Init(SDL_INIT_VIDEO)");

    global_window = SDL_CreateWindow("Hello World!",
        800, 600,
        SDL_WINDOW_RESIZABLE);
    if (!global_window)
        return print_sdl_error(MODULE_NAME, "SDL_CreateWindow(...)");

    SDL_SetWindowMinimumSize(global_window, 800, 600);

    global_renderer = SDL_CreateRenderer(global_window, NULL);
    if (!global_renderer)
        return print_sdl_error(MODULE_NAME, "SDL_CreateRenderer(...)");

    text_init_with_file(FONT_FILE_PATH);
    text_texture = text_render(IMG1_FILE_PATH);

    SDL_Color color_black = { .r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE };

    thread = SDL_CreateThread(run_in_thread, "OffloadImageLoad", (void*)IMG1_FILE_PATH);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDLCALL SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDLCALL SDL_AppIterate(void* appstate)
{
    SDL_SetRenderDrawColor(global_renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(global_renderer);

    SDL_SetRenderDrawColor(global_renderer, 0x0, 0x0, 0x0, SDL_ALPHA_OPAQUE);
    if (img1_texture)
    {
        SDL_RenderTexture(global_renderer, img1_texture, NULL, NULL);
    }
    if (text_texture)
    {
        float iW = 0, iH = 0;
        if (SDL_GetTextureSize(text_texture, &iW, &iH))
        {
            SDL_FRect rect = {
                .x = (800.f - iW) / 2.f,
                .y = (600.f - iH) / 2.f,
                .w = iW, .h = iH
            };
            float border = 5.f;
            SDL_FRect frame = {
                .x = rect.x - border,
                .y = rect.y - border,
                .w = rect.w + 2 * border,
                .h = rect.h + 2 * border
            };
            SDL_RenderFillRect(global_renderer, &frame);
            SDL_SetTextureColorMod(text_texture, (SDL_GetTicks() / 4)% 0xff, 0, 0);
            SDL_RenderTexture(global_renderer, text_texture, NULL, &rect);
        }
    }
    SDL_RenderPresent(global_renderer);

    if (thread_finished)
    {
        int ret;
        SDL_WaitThread(thread, &ret);
        thread = NULL;
        thread_finished = false;

        img1_texture = image_load_from_memory(img1_membuffer.data, img1_membuffer.len);
        files_free_memory_buffer(&img1_membuffer);
    }

    return SDL_APP_CONTINUE;
}

void SDLCALL SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    if (thread)
    {
        int ret;
        SDL_WaitThread(thread, &ret);
    }

    if (img1_texture)
        SDL_DestroyTexture(img1_texture);

    if (text_texture)
        SDL_DestroyTexture(text_texture);

    text_quit();

    if (global_renderer)
        SDL_DestroyRenderer(global_renderer);

    if (global_window)
        SDL_DestroyWindow(global_window);

    SDL_Quit();
}