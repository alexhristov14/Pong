#pragma once

#include "pong/data.hpp"
#include <SDL3/SDL.h>

void updateAI(SDL_FRect &paddle, const PongBall &ball, float dt);
