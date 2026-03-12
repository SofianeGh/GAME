#pragma once
#include "common.hpp"
#include "player.hpp"
#include <string>

// Affiche HP (cœurs) + jauge de dash + indicateur de niveau
void drawHUD(SDL_Renderer* r, const Player& p,
             int levelIdx, int totalLevels);
