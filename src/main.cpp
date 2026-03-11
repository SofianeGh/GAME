// ============================================================
//  GAME — Platformer  (SDL2, C++17)
//  Compilation : make   (voir Makefile)
//  Contrôles   : WASD / flèches  →  déplacement + saut
//                SHIFT ou X      →  dash
//                Échap           →  menu / quitter
// ============================================================

#include <SDL2/SDL.h>
#include "common.hpp"
#include "level.hpp"
#include "player.hpp"
#include "hud.hpp"
#include <vector>
#include <cmath>

// ── États du jeu ─────────────────────────────────────────────
enum class GameState { MENU, PLAYING, DEAD, WIN };

// ─────────────────────────────────────────────────────────────
//  Dessin du niveau (plateformes, piques, checkpoints, sortie)
// ─────────────────────────────────────────────────────────────
static void drawPlatform(SDL_Renderer* r, const FRect& p,
                         SDL_Color top, SDL_Color body)
{
    drawFilled(r, p, body);
    drawFilled(r, {p.x, p.y, p.w, 6.f}, top);   // bandeau du dessus
}
player.init(100.f, 200.f);
player.loadTexture(renderer, "assets/char_32x32.png");  // ✅ ici renderer est connu
static void drawSpike(SDL_Renderer* r, const FRect& s)
{
    // Triangles rouges répétés sur toute la largeur
    int n = (int)(s.w / 12.f);
    SDL_SetRenderDrawColor(r, 200, 50, 50, 255);
    for (int i = 0; i < n; i++) {
        float bx = s.x + i * 12.f;
        float by = s.y + s.h;
        float tx = bx + 6.f, ty = s.y;
        SDL_RenderDrawLine(r, (int)bx, (int)by, (int)tx, (int)ty);
        SDL_RenderDrawLine(r, (int)tx, (int)ty, (int)(bx+12), (int)by);
        SDL_RenderDrawLine(r, (int)bx, (int)by, (int)(bx+12), (int)by);
        // Remplissage simplifié (2 lignes diagonales internes)
        SDL_SetRenderDrawColor(r, 160, 40, 40, 255);
        SDL_RenderDrawLine(r, (int)(bx+3),(int)by, (int)tx,(int)(ty+4));
        SDL_RenderDrawLine(r, (int)(bx+9),(int)by, (int)tx,(int)(ty+4));
        SDL_SetRenderDrawColor(r, 200, 50, 50, 255);
    }
}

static void drawCheckpoint(SDL_Renderer* r, const Checkpoint& c)
{
    // Poteau
    drawFilled(r, {c.rect.x + 7.f, c.rect.y, 5.f, c.rect.h},
               {100, 70, 35, 255});
    // Drapeau (gris si non activé, doré si activé)
    SDL_Color flag = c.active ? SDL_Color{255, 200, 40, 255}
                              : SDL_Color{100, 100, 100, 255};
    drawFilled(r, {c.rect.x + 12.f, c.rect.y, 15.f, 12.f}, flag);
    if (c.active) {
        // Étoile/reflet sur le drapeau activé
        SDL_SetRenderDrawColor(r, 255, 255, 150, 255);
        SDL_Rect shine = {(int)c.rect.x+14, (int)c.rect.y+2, 5, 4};
        SDL_RenderFillRect(r, &shine);
    }
}

static void drawExit(SDL_Renderer* r, const LevelExit& e, float t)
{
    // Porte dorée
    drawFilled(r, e.rect, {180, 130, 50, 255});
    drawOutline(r, e.rect, {240, 180, 60, 255});
    // Poignée
    drawFilled(r, {e.rect.x + e.rect.w - 8.f, e.rect.y + e.rect.h/2 - 3.f, 5.f, 6.f},
               {255, 215, 0, 255});
    // Étoile pulsante au-dessus
    float pulse = 0.7f + 0.3f * std::sin(t * 4.f);
    int sz = (int)(16 * pulse);
    int sx = (int)(e.rect.x + e.rect.w/2 - sz/2);
    int sy = (int)(e.rect.y - sz - 6);
    SDL_SetRenderDrawColor(r, 255, 220, 50, 255);
    SDL_Rect star = {sx, sy, sz, sz}; SDL_RenderFillRect(r, &star);
}

static void drawLevel(SDL_Renderer* r, const Level& lv, float t)
{
    SDL_Color groundTop  = lv.groundColor;
    SDL_Color groundBody = {(Uint8)(lv.groundColor.r*3/4),
                            (Uint8)(lv.groundColor.g*3/4),
                            (Uint8)(lv.groundColor.b*3/4), 255};
    SDL_Color platTop  = lv.platColor;
    SDL_Color platBody = {(Uint8)(lv.platColor.r*3/4),
                          (Uint8)(lv.platColor.g*3/4),
                          (Uint8)(lv.platColor.b*3/4), 255};

    for (auto& p : lv.platforms) {
        if (p.isGround) drawPlatform(r, p.rect, groundTop, groundBody);
        else            drawPlatform(r, p.rect, platTop,   platBody);
    }
    for (auto& s : lv.spikes)      drawSpike(r, s.rect);
    for (auto& c : lv.checkpoints) drawCheckpoint(r, c);
    drawExit(r, lv.exit, t);
}

// ─────────────────────────────────────────────────────────────
//  Écrans de state
// ─────────────────────────────────────────────────────────────
static void drawMenuScreen(SDL_Renderer* r, float t)
{
    SDL_SetRenderDrawColor(r, 12, 8, 28, 255);
    SDL_RenderClear(r);

    // Titre (gros bloc stylisé)
    SDL_SetRenderDrawColor(r, 70, 130, 220, 255);
    SDL_Rect title = {200, 100, 400, 90}; SDL_RenderFillRect(r, &title);
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderDrawRect(r, &title);

    // Lettre "G" pixel-art dans le titre
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_Rect g1={230,120,40,10}, g2={220,120,10,50}, g3={230,155,50,10},
             g4={270,140,10,25}, g5={240,140,30,5};
    SDL_RenderFillRect(r,&g1); SDL_RenderFillRect(r,&g2);
    SDL_RenderFillRect(r,&g3); SDL_RenderFillRect(r,&g4);
    SDL_RenderFillRect(r,&g5);

    // Bouton "ENTER" qui pulse
    float p = 0.5f + 0.5f * std::sin(t * 3.f);
    SDL_SetRenderDrawColor(r, (Uint8)(100*p), (Uint8)(180*p), (Uint8)(255*p), 255);
    SDL_Rect btn = {300, 240, 200, 36}; SDL_RenderFillRect(r, &btn);
    SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
    SDL_RenderDrawRect(r, &btn);

    // Légende contrôles (petits blocs)
    SDL_SetRenderDrawColor(r, 50, 50, 50, 255);
    SDL_Rect leg = {240, 310, 320, 130}; SDL_RenderFillRect(r, &leg);
    SDL_SetRenderDrawColor(r, 80, 80, 80, 255);
    SDL_RenderDrawRect(r, &leg);

    // Icônes WASD
    int kx[] = {268, 298, 328, 358};
    for (int i=0; i<4; i++) {
        SDL_SetRenderDrawColor(r, 90, 90, 130, 255);
        SDL_Rect key = {kx[i], 325, 24, 24}; SDL_RenderFillRect(r, &key);
        SDL_SetRenderDrawColor(r, 150, 150, 200, 255);
        SDL_RenderDrawRect(r, &key);
    }
    // Icône SHIFT (dash)
    SDL_SetRenderDrawColor(r, 60, 180, 255, 255);
    SDL_Rect shift = {268, 360, 80, 24}; SDL_RenderFillRect(r, &shift);
    SDL_SetRenderDrawColor(r, 100, 200, 255, 255);
    SDL_RenderDrawRect(r, &shift);

    // Petit perso dans le menu
    SDL_SetRenderDrawColor(r, 70, 130, 220, 255);
    SDL_Rect hero = {384, 320, 30, 40}; SDL_RenderFillRect(r, &hero);
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_Rect eye1 = {389, 328, 6, 7}, eye2 = {401, 328, 6, 7};
    SDL_RenderFillRect(r, &eye1); SDL_RenderFillRect(r, &eye2);
}

static void drawDeadOverlay(SDL_Renderer* r, float timer)
{
    // Assombrissement rouge
    SDL_SetRenderDrawColor(r, 140, 0, 0, 170);
    SDL_Rect ov = {0, 0, SCREEN_W, SCREEN_H}; SDL_RenderFillRect(r, &ov);

    // Grande croix rouge
    SDL_SetRenderDrawColor(r, 255, 40, 40, 255);
    SDL_Rect v = {385, 150, 30, 170}, h = {295, 200, 210, 40};
    SDL_RenderFillRect(r, &v); SDL_RenderFillRect(r, &h);

    // Hint "press R" une fois le timer écoulé
    if (timer <= 0.f) {
        float p = 0.5f + 0.5f * std::sin(timer * -8.f);
        SDL_SetRenderDrawColor(r, (Uint8)(180*p), (Uint8)(180*p), (Uint8)(180*p), 255);
        SDL_Rect hint = {310, 370, 180, 32}; SDL_RenderFillRect(r, &hint);
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        SDL_RenderDrawRect(r, &hint);
    }
}

static void drawWinScreen(SDL_Renderer* r, float t)
{
    SDL_SetRenderDrawColor(r, 0, 80, 0, 190);
    SDL_Rect ov = {0, 0, SCREEN_W, SCREEN_H}; SDL_RenderFillRect(r, &ov);

    // Étoiles qui tournent (simulation avec rectangles)
    for (int i = 0; i < 8; i++) {
        float angle = t * 1.5f + i * 3.14159f / 4.f;
        int ex = (int)(400 + 120 * std::cos(angle));
        int ey = (int)(200 + 80  * std::sin(angle));
        SDL_SetRenderDrawColor(r, 255, 220, 50, 255);
        SDL_Rect star = {ex-8, ey-8, 16, 16}; SDL_RenderFillRect(r, &star);
    }

    // Trophée central
    SDL_SetRenderDrawColor(r, 255, 200, 40, 255);
    SDL_Rect cup1 = {350, 160, 100, 80};
    SDL_Rect cup2 = {370, 240,  60, 20};
    SDL_Rect cup3 = {340, 258,  80, 14};
    SDL_RenderFillRect(r, &cup1);
    SDL_RenderFillRect(r, &cup2);
    SDL_RenderFillRect(r, &cup3);

    // "Press ENTER" pour revenir au menu
    float p = 0.5f + 0.5f * std::sin(t * 3.f);
    SDL_SetRenderDrawColor(r, (Uint8)(100*p), (Uint8)(200*p), (Uint8)(100*p), 255);
    SDL_Rect btn = {290, 360, 220, 34}; SDL_RenderFillRect(r, &btn);
}

// ─────────────────────────────────────────────────────────────
//  Point d'entrée
// ─────────────────────────────────────────────────────────────
int main(int, char*[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow(
        "GAME — Platformer  (WASD + SHIFT=dash)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);

    SDL_Renderer* rnd = SDL_CreateRenderer(
        win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_BLEND);

    // ── Chargement des niveaux ────────────────────────────────
    auto levels  = buildLevels();
    int  lvIdx   = 0;
    Player player;
    player.init(levels[0].startX, levels[0].startY);

    GameState state     = GameState::MENU;
    float     deadTimer = 1.5f;   // délai avant d'afficher "press R"
    float     timeAcc   = 0.f;    // temps global pour animations

    bool running = true;
    SDL_Event ev;

    while (running)
    {
        bool jumpPressed = false, dashPressed = false;

        // ── Événements ────────────────────────────────────────
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_QUIT) { running = false; break; }

            if (ev.type == SDL_KEYDOWN)
            {
                auto k = ev.key.keysym.sym;

                if (k == SDLK_ESCAPE) {
                    if (state == GameState::PLAYING) state = GameState::MENU;
                    else running = false;
                }

                if (state == GameState::MENU && k == SDLK_RETURN) {
                    lvIdx = 0;
                    for (auto& c : levels[lvIdx].checkpoints) c.active = false;
                    player.init(levels[lvIdx].startX, levels[lvIdx].startY);
                    state = GameState::PLAYING;
                }

                if (state == GameState::DEAD && deadTimer <= 0.f && k == SDLK_r) {
                    player.respawn();
                    state = GameState::PLAYING;
                }

                if (state == GameState::WIN && k == SDLK_RETURN)
                    state = GameState::MENU;

                if (state == GameState::PLAYING) {
                    if (k == SDLK_SPACE || k == SDLK_w || k == SDLK_UP)
                        jumpPressed = true;
                    if (k == SDLK_LSHIFT || k == SDLK_x)
                        dashPressed = true;
                }
            }
        }

        timeAcc += DELTA;
        Level& lv = levels[lvIdx];

        // ── Mise à jour ───────────────────────────────────────
        if (state == GameState::PLAYING)
        {
            const Uint8* keys = SDL_GetKeyboardState(nullptr);
            player.handleInput(keys, jumpPressed, dashPressed);
            player.update(DELTA, lv);

            // Sortie de niveau
            if (overlaps(player.rect, lv.exit.rect))
            {
                lvIdx++;
                if (lvIdx >= (int)levels.size()) {
                    state = GameState::WIN;
                } else {
                    for (auto& c : levels[lvIdx].checkpoints) c.active = false;
                    player.init(levels[lvIdx].startX, levels[lvIdx].startY);
                }
            }

            if (player.isDead()) {
                state     = GameState::DEAD;
                deadTimer = 1.5f;
            }
        }

        if (state == GameState::DEAD)
            deadTimer -= DELTA;

        // ── Rendu ─────────────────────────────────────────────
        if (state == GameState::MENU) {
            drawMenuScreen(rnd, timeAcc);
        }
        else {
            // Fond
            SDL_SetRenderDrawColor(rnd, lv.bgColor.r, lv.bgColor.g, lv.bgColor.b, 255);
            SDL_RenderClear(rnd);

            drawLevel(rnd, lv, timeAcc);
            player.draw(rnd);
            drawHUD(rnd, player, lvIdx, (int)levels.size());

            if (state == GameState::DEAD) drawDeadOverlay(rnd, deadTimer);
            if (state == GameState::WIN)  drawWinScreen(rnd, timeAcc);
        }

        SDL_RenderPresent(rnd);
    }

    SDL_DestroyRenderer(rnd);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
