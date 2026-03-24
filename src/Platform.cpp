#include "Platform.h"

Platform::Platform(float x, float y, float w, float h)
{
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
}

const Rect& Platform::getRect() const
{
    return rect;
}

void Platform::drawPlatform(SDL_Renderer* renderer, SDL_Color top, SDL_Color body)
{
    SDL_Rect sdlRect = {(int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h};
    SDL_SetRenderDrawColor(renderer, body.r, body.g, body.b, body.a);
    SDL_RenderFillRect(renderer, &sdlRect);

    SDL_Rect topLine = {(int)rect.x, (int)rect.y, (int)rect.w, 6};
    SDL_SetRenderDrawColor(renderer, top.r, top.g, top.b, top.a);
    SDL_RenderFillRect(renderer, &topLine);
}