#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include "Rectangle.h"
#include "Platform.h"

struct Player {
    //doit passer en privée et des données
    Rect  rect  = {100.f, 200.f, 36.f, 48.f};
    float vx = 0.f, vy = 0.f;
    bool onGround = false;
    // stats 
    int hearts = 3;
    bool alive= true;

    // Dash
    bool isDashing = false;
    float dashTimer = 0.f;
    float dashCooldown = 0.f;
    int dashDir = 1;

    //attack
    float attackdir = 1;
    bool isAttacking = false;
    int attackTimer = 0;
    int attackdamage = 1;

    //public + manque commentaires pour doxyfile
    void handleInput(const Uint8* keys);
    void update(float dt, const std::vector<Platform>& platforms);
    void draw(SDL_Renderer* renderer) const;
};