#pragma once

#include "pong/data.hpp"
#include <SDL3/SDL.h>

void DrawCircleOutline(SDL_Renderer *renderer, const PongBall &ball);
void FillCircle(SDL_Renderer *renderer, const PongBall &ball);
void DrawCircle(SDL_Renderer *renderer, const PongBall &ball);
