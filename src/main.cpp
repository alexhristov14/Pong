#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>

static constexpr int WIDTH = 640;
static constexpr int HEIGHT = 480;

int main(void) {

  SDL_Window *window;
  SDL_Renderer *renderer;
  bool done = false;

  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("Pong", WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
  if (window == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n",
                 SDL_GetError());
    return 1;
  }

  renderer = SDL_CreateRenderer(window, NULL);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

  const SDL_FRect p1 = {
      .x = 20.0f, .y = (float)HEIGHT / 2.0f, .w = 15.0f, .h = 90.0f};
  const SDL_FRect p2 = {.x = (float)WIDTH - 20.0f,
                        .y = (float)HEIGHT / 2.0f,
                        .w = 15.0f,
                        .h = 90.0f};

  bool successP1 = SDL_RenderRect(renderer, &p1);
  if (!successP1) {
    std::cerr << "Error rendering player one's rectangle\n";
    return 1;
  }

  bool successP2 = SDL_RenderRect(renderer, &p2);
  if (!successP2) {
    std::cerr << "Error rendering player two's rectangle\n";
    return 1;
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, &p1);
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, &p2);

  SDL_RenderPresent(renderer);

  while (!done) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }
    }
  }

  SDL_DestroyWindow(window);

  SDL_Quit();
  return 0;
}
