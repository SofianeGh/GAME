#include "player.hpp"
#include <algorithm>
#include <cmath>
#include <SDL2/SDL_image.h>

// ─────────────────────────────────────────────────────────────

void Player::init(float sx, float sy)
{
    rect     = {sx, sy, 30.f, 40.f};
    vx = vy  = 0.f;
    hp = maxHp = 3;
    onGround = touchWallL = touchWallR = false;
    facingDir   = 1;
    dashState   = DashState::READY;
    dashTimer   = 0.f;
    iframeTimer = 0.f;
    spawnX = sx; spawnY = sy;

}

// ─────────────────────────────────────────────────────────────
void Player::handleInput(const Uint8* keys, bool jumpPressed, bool dashPressed)
{
    bool dashing = (dashState == DashState::DASHING);

    // ── Déplacement horizontal (désactivé pendant le dash) ───
    if (!dashing) {
        vx = 0.f;
        if (keys[SDL_SCANCODE_LEFT]  || keys[SDL_SCANCODE_A]) { vx = -MOVE_SPEED; facingDir = -1; }
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) { vx =  MOVE_SPEED; facingDir =  1; }
    }

    // ── Saut (sol + wall jump) ────────────────────────────────
    if (jumpPressed) {
        if (onGround) {
            vy = JUMP_FORCE;
            onGround = false;
        }
        else if (touchWallL && !onGround) {
            vy = JUMP_FORCE;
            vx = WALL_JUMP_VX;    // élan vers la droite
            facingDir = 1;
        }
        else if (touchWallR && !onGround) {
            vy = JUMP_FORCE;
            vx = -WALL_JUMP_VX;   // élan vers la gauche
            facingDir = -1;
        }
    }

    // ── Dash ─────────────────────────────────────────────────
    if (dashPressed && dashState == DashState::READY) {
        dashDirX  = (vx != 0.f) ? (vx > 0.f ? 1.f : -1.f) : (float)facingDir;
        dashState = DashState::DASHING;
        dashTimer = DASH_DURATION;
        vy        = 0.f;   // annule la gravité pendant le dash
    }
}

// ─────────────────────────────────────────────────────────────
void Player::loadTexture(SDL_Renderer* r, const char* path)
{
    texture = IMG_LoadTexture(r, path);
}
// ─────────────────────────────────────────────────────────────
void Player::update(float dt, Level& level)
{
    // ── Timers ────────────────────────────────────────────────
    if (iframeTimer > 0.f) iframeTimer -= dt;

    // ── Machine à états du dash ───────────────────────────────
    bool dashing = false;
    switch (dashState) {
    case DashState::DASHING:
        dashTimer -= dt;
        vx = dashDirX * DASH_SPEED;
        vy = 0.f;
        dashing = true;
        if (dashTimer <= 0.f) {
            dashState = DashState::COOLDOWN;
            dashTimer = DASH_COOLDOWN;
        }
        break;
    case DashState::COOLDOWN:
        dashTimer -= dt;
        if (dashTimer <= 0.f) dashState = DashState::READY;
        break;
    default: break;
    }

    // ── Gravité (ignorée pendant le dash) ────────────────────
    if (!dashing) {
        vy += GRAVITY * dt;
        if (vy > MAX_FALL) vy = MAX_FALL;

        // Glissement sur mur : ralentit la chute
        bool onWall = (!onGround) && (touchWallL || touchWallR);
        if (onWall && vy > 0.f)
            vy = std::min(vy, WALL_SLIDE_VEL);
    }

    // ── Collision horizontale ─────────────────────────────────
    touchWallL = touchWallR = false;
    rect.x += vx * dt;

    for (auto& p : level.platforms) {
        if (!overlaps(rect, p.rect)) continue;
        // Détermine le côté de l'impact via la direction de vx
        bool hitRight = (vx > 0.f) ||
            (vx == 0.f && rect.x + rect.w / 2.f > p.rect.x + p.rect.w / 2.f);
        if (hitRight) { rect.x = p.rect.x - rect.w; touchWallR = true; }
        else          { rect.x = p.rect.x + p.rect.w; touchWallL = true; }
        if (!dashing) vx = 0.f;
    }
    // Bords de l'écran
    if (rect.x < 0.f)               { rect.x = 0.f;               touchWallL = true; }
    if (rect.x + rect.w > SCREEN_W) { rect.x = SCREEN_W - rect.w; touchWallR = true; }

    // ── Collision verticale ───────────────────────────────────
    onGround = false;
    rect.y += vy * dt;

    for (auto& p : level.platforms) {
        if (!overlaps(rect, p.rect)) continue;
        if (vy > 0.f) { rect.y = p.rect.y - rect.h; onGround = true; }
        else          { rect.y = p.rect.y + p.rect.h; }
        vy = 0.f;
    }

    // ── Piques ───────────────────────────────────────────────
    if (!isInvincible()) {
        for (auto& s : level.spikes) {
            if (overlaps(rect, s.rect)) { takeDamage(1); break; }
        }
    }

    // ── Checkpoints ───────────────────────────────────────────
    for (auto& c : level.checkpoints) {
        if (!c.active && overlaps(rect, c.rect)) {
            c.active = true;
            spawnX = c.spawnX;
            spawnY = c.spawnY;
        }
    }

    // ── Mort par chute ────────────────────────────────────────
    if (rect.y > SCREEN_H + 100.f)
        hp = 0;
}

// ─────────────────────────────────────────────────────────────
void Player::takeDamage(int dmg)
{
    if (isInvincible()) return;
    hp = std::max(0, hp - dmg);
    iframeTimer = IFRAME_DUR;
    if (!isDead()) vy = JUMP_FORCE * 0.45f;   // petit recul vers le haut
}

void Player::respawn()
{
    rect.x = spawnX; rect.y = spawnY;
    vx = vy = 0.f;
    hp = maxHp;
    onGround = touchWallL = touchWallR = false;
    dashState   = DashState::READY;
    dashTimer   = 0.f;
    iframeTimer = 0.6f;   // brève invincibilité au respawn
}

float Player::dashReadyRatio() const
{
    if (dashState == DashState::READY)   return 1.f;
    if (dashState == DashState::DASHING) return 0.f;
    return 1.f - (dashTimer / DASH_COOLDOWN);
}

// ─────────────────────────────────────────────────────────────
void Player::draw(SDL_Renderer* r) const
{
    if (texture) {
        SDL_Rect dst = {(int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h};
        SDL_RenderCopy(r, texture, nullptr, &dst);
    }

    if (isInvincible() && (int)(iframeTimer * 12) % 2 == 0) return;

    bool dashing = (dashState == DashState::DASHING);
    int x = (int)rect.x, y = (int)rect.y;
    int w = (int)rect.w, h = (int)rect.h;

    // ✅ Rajouter ces deux lignes ici :
    SDL_Color bodyCol   = dashing ? SDL_Color{190, 235, 255, 255}
                                  : SDL_Color{ 70, 130, 220, 255};
    SDL_Color borderCol = dashing ? SDL_Color{100, 200, 255, 255}
                                  : SDL_Color{ 30,  60, 140, 255};

    // ── Corps ────────────────────────────────────────────────
    SDL_SetRenderDrawColor(r, bodyCol.r, bodyCol.g, bodyCol.b, bodyCol.a);
    SDL_Rect body = {x, y, w, h};
    SDL_RenderFillRect(r, &body);
    SDL_SetRenderDrawColor(r, borderCol.r, borderCol.g, borderCol.b, borderCol.a);
    SDL_RenderDrawRect(r, &body);

    if (!dashing) {
        // ── Yeux ─────────────────────────────────────────────
        int px = (facingDir > 0) ? 2 : 0;   // pupilles regardent dans la direction
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        SDL_Rect eL = {x+3,       y+9,  8, 9};
        SDL_Rect eR = {x+w-11,    y+9,  8, 9};
        SDL_RenderFillRect(r, &eL); SDL_RenderFillRect(r, &eR);

        SDL_SetRenderDrawColor(r, 20, 20, 80, 255);
        SDL_Rect pL = {x+3+px,    y+12, 4, 5};
        SDL_Rect pR = {x+w-11+px, y+12, 4, 5};
        SDL_RenderFillRect(r, &pL); SDL_RenderFillRect(r, &pR);

        // ── Bouche ────────────────────────────────────────────
        SDL_SetRenderDrawColor(r, 200, 70, 70, 255);
        SDL_Rect mouth = {x+6, y+h-10, w-12, 3};
        SDL_RenderFillRect(r, &mouth);
    }
    else {
        // ── Traînée de dash (lignes blanches) ─────────────────
        SDL_SetRenderDrawColor(r, 255, 255, 255, 200);
        int trailDir = (facingDir > 0) ? -1 : 1;
        for (int i = 1; i <= 3; i++) {
            int tx = x + trailDir * i * 10;
            int alpha = 255 - i * 70;
            SDL_SetRenderDrawColor(r, 190, 230, 255, (Uint8)alpha);
            SDL_Rect trail = {tx, y+4, w, h-8};
            SDL_RenderFillRect(r, &trail);
        }
    }

    // ── Griffes sur les murs (wall slide) ────────────────────
    if (!onGround && (touchWallL || touchWallR)) {
        SDL_SetRenderDrawColor(r, 255, 210, 50, 255);
        int wx = touchWallR ? x + w + 1 : x - 5;
        for (int i = 0; i < 3; i++) {
            int cy = y + 8 + i * 11;
            SDL_RenderDrawLine(r, wx, cy, wx + (touchWallR ? 4 : -4), cy + 5);
        }
    }
}
