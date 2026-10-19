#include "pong/menu.hpp"
#include <string.h>

bool buttonContains(const Button &button, float x, float y) {
  SDL_FPoint point = {x, y};
  return SDL_PointInRectFloat(&point, &button.rect);
}

void renderButton(SDL_Renderer *renderer, TTF_Font *font,
                  const Button &button) {
  float mouseX, mouseY;
  SDL_GetMouseState(&mouseX, &mouseY);
  bool hovered = buttonContains(button, mouseX, mouseY);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  if (hovered)
    SDL_RenderFillRect(renderer, &button.rect);
  else
    SDL_RenderRect(renderer, &button.rect);

  // Text is black on the filled (hovered) button, white otherwise
  SDL_Color color =
      hovered ? SDL_Color{0, 0, 0, 255} : SDL_Color{255, 255, 255, 255};
  SDL_Surface *surface =
      TTF_RenderText_Blended(font, button.label, strlen(button.label), color);

  if (surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture) {
      SDL_FRect dest = {0, 0, 0, 0};
      SDL_GetTextureSize(texture, &dest.w, &dest.h);
      dest.x = button.rect.x + (button.rect.w - dest.w) / 2.0f;
      dest.y = button.rect.y + (button.rect.h - dest.h) / 2.0f;

      SDL_RenderTexture(renderer, texture, NULL, &dest);
      SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
  }
}
