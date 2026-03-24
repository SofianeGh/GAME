#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <string>

class Menu {
public:
    Menu();
    Menu(const std::vector<std::string>& opts);

    void handleInput(const Uint8* keyboardState);
    void render(SDL_Renderer* renderer);

    int getSelected() const;
    std::string getSelectedText() const;

private:
    std::vector<std::string> options;
    size_t selectedIndex;  // size_t pour correspondre à options.size()

    bool upPressed;
    bool downPressed;
};