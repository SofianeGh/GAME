#include "hud.hpp"

// ── Cœur pixel-art (18×18) ────────────────────────────────────
static void drawHeart(SDL_Renderer* r, int x, int y, bool filled)
{
    // Masque du cœur en petits rectangles
    SDL_Color c = filled ? SDL_Color{220, 50, 60, 255}
                         : SDL_Color{ 70, 70, 70, 255};
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);

    //  Rangée 0 :  ##  ##
    SDL_Rect r0a = {x+2, y,   5, 4};  SDL_RenderFillRect(r, &r0a);
    SDL_Rect r0b = {x+9, y,   5, 4};  SDL_RenderFillRect(r, &r0b);
    //  Rangée 1 : ########
    SDL_Rect r1  = {x,   y+3, 16, 5}; SDL_RenderFillRect(r, &r1);
    //  Rangée 2 : ########
    SDL_Rect r2  = {x+1, y+7, 14, 5}; SDL_RenderFillRect(r, &r2);
    //  Rangée 3 :  ######
    SDL_Rect r3  = {x+3, y+11, 10, 4}; SDL_RenderFillRect(r, &r3);
    //  Rangée 4 :   ####
    SDL_Rect r4  = {x+5, y+14,  6, 3}; SDL_RenderFillRect(r, &r4);
    //  Rangée 5 :    ##
    SDL_Rect r5  = {x+7, y+16,  2, 2}; SDL_RenderFillRect(r, &r5);

    // Reflet (uniquement sur cœur plein)
    if (filled) {
        SDL_SetRenderDrawColor(r, 255, 130, 140, 255);
        SDL_Rect shine = {x+3, y+3, 4, 3}; SDL_RenderFillRect(r, &shine);
    }
}

// ── Icône dash (éclair) ───────────────────────────────────────
static void drawDashIcon(SDL_Renderer* r, int x, int y, float ratio)
{
    // Fond
    SDL_SetRenderDrawColor(r, 40, 40, 40, 255);
    SDL_Rect bg = {x, y, 70, 14}; SDL_RenderFillRect(r, &bg);

    // Remplissage de la jauge
    SDL_Color fill = (ratio >= 1.f) ? SDL_Color{60, 200, 255, 255}
                                    : SDL_Color{30, 100, 160, 255};
    SDL_SetRenderDrawColor(r, fill.r, fill.g, fill.b, fill.a);
    SDL_Rect bar = {x, y, (int)(70 * ratio), 14}; SDL_RenderFillRect(r, &bar);

    // Bordure
    SDL_SetRenderDrawColor(r, 140, 140, 140, 255);
    SDL_RenderDrawRect(r, &bg);

    // Petit éclair pixel au centre de l'icône
    SDL_SetRenderDrawColor(r, 255, 230, 50, 255);
    SDL_RenderDrawLine(r, x+37, y+2, x+33, y+7);
    SDL_RenderDrawLine(r, x+33, y+7, x+37, y+7);
    SDL_RenderDrawLine(r, x+37, y+7, x+33, y+12);
}

// ── Barre de HUD (fond semi-transparent en haut) ──────────────
void drawHUD(SDL_Renderer* r, const Player& p,
             int levelIdx, int totalLevels)
{
    // Bande de fond
    SDL_SetRenderDrawColor(r, 0, 0, 0, 160);
    SDL_Rect band = {0, 0, SCREEN_W, 44}; SDL_RenderFillRect(r, &band);

    // ── Cœurs HP ─────────────────────────────────────────────
    for (int i = 0; i < p.maxHp; i++)
        drawHeart(r, 10 + i * 24, 12, i < p.hp);

    // ── Jauge dash ────────────────────────────────────────────
    drawDashIcon(r, 90, 15, p.dashReadyRatio());

    // ── Indicateur de niveau (petits carrés colorés) ──────────
    for (int i = 0; i < totalLevels; i++) {
        SDL_Color c = (i == levelIdx) ? SDL_Color{255, 200, 50, 255}
                                      : SDL_Color{ 80,  80, 80, 255};
        SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
        SDL_Rect dot = {SCREEN_W - totalLevels*18 + i*18, 16, 12, 12};
        SDL_RenderFillRect(r, &dot);
    }
}
