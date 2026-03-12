#pragma once
#include "common.hpp"
#include <vector>
#include <string>

struct Platform  { FRect rect; bool isGround = false; };
struct Spike     { FRect rect; };
struct Checkpoint{ FRect rect; bool active = false; float spawnX, spawnY; };
struct LevelExit { FRect rect; };

struct Level {
    std::string             name;
    std::vector<Platform>   platforms;
    std::vector<Spike>      spikes;
    std::vector<Checkpoint> checkpoints;
    LevelExit               exit;
    float                   startX, startY;
    SDL_Color               bgColor;
    SDL_Color               groundColor;
    SDL_Color               platColor;
};

std::vector<Level> buildLevels();
