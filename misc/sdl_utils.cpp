#include <SDL3/SDL.h>

SDL_Texture *LoadTexture(SDL_Renderer *renderer, const char *file, bool transparent, int *width_out, int *height_out)
{
    SDL_Surface *temp = NULL;
    SDL_Texture *texture = NULL;
    char *path;

    path = (char*)file;

    if (path) {
        file = path;
    }

    temp = SDL_LoadBMP(file);
    if (!temp) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load %s: %s", file, SDL_GetError());
    } else {
        /* Set transparent pixel as the pixel at (0,0) */
        if (transparent) {
            if (SDL_GetSurfacePalette(temp)) {
                const Uint8 bpp = SDL_BITSPERPIXEL(temp->format);
                const Uint8 mask = (1 << bpp) - 1;
                if (SDL_PIXELORDER(temp->format) == SDL_BITMAPORDER_4321)
                    SDL_SetSurfaceColorKey(temp, true, (*(Uint8 *)temp->pixels) & mask);
                else
                    SDL_SetSurfaceColorKey(temp, true, ((*(Uint8 *)temp->pixels) >> (8 - bpp)) & mask);
            } else {
                switch (SDL_BITSPERPIXEL(temp->format)) {
                case 15:
                    SDL_SetSurfaceColorKey(temp, true,
                                    (*(Uint16 *)temp->pixels) & 0x00007FFF);
                    break;
                case 16:
                    SDL_SetSurfaceColorKey(temp, true, *(Uint16 *)temp->pixels);
                    break;
                case 24:
                    SDL_SetSurfaceColorKey(temp, true,
                                    (*(Uint32 *)temp->pixels) & 0x00FFFFFF);
                    break;
                case 32:
                    SDL_SetSurfaceColorKey(temp, true, *(Uint32 *)temp->pixels);
                    break;
                }
            }
        }

        if (width_out) {
            *width_out = temp->w;
        }

        if (height_out) {
            *height_out = temp->h;
        }

        texture = SDL_CreateTextureFromSurface(renderer, temp);
        if (!texture) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError());
        }
    }
    SDL_DestroySurface(temp);
    if (path) {
        SDL_free(path);
    }
    return texture;
}
