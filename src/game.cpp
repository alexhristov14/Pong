#include "pong/game.hpp"
#include "pong/circle.hpp"
#include <iostream>
#include <math.h>

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

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, &p1);
  SDL_RenderFillRect(renderer, &p2);

  DrawCircle(renderer, ball);

  SDL_RenderPresent(renderer);

  return 0;
}

void restart(SDL_FRect &p1, SDL_FRect &p2, PongBall &ball,
             const SDL_FRect &startP1, const SDL_FRect &startP2,
             const PongBall &startBall) {
  p1 = startP1;
  p2 = startP2;
  ball = startBall;

  float angle = (SDL_randf() * 2.0f - 1.0f) * MAX_SERVE_ANGLE;
  ball.dx = copysignf(cosf(angle), startBall.dx);
  ball.dy = sinf(angle);
}

float bounceAngle(const PongBall &ball, const SDL_FRect &paddle) {
  float offset = (ball.cy - (paddle.y + paddle.h / 2.0f)) / (paddle.h / 2.0f);
  offset = SDL_clamp(offset, -1.0f, 1.0f);
  return offset * MAX_BOUNCE_ANGLE;
}

bool ballHitsPaddle(const PongBall &ball, const SDL_FRect &paddle) {
  // Closest point on the paddle to the ball's center
  float closestX = SDL_clamp(ball.cx, paddle.x, paddle.x + paddle.w);
  float closestY = SDL_clamp(ball.cy, paddle.y, paddle.y + paddle.h);

  float distX = ball.cx - closestX;
  float distY = ball.cy - closestY;
  return distX * distX + distY * distY <= ball.radius * ball.radius;
}

void registerPaddleHit(PongBall &ball) {
  ++ball.paddleHits;
  if (ball.paddleHits % HITS_PER_SPEEDUP == 0)
    ball.speed = SDL_min(ball.speed * SPEEDUP_FACTOR, MAX_BALL_SPEED);
}
