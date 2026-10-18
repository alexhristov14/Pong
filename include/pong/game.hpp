#pragma once

#include "pong/data.hpp"
#include <SDL3/SDL.h>

int initApplication(SDL_Renderer *renderer, SDL_FRect &p1, SDL_FRect &p2,
                    PongBall &ball);
void restart(SDL_FRect &p1, SDL_FRect &p2, PongBall &ball,
             const SDL_FRect &startP1, const SDL_FRect &startP2,
             const PongBall &startBall);
float bounceAngle(const PongBall &ball, const SDL_FRect &paddle);
bool ballHitsPaddle(const PongBall &ball, const SDL_FRect &paddle);
// Counts a paddle hit and speeds the ball up every HITS_PER_SPEEDUP hits
void registerPaddleHit(PongBall &ball);
