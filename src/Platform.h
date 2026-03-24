#pragma once
#include <SDL2/SDL.h>
#include "Rectangle.h"

class Platform
{
    private:
    Rect rect;

    public: 

    Platform(float x, float y, float w, float h);
    const Rect& getRect() const;
    void drawPlatform(SDL_Renderer* renderer, SDL_Color top, SDL_Color body);
};