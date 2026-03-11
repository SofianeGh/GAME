#pragma once
#include <SDL2/SDL.h>

// ── Fenêtre ───────────────────────────────────────────────────
constexpr int   SCREEN_W = 800;
constexpr int   SCREEN_H = 500;
constexpr float DELTA    = 1.0f / 60.0f;

// ── Physique ──────────────────────────────────────────────────
constexpr float GRAVITY        = 1400.f;
constexpr float JUMP_FORCE     = -570.f;
constexpr float MOVE_SPEED     = 230.f;
constexpr float MAX_FALL       = 850.f;
constexpr float WALL_SLIDE_VEL = 90.f;    // vitesse max de glissement sur mur
constexpr float WALL_JUMP_VX   = 260.f;   // élan horizontal du wall jump
constexpr float DASH_SPEED     = 620.f;
constexpr float DASH_DURATION  = 0.13f;
constexpr float DASH_COOLDOWN  = 0.75f;
constexpr float IFRAME_DUR     = 1.2f;    // invincibilité après dommage

// ── Types ─────────────────────────────────────────────────────
struct FRect { float x, y, w, h; };

inline bool overlaps(const FRect& a, const FRect& b) {
    return a.x < b.x+b.w && a.x+a.w > b.x &&
           a.y < b.y+b.h && a.y+a.h > b.y;
}
inline SDL_Rect toSDL(const FRect& r) {
    return {(int)r.x, (int)r.y, (int)r.w, (int)r.h};
}
inline void drawFilled(SDL_Renderer* r, const FRect& f, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    auto s = toSDL(f); SDL_RenderFillRect(r, &s);
}
inline void drawOutline(SDL_Renderer* r, const FRect& f, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    auto s = toSDL(f); SDL_RenderDrawRect(r, &s);
}
