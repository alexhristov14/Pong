#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_timer.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <math.h>

static constexpr int WIDTH = 640;
static constexpr int HEIGHT = 480;
static constexpr int BALL_RADIUS = 15;
static constexpr float SPEED = 300.0f;
static constexpr float BALL_SPEED = 300.0f;

typedef struct PongBall {
  int radius = BALL_RADIUS;
  float cx, cy;
  float dx = 1.0f, dy = 1.0f;
} PongBall;

void DrawCircleOutline(SDL_Renderer *renderer, const PongBall &ball);
void FillCircle(SDL_Renderer *renderer, const PongBall &ball);
void DrawCircle(SDL_Renderer *renderer, const PongBall &ball);
int initApplication(SDL_Renderer *renderer, SDL_FRect &p1, SDL_FRect &p2,
                    PongBall &ball);

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
  SDL_SetRenderVSync(renderer, 1);

  SDL_FRect p1 = {
      .x = 20.0f, .y = (float)HEIGHT / 2.0f - 45.0f, .w = 15.0f, .h = 90.0f};
  SDL_FRect p2 = {.x = (float)WIDTH - 20.0f,
                  .y = (float)HEIGHT / 2.0f - 45.0f,
                  .w = 15.0f,
                  .h = 90.0f};

  PongBall ball = {.cx = (float)WIDTH / 2.0f, .cy = (float)HEIGHT / 2.0f};

  int result = initApplication(renderer, p1, p2, ball);

  if (result == 1) {
    SDL_Quit();
    return 1;
  }

  Uint64 lastTicks = SDL_GetTicks();

  while (!done) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }
    }

    Uint64 now = SDL_GetTicks();
    float dt = (now - lastTicks) / 1000.0f;
    lastTicks = now;

    const bool *keys = SDL_GetKeyboardState(nullptr);
    float dy = 0;
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_K])
      dy -= 1;
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_J])
      dy += 1;

    p1.y += dy * SPEED * dt;

    if (p1.y < 0.0f)
      p1.y = 0.0f;
    if (p1.y + 90.0f >= HEIGHT)
      p1.y = HEIGHT - 90.0f;

    // Moving the ball
    ball.cx += ball.dx * BALL_SPEED * dt;
    ball.cy += ball.dy * BALL_SPEED * dt;

    if (ball.cy - ball.radius < 0.0f) {
      ball.cy = ball.radius;
      ball.dy = 1.0f;
    }
    if (ball.cy + ball.radius > HEIGHT) {
      ball.cy = HEIGHT - ball.radius;
      ball.dy = -1.0f;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &p1);
    SDL_RenderFillRect(renderer, &p2);
    DrawCircle(renderer, ball);

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyWindow(window);

  SDL_Quit();
  return 0;
}

int initApplication(SDL_Renderer *renderer, SDL_FRect &p1, SDL_FRect &p2,
                    PongBall &ball) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

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

  DrawCircle(renderer, ball);

  SDL_RenderPresent(renderer);

  return 0;
}

void DrawCircleOutline(SDL_Renderer *renderer, const PongBall &ball) {
  float cx = ball.cx;
  float cy = ball.cy;
  int x = ball.radius;
  int y = 0;
  int err = 1 - ball.radius;

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

void FillCircle(SDL_Renderer *renderer, const PongBall &ball) {
  int radius = ball.radius;
  for (int dy = -radius; dy <= radius; dy++) {
    float dx = sqrtf((float)(radius * radius - dy * dy));
    SDL_RenderLine(renderer, ball.cx - dx, ball.cy + dy, ball.cx + dx,
                   ball.cy + dy);
  }
}

void DrawCircle(SDL_Renderer *renderer, const PongBall &ball) {
  DrawCircleOutline(renderer, ball);
  FillCircle(renderer, ball);
}
