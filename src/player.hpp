#pragma once
#include "common.hpp"
#include "level.hpp"
#include <SDL2/SDL_image.h>

enum class DashState { READY, DASHING, COOLDOWN };

class Player {
public:
SDL_Texture* texture = nullptr;
void loadTexture(SDL_Renderer* r, const char* path);
    FRect rect      = {100.f, 200.f, 30.f, 40.f};
    float vx = 0.f, vy = 0.f;

    int   hp = 3, maxHp = 3;
    bool  onGround    = false;
    bool  touchWallL  = false;
    bool  touchWallR  = false;
    int   facingDir   = 1;   // 1=droite  -1=gauche

    DashState dashState  = DashState::READY;
    float     dashTimer  = 0.f;
    float     dashDirX   = 1.f;

    float     iframeTimer = 0.f;

    float spawnX = 100.f, spawnY = 200.f;

    // ── API ──────────────────────────────────────────────────
    void  init(float sx, float sy);
    void  handleInput(const Uint8* keys, bool jumpPressed, bool dashPressed);
    void  update(float dt, Level& level);
    void  takeDamage(int dmg);
    void  respawn();

    bool  isDead()       const { return hp <= 0; }
    bool  isInvincible() const { return iframeTimer > 0.f; }
    float dashReadyRatio() const;   // 0.0 → 1.0 (jauge cooldown)

    void  draw(SDL_Renderer* r) const;
};
