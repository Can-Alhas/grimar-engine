#include <SDL.h>
#include <cstdio>

#include "grimar/core/Assert.hpp"
#include "grimar/core/Log.hpp"

int main(int argc, char* argv[])
{
    // if (SDL_Init(SDL_INIT_VIDEO) != 0)
    // {
    //     std::printf("SDL_Init Error: %s\n", SDL_GetError());
    //     return 1;
    // }
    //
    // SDL_Window* window = SDL_CreateWindow(
    //     "Grimar Engine - SDL2 Test",
    //     SDL_WINDOWPOS_CENTERED,
    //     SDL_WINDOWPOS_CENTERED,
    //     800,
    //     600,
    //     SDL_WINDOW_SHOWN
    // );
    //
    // if (!window)
    // {
    //     std::printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    //     SDL_Quit();
    //     return 1;
    // }
    //
    // SDL_Renderer* renderer = SDL_CreateRenderer(
    //     window,
    //     -1,
    //     SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    // );
    //
    // if (!renderer)
    // {
    //     std::printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    //     SDL_DestroyWindow(window);
    //     SDL_Quit();
    //     return 1;
    // }
    //
    // bool running = true;
    // while (running)
    // {
    //     SDL_Event e;
    //     while (SDL_PollEvent(&e))
    //     {
    //         if (e.type == SDL_QUIT)
    //             running = false;
    //
    //         if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
    //             running = false;
    //     }
    //
    //     // Arka plan rengi (koyu gri)
    //     SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    //     SDL_RenderClear(renderer);
    //
    //     SDL_RenderPresent(renderer);
    // }
    //
    // SDL_DestroyRenderer(renderer);
    // SDL_DestroyWindow(window);
    // SDL_Quit();
    // return 0;

    //GRIMAR_ASSERT(2 + 2 == 4);
    //GRIMAR_ASSERT_MSG(false, "Intentional assert test");

    // GRIMAR_LOG_TRACE("info test");
    // GRIMAR_LOG_INFO("info test");
    // GRIMAR_LOG_WARN("warn test");
    // GRIMAR_LOG_ERROR("error test");

    GRIMAR_LOG_INFO("Boot");
    GRIMAR_ASSERT_MSG(false, "Intentional assert test");

    return 0;
}
