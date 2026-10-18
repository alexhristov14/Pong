#pragma once

#include <SDL3/SDL.h>

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;
constexpr int BALL_RADIUS = 15;
constexpr float SPEED = 500.0f;
constexpr float BALL_SPEED = 450.0f;      // starting speed of each serve
constexpr float MAX_BALL_SPEED = 850.0f;  // faster can skip through paddles
constexpr int HITS_PER_SPEEDUP = 10;
constexpr float SPEEDUP_FACTOR = 1.1f;    // +10%
constexpr float AI_SPEED = 360.0f; // slower than the player, so it can miss
constexpr float AI_DEAD_ZONE = 10.0f;
constexpr float MAX_SERVE_ANGLE = SDL_PI_F / 4.0f;  // 45 degrees
constexpr float MAX_BOUNCE_ANGLE = SDL_PI_F / 3.0f; // 60 degrees

typedef struct PongBall {
  int radius = BALL_RADIUS;
  float cx, cy;
  float dx = -1.0f, dy = -0.5f;
  float speed = BALL_SPEED;
  int paddleHits = 0;
} PongBall;

extern int score_p1;
extern int score_p2;
