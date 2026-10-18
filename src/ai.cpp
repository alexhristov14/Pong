#include "pong/ai.hpp"

void updateAI(SDL_FRect &paddle, const PongBall &ball, float dt) {
  float paddleCenter = paddle.y + paddle.h / 2.0f;
  float target = ball.dx > 0.0f ? ball.cy : HEIGHT / 2.0f;
  float diff = target - paddleCenter;

  if (SDL_fabsf(diff) > AI_DEAD_ZONE) {
    float step = AI_SPEED * dt;
    paddle.y += SDL_clamp(diff, -step, step);
  }

  if (paddle.y < 0.0f)
    paddle.y = 0.0f;
  if (paddle.y + paddle.h > HEIGHT)
    paddle.y = HEIGHT - paddle.h;
}
