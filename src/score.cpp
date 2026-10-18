#include "pong/score.hpp"
#include <format>
#include <string>

void renderScore(SDL_Renderer *renderer, TTF_Font *font, int score, float x,
                 float y) {
  std::string text = std::format("Score: {}", score);

  SDL_Color color = {255, 255, 255, 255};
  SDL_Surface *surface =
      TTF_RenderText_Blended(font, text.c_str(), text.size(), color);

  if (surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture) {
      SDL_FRect dest = {x, y, 0, 0};
      SDL_GetTextureSize(texture, &dest.w, &dest.h);

      SDL_RenderTexture(renderer, texture, NULL, &dest);
      SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
  }
}
