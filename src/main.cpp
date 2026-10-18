#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_timer.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <format>
#include <iostream>
#include <math.h>

static int score_p1 = 0;
static int score_p2 = 2;

static constexpr int WIDTH = 640;
static constexpr int HEIGHT = 480;
static constexpr int BALL_RADIUS = 15;
static constexpr float SPEED = 300.0f;
static constexpr float BALL_SPEED = 300.0f;

typedef struct PongBall {
  int radius = BALL_RADIUS;
  float cx, cy;
  float dx = -1.0f, dy = -0.5f;
} PongBall;

void renderScore(SDL_Renderer *renderer, TTF_Font *font, int score, float x,
                 float y);
void DrawCircleOutline(SDL_Renderer *renderer, const PongBall &ball);
void FillCircle(SDL_Renderer *renderer, const PongBall &ball);
void DrawCircle(SDL_Renderer *renderer, const PongBall &ball);
int initApplication(SDL_Renderer *renderer, SDL_FRect &p1, SDL_FRect &p2,
                    PongBall &ball);
void restart(SDL_FRect &p1, SDL_FRect &p2, PongBall &ball,
             const SDL_FRect &startP1, const SDL_FRect &startP2,
             const PongBall &startBall);

int main(void) {

  SDL_Window *window;
  SDL_Renderer *renderer;
  bool done = false;

  SDL_Init(SDL_INIT_VIDEO);

  if (!TTF_Init()) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not initialize SDL_ttf: %s\n",
                 SDL_GetError());
    SDL_Quit();
    return 1;
  }

  std::string fontPath =
      std::format("{}assets/fonts/DejaVuSansMono.ttf", SDL_GetBasePath());

  TTF_Font *font = TTF_OpenFont(fontPath.c_str(), 24.0f);

  if (font == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not load font %s: %s\n",
                 fontPath.c_str(), SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

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
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  // Starting positions, restored when the game restarts
  const SDL_FRect startP1 = p1;
  const SDL_FRect startP2 = p2;
  const PongBall startBall = ball;

  // First serve gets a random angle too
  restart(p1, p2, ball, startP1, startP2, startBall);

  Uint64 lastTicks = SDL_GetTicks();

  while (!done) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }
      if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat &&
          event.key.scancode == SDL_SCANCODE_R) {
        restart(p1, p2, ball, startP1, startP2, startBall);
        score_p1 = 0;
        score_p2 = 0;
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
      ball.dy = fabsf(ball.dy);
    }
    if (ball.cy + ball.radius > HEIGHT) {
      ball.cy = HEIGHT - ball.radius;
      ball.dy = -fabsf(ball.dy);
    }

    // Detecting ball & paddle collision
    if (ball.dx < 0.0f && ball.cx - ball.radius <= p1.x + p1.w &&
        ball.cx + ball.radius >= p1.x && ball.cy + ball.radius >= p1.y &&
        ball.cy - ball.radius <= p1.y + p1.h) {
      ball.cx = p1.x + p1.w + ball.radius;
      ball.dx = 1.0f;
    }
    if (ball.dx > 0.0f && ball.cx + ball.radius >= p2.x &&
        ball.cx - ball.radius <= p2.x + p2.w && ball.cy + ball.radius >= p2.y &&
        ball.cy - ball.radius <= p2.y + p2.h) {
      ball.cx = p2.x - ball.radius;
      ball.dx = -1.0f;
    }

    if (ball.cx - BALL_RADIUS <= 0.0f) {
      ++score_p2;
      restart(p1, p2, ball, startP1, startP2, startBall);
      ball.dx = -1.0f; // serve toward player 1, who lost the point
    }
    if (ball.cx + BALL_RADIUS >= WIDTH) {
      ++score_p1;
      restart(p1, p2, ball, startP1, startP2, startBall);
      ball.dx = 1.0f; // serve toward player 2, who lost the point
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &p1);
    SDL_RenderFillRect(renderer, &p2);
    DrawCircle(renderer, ball);

    renderScore(renderer, font, score_p1, 60.0f, 20.0f);
    renderScore(renderer, font, score_p2, WIDTH - 200.0f, 20.0f);

    SDL_RenderPresent(renderer);
  }

  TTF_CloseFont(font);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  TTF_Quit();
  SDL_Quit();
  return 0;
}

void renderScore(SDL_Renderer *renderer, TTF_Font *font, int score, float x,
                 float y) {
  std::string text = std::format("Score: {}", score);

  SDL_Color color = {255, 255, 255, 255};
  SDL_Surface *surface =
      TTF_RenderText_Blended(font, text.c_str(), text.size(), color);

  if (surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture) {
      SDL_FRect dest = {x, y, 0, 0};
      SDL_GetTextureSize(texture, &dest.w, &dest.h);

      SDL_RenderTexture(renderer, texture, NULL, &dest);
      SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
  }
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

void restart(SDL_FRect &p1, SDL_FRect &p2, PongBall &ball,
             const SDL_FRect &startP1, const SDL_FRect &startP2,
             const PongBall &startBall) {
  p1 = startP1;
  p2 = startP2;
  ball = startBall;

  // Random serve angle: dy in [-0.75, 0.75]
  ball.dy = (SDL_randf() * 2.0f - 1.0f) * 0.75f;
}
