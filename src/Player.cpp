#include "Player.h"
#include "Constants.h"

void Player::handleInput(const Uint8* keys)
{
    vx = 0.f;
    if (!isDashing)
    {
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) { vx = -MOVE_SPEED; dashDir = -1; }
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) { vx = MOVE_SPEED; dashDir = 1; }

        if ((keys[SDL_SCANCODE_SPACE] || keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) && onGround)
        {
            vy = JUMP_FORCE;
            onGround = false;
        }

        if (keys[SDL_SCANCODE_X] && dashCooldown <= 0.f)
        {
            isDashing = true;
            dashTimer = DASH_DURATION;
            dashCooldown = DASH_COOLDOWN;
        }
    }
}

void Player::update(float dt, const std::vector<Platform>& platforms)
{
    if (isDashing)
    {
        vx = dashDir * DASH_SPEED;
        vy = 0.f;
        dashTimer -= dt;
        if (dashTimer <= 0.f) isDashing = false;
    }
    else
    {
        vy += GRAVITY * dt;
        if (vy > MAX_FALL_VEL) vy = MAX_FALL_VEL;
    }

    if (dashCooldown > 0.f) dashCooldown -= dt;

    // Horizontal collision
    rect.x += vx * dt;
    for (const auto& p : platforms)
    {
        const Rect& plat = p.getRect();

        if (overlaps(rect, plat))
        {
            rect.x = vx > 0 ? plat.x - rect.w : plat.x + plat.w;
            vx = 0.f;
        }
    }

    // Vertical collision
    onGround = false;
    rect.y += vy * dt;
    for (const auto& p : platforms)
    {
        const Rect& plat = p.getRect();

        if (overlaps(rect, plat))
        {
            if (vy > 0)
            {
                rect.y = plat.y - rect.h;
                onGround = true;
            }
            else
            {
                rect.y = plat.y + plat.h;
            }
            vy = 0.f;
        }
    }

    // Screen limits
    if (rect.x < 0) rect.x = 0;
    if (rect.x + rect.w > SCREEN_W) rect.x = SCREEN_W - rect.w;

    // Respawn
    if (rect.y > SCREEN_H + 100) { rect.x = 100.f; rect.y = 200.f; vy = 0.f; }
}

void Player::draw(SDL_Renderer* renderer) const
{
    SDL_Rect body = {(int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h};
    if (isDashing) SDL_SetRenderDrawColor(renderer, 255, 200, 50, 255);
    else SDL_SetRenderDrawColor(renderer, 130, 130, 220, 255);

    SDL_RenderFillRect(renderer, &body);
    SDL_SetRenderDrawColor(renderer, 30, 60, 140, 255);
    SDL_RenderDrawRect(renderer, &body);
}