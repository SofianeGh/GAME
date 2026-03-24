#include "Menu.h"
#include <SDL2/SDL_ttf.h>
#include <cstdio>

// ⚠️ on récupère la font depuis main
extern TTF_Font* font;

Menu::Menu() {
    options = {"Start Game", "Quit"};
    selectedIndex = 0;
    upPressed = false;
    downPressed = false;
}

Menu::Menu(const std::vector<std::string>& opts) {
    options = opts;
    selectedIndex = 0;
    upPressed = false;
    downPressed = false;
}

void Menu::handleInput(const Uint8* state) {

    if (state[SDL_SCANCODE_UP]) {
        if (!upPressed) {
            if (selectedIndex == 0)
                selectedIndex = options.size() - 1;
            else
                selectedIndex--;
        }
        upPressed = true;
    } else upPressed = false;

    if (state[SDL_SCANCODE_DOWN]) {
        if (!downPressed) {
            selectedIndex++;
            if (selectedIndex >= options.size())
                selectedIndex = 0;
        }
        downPressed = true;
    } else downPressed = false;
}

int Menu::getSelected() const {
    return static_cast<int>(selectedIndex);
}

void Menu::render(SDL_Renderer* renderer) {

    if (!font) return;  // sécurité font

    size_t y = 200;

    for (size_t i = 0; i < options.size(); i++) {

        SDL_Color color = (i == selectedIndex)
            ? SDL_Color{255, 0, 0, 255}   // sélection rouge
            : SDL_Color{255, 255, 255, 255}; // blanc

        SDL_Surface* surface = TTF_RenderText_Solid(font, options[i].c_str(), color);
        if (!surface) {
            printf("Erreur TTF_RenderText_Solid: %s\n", TTF_GetError());
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            printf("Erreur SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
            SDL_FreeSurface(surface);
            continue;
        }

        SDL_Rect dst = {300, static_cast<int>(y), surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &dst);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        y += 60;
    }
}

std::string Menu::getSelectedText() const {
    return options[selectedIndex];
}