#pragma once

constexpr int   SCREEN_W   = 1800;
constexpr int   SCREEN_H   = 1500;
constexpr int   FPS        = 60;
constexpr float DELTA      = 1.0f / FPS;

// ── Physique ─────────────
constexpr float GRAVITY      = 1400.0f;
constexpr float JUMP_FORCE   = -520.0f;
constexpr float MOVE_SPEED   = 220.0f;
constexpr float MAX_FALL_VEL = 900.0f;

// ── DASH ────────────────
constexpr float DASH_SPEED    = 600.0f;
constexpr float DASH_DURATION = 0.15f;
constexpr float DASH_COOLDOWN = 0.5f;