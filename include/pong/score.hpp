#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

void renderScore(SDL_Renderer *renderer, TTF_Font *font, int score, float x,
                 float y);
