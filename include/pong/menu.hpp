#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct Button {
  SDL_FRect rect;
  const char *label;
} Button;

// Outlined button with centered text; filled in while the mouse is over it
void renderButton(SDL_Renderer *renderer, TTF_Font *font, const Button &button);
bool buttonContains(const Button &button, float x, float y);
