#include "SDL3/SDL_rect.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <math.h>

static constexpr int WIDTH = 640;
static constexpr int HEIGHT = 480;
static constexpr int BALL_RADIUS = 15;

void DrawCircle(SDL_Renderer *renderer, float cx, float cy, int radius);
void FillCircle(SDL_Renderer *renderer, float cx, float cy, int radius);

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

  // render the rectangles
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, &p1);
  SDL_RenderFillRect(renderer, &p2);

  DrawCircle(renderer, (float)WIDTH / 2.0f, (float)HEIGHT / 2.0f + BALL_RADIUS,
             BALL_RADIUS);
  FillCircle(renderer, (float)WIDTH / 2.0f, (float)HEIGHT / 2.0f + BALL_RADIUS,
             BALL_RADIUS);

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

void DrawCircle(SDL_Renderer *renderer, float cx, float cy, int radius) {
  int x = radius;
  int y = 0;
  int err = 1 - radius;

  while (x >= y) {
    SDL_FPoint pts[8] = {
        {cx + x, cy + y}, {cx + y, cy + x}, {cx - y, cy + x}, {cx - x, cy + y},
        {cx - x, cy - y}, {cx - y, cy - x}, {cx + y, cy - x}, {cx + x, cy - y},
    };

    SDL_RenderPoints(renderer, pts, 8);

    ++y;
    if (err < 0) {
      err += 2 * y + 1;
    } else {
      x--;
      err += 2 * (y - x) + 1;
    }
  }
}

void FillCircle(SDL_Renderer *renderer, float cx, float cy, int radius) {
  for (int dy = -radius; dy <= radius; dy++) {
    float dx = sqrtf((float)(radius * radius - dy * dy));
    SDL_RenderLine(renderer, cx - dx, cy + dy, cx + dx, cy + dy);
  }
}
