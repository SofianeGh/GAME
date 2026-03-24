#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <cstdio>
#include "Constants.h"
#include "Player.h"
#include "Platform.h"
#include "Menu.h"

// ----- FONT GLOBAL -----
TTF_Font* font = nullptr;

// ----- TOUCHES MODIFIABLES -----
SDL_Scancode LEFT_KEY  = SDL_SCANCODE_A;
SDL_Scancode RIGHT_KEY = SDL_SCANCODE_D;
SDL_Scancode JUMP_KEY  = SDL_SCANCODE_SPACE;
SDL_Scancode DASH_KEY  = SDL_SCANCODE_X;

// ----- ETAT DU JEU -----
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    OPTIONS,
    EXIT
};

int main()
{
    // ---- Initialisation SDL ----
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    font = TTF_OpenFont("assets/font.ttf", 32);
    if (!font) {
        printf("Impossible de charger la font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Platformer + Dash",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    // ---- Plateau ----
    std::vector<Platform> platforms = {
        Platform(0.f, 460.f, 800.f, 40.f),
        Platform(80.f, 400.f, 130.f, 10.f),
        Platform(250.f, 350.f, 100.f, 10.f),
        Platform(400.f, 300.f, 150.f, 10.f),
        Platform(600.f, 250.f, 120.f, 10.f)
    };

    SDL_Color grassGreen = {80,160,50,255};
    SDL_Color dirtBrown  = {120,80,40,255};
    SDL_Color floatTop   = {60,200,100,255};
    SDL_Color floatBody  = {90,130,60,255};

    Player player;

    // ---- MENUS ----
    Menu menu({"Start Game", "Options", "Quit"});
    Menu pauseMenu({"Resume", "Options", "Main Menu"});
    Menu optionsMenu({
        "Move Left: A",
        "Move Right: D",
        "Jump: Space",
        "Dash: X",
        "Back"
    });

    GameState state = GameState::MENU;
    bool running = true;
    bool enterPressed = false;

    SDL_Event event;

    while (running && state != GameState::EXIT)
    {
        // ---- EVENEMENTS ----
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) running = false;

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                if (state == GameState::PLAYING)
                    state = GameState::PAUSED;
                else if (state == GameState::PAUSED)
                    state = GameState::PLAYING;
                else if (state == GameState::OPTIONS)
                    state = GameState::PAUSED;
                else
                    running = false;
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(nullptr);

        // ---- UPDATE ----
        if (state == GameState::MENU)
        {
            menu.handleInput(keys);
            if (keys[SDL_SCANCODE_RETURN] && !enterPressed) {
                int choice = menu.getSelected();
                if (choice == 0) state = GameState::PLAYING;
                if (choice == 1) state = GameState::OPTIONS;
                if (choice == 2) state = GameState::EXIT;
                enterPressed = true;
            } else if (!keys[SDL_SCANCODE_RETURN]) enterPressed = false;
        }
        else if (state == GameState::PLAYING)
        {
            player.handleInput(keys);
            player.update(DELTA, platforms);
        }
        else if (state == GameState::PAUSED)
        {
            pauseMenu.handleInput(keys);
            if (keys[SDL_SCANCODE_RETURN] && !enterPressed) {
                int choice = pauseMenu.getSelected();
                if (choice == 0) state = GameState::PLAYING;
                if (choice == 1) state = GameState::OPTIONS;
                if (choice == 2) state = GameState::MENU;
                enterPressed = true;
            } else if (!keys[SDL_SCANCODE_RETURN]) enterPressed = false;
        }
        else if (state == GameState::OPTIONS)
        {
            optionsMenu.handleInput(keys);
            if (keys[SDL_SCANCODE_RETURN] && !enterPressed) {
                int choice = optionsMenu.getSelected();
                if (choice == 4) { // Back
                    state = GameState::PAUSED;
                } else {
                    // attente nouvelle touche
                    SDL_Event e;
                    bool waiting = true;
                    printf("Appuyez sur la nouvelle touche pour %s\n", optionsMenu.getSelectedText().c_str());
                    while (waiting) {
                        while (SDL_PollEvent(&e)) {
                            if (e.type == SDL_KEYDOWN) {
                                SDL_Scancode newKey = e.key.keysym.scancode;
                                switch (choice) {
                                    case 0: LEFT_KEY  = newKey; break;
                                    case 1: RIGHT_KEY = newKey; break;
                                    case 2: JUMP_KEY  = newKey; break;
                                    case 3: DASH_KEY  = newKey; break;
                                }
                                waiting = false;
                            }
                        }
                    }
                }
                enterPressed = true;
            } else if (!keys[SDL_SCANCODE_RETURN]) enterPressed = false;
        }

        // ---- RENDER ----
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        if (state == GameState::MENU)
        {
            menu.render(renderer);
        }
        else if (state == GameState::PLAYING)
        {
            for (size_t i = 0; i < platforms.size(); ++i)
                platforms[i].drawPlatform(renderer,
                    i==0?grassGreen:floatTop,
                    i==0?dirtBrown:floatBody);
            player.draw(renderer);
        }
        else if (state == GameState::PAUSED)
        {
            for (size_t i = 0; i < platforms.size(); ++i)
                platforms[i].drawPlatform(renderer,
                    i==0?grassGreen:floatTop,
                    i==0?dirtBrown:floatBody);
            player.draw(renderer);

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0,0,0,150);
            SDL_Rect overlay = {0,0,SCREEN_W,SCREEN_H};
            SDL_RenderFillRect(renderer, &overlay);

            pauseMenu.render(renderer);
        }
        else if (state == GameState::OPTIONS)
        {
            optionsMenu.render(renderer);
        }

        SDL_RenderPresent(renderer);
    }

    // ---- CLEANUP ----
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}