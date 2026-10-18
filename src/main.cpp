#include "pong/ai.hpp"
#include "pong/circle.hpp"
#include "pong/data.hpp"
#include "pong/game.hpp"
#include "pong/score.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <format>
#include <math.h>
#include <string>

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

    updateAI(p2, ball, dt);

    // Moving the ball
    ball.cx += ball.dx * ball.speed * dt;
    ball.cy += ball.dy * ball.speed * dt;

    if (ball.cy - ball.radius < 0.0f) {
      ball.cy = ball.radius;
      ball.dy = fabsf(ball.dy);
    }
    if (ball.cy + ball.radius > HEIGHT) {
      ball.cy = HEIGHT - ball.radius;
      ball.dy = -fabsf(ball.dy);
    }

    // Detecting ball & paddle collision. The ball's center must still be in
    // front of the paddle's face; once it's past, it's a miss.
    if (ball.dx < 0.0f && ball.cx >= p1.x + p1.w && ballHitsPaddle(ball, p1)) {
      ball.cx = p1.x + p1.w + ball.radius;
      float angle = bounceAngle(ball, p1);
      ball.dx = cosf(angle);
      ball.dy = sinf(angle);
      registerPaddleHit(ball);
    }
    if (ball.dx > 0.0f && ball.cx <= p2.x && ballHitsPaddle(ball, p2)) {
      ball.cx = p2.x - ball.radius;
      float angle = bounceAngle(ball, p2);
      ball.dx = -cosf(angle);
      ball.dy = sinf(angle);
      registerPaddleHit(ball);
    }

    if (ball.cx - BALL_RADIUS <= 0.0f) {
      ++score_p2;
      restart(p1, p2, ball, startP1, startP2, startBall);
      ball.dx = -fabsf(ball.dx); // serve toward player 1, who lost the point
    }
    if (ball.cx + BALL_RADIUS >= WIDTH) {
      ++score_p1;
      restart(p1, p2, ball, startP1, startP2, startBall);
      ball.dx = fabsf(ball.dx); // serve toward player 2, who lost the point
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
