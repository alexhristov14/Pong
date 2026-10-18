#include "pong/circle.hpp"
#include <math.h>

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
