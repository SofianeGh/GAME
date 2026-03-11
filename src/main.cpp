// ============================================================
//  Mini Platformer - SDL2
//  Compilation :
//    Linux/macOS : g++ main.cpp -o platformer $(sdl2-config --cflags --libs)
//    Windows     : g++ main.cpp -o platformer -lSDL2main -lSDL2
// ============================================================

#include <SDL2/SDL.h>
#include <vector>

// ── Constantes fenêtre ────────────────────────────────────────
constexpr int   SCREEN_W   = 800;
constexpr int   SCREEN_H   = 500;
constexpr int   FPS        = 60;
constexpr float DELTA      = 1.0f / FPS;

// ── Physique ──────────────────────────────────────────────────
constexpr float GRAVITY      = 1400.0f;   // px/s²
constexpr float JUMP_FORCE   = -520.0f;   // px/s  (négatif = vers le haut)
constexpr float MOVE_SPEED   = 220.0f;    // px/s
constexpr float MAX_FALL_VEL = 900.0f;    // px/s

// ─────────────────────────────────────────────────────────────
struct Rect { float x, y, w, h; };

// Retourne true si deux rectangles se chevauchent
bool overlaps(const Rect& a, const Rect& b)
{
    return a.x < b.x + b.w && a.x + a.w > b.x &&
           a.y < b.y + b.h && a.y + a.h > b.y;
}

// ── Joueur ────────────────────────────────────────────────────
struct Player {
    Rect  rect  = {100.f, 200.f, 36.f, 48.f};
    float vx    = 0.f;
    float vy    = 0.f;
    bool  onGround = false;

    void handleInput(const Uint8* keys)
    {
        vx = 0.f;
        if (keys[SDL_SCANCODE_LEFT]  || keys[SDL_SCANCODE_A]) vx = -MOVE_SPEED;
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) vx =  MOVE_SPEED;

        if ((keys[SDL_SCANCODE_SPACE] || keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
            && onGround)
        {
            vy = JUMP_FORCE;
            onGround = false;
        }
    }

    void update(float dt, const std::vector<Rect>& platforms)
    {
        // ── Appliquer la gravité ──────────────────────────────
        vy += GRAVITY * dt;
        if (vy > MAX_FALL_VEL) vy = MAX_FALL_VEL;

        // ── Déplacement horizontal puis collision ─────────────
        rect.x += vx * dt;
        for (const auto& p : platforms)
        {
            if (overlaps(rect, p))
            {
                if (vx > 0) rect.x = p.x - rect.w;
                else        rect.x = p.x + p.w;
                vx = 0.f;
            }
        }

        // ── Déplacement vertical puis collision ───────────────
        onGround = false;
        rect.y += vy * dt;
        for (const auto& p : platforms)
        {
            if (overlaps(rect, p))
            {
                if (vy > 0)          // chute → on pose le perso
                {
                    rect.y   = p.y - rect.h;
                    onGround = true;
                }
                else                 // montée → rebond plafond
                {
                    rect.y = p.y + p.h;
                }
                vy = 0.f;
            }
        }

        // ── Bornes de l'écran (gauche/droite) ─────────────────
        if (rect.x < 0)                 rect.x = 0;
        if (rect.x + rect.w > SCREEN_W) rect.x = SCREEN_W - rect.w;

        // ── Respawn si on tombe en-dessous de l'écran ─────────
        if (rect.y > SCREEN_H + 100)
        {
            rect.x = 100.f;
            rect.y = 200.f;
            vy     = 0.f;
        }
    }

    void draw(SDL_Renderer* renderer) const
    {
        int x = (int)rect.x, y = (int)rect.y;
        int w = (int)rect.w, h = (int)rect.h;

        // Corps bleu
        SDL_SetRenderDrawColor(renderer, 130, 130, 220, 255);
        SDL_Rect body = {x, y, w, h};
        SDL_RenderFillRect(renderer, &body);

        // Contour
        SDL_SetRenderDrawColor(renderer, 30, 60, 140, 255);
        SDL_RenderDrawRect(renderer, &body);
    }
};

// ── Dessin d'une plateforme avec bord coloré ──────────────────
void drawPlatform(SDL_Renderer* r, const Rect& p,
                  SDL_Color top, SDL_Color body)
{
    SDL_SetRenderDrawColor(r, body.r, body.g, body.b, body.a);
    SDL_Rect rect = {(int)p.x, (int)p.y, (int)p.w, (int)p.h};
    SDL_RenderFillRect(r, &rect);

    SDL_SetRenderDrawColor(r, top.r, top.g, top.b, top.a);
    SDL_Rect topLine = {(int)p.x, (int)p.y, (int)p.w, 6};
    SDL_RenderFillRect(r, &topLine);
}

// ── Point d'entrée ────────────────────────────────────────────
int main(int /*argc*/, char* /*argv*/[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Mini Platformer  —  ZQSD / fleches + ESPACE pour sauter",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // ── Liste des plateformes  {x, y, largeur, hauteur} ───────
    std::vector<Rect> platforms = {
        // Sol
        {  0.f, 460.f, 800.f, 40.f},
        // Plateformes flottantes
        { 80.f, 400.f, 130.f, 10.f},

    };

    SDL_Color grassGreen = { 80, 160,  50, 255};
    SDL_Color dirtBrown  = {120,  80,  40, 255};
    SDL_Color floatTop   = { 60, 200, 100, 255};
    SDL_Color floatBody  = { 90, 130,  60, 255};

    Player player;
    bool running = true;
    SDL_Event event;

    // ── Boucle principale ─────────────────────────────────────
    while (running)
    {
        // Événements
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        player.handleInput(keys);
        player.update(DELTA, platforms);

        // ── Fond ciel bleu ────────────────────────────────────
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);


        // Plateformes
        for (size_t i = 0; i < platforms.size(); ++i)
        {
            if (i == 0) drawPlatform(renderer, platforms[i], grassGreen, dirtBrown);
            else        drawPlatform(renderer, platforms[i], floatTop,   floatBody);
        }

        // Joueur
        player.draw(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}