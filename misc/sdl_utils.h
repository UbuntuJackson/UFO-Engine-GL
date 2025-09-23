#pragma once
#include <SDL3/SDL.h>

SDL_Texture *LoadTexture(SDL_Renderer *renderer, const char *file, bool transparent,
                         int *width_out, int *height_out);