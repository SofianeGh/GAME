#include "level.hpp"

std::vector<Level> buildLevels()
{
    std::vector<Level> levels;

    // ─────────────────────────────────────────────────────────
    // NIVEAU 1 : Les Plainess  (tutoriel doux, ciel bleu)
    // Objectif : apprendre le saut et le dash
    // ─────────────────────────────────────────────────────────
    {
        Level lv;
        lv.name     = "Les Plaines";
        lv.startX   = 60.f;  lv.startY = 390.f;
        lv.bgColor    = {100, 180, 255, 255};
        lv.groundColor= { 80, 160,  50, 255};
        lv.platColor  = { 55, 120,  35, 255};

        // Sol
        lv.platforms.push_back({{ 0.f, 460.f, 800.f, 40.f}, true});

        // Plateformes montantes (de gauche à droite)
        lv.platforms.push_back({{110.f, 390.f, 120.f, 14.f}});
        lv.platforms.push_back({{300.f, 330.f, 110.f, 14.f}});
        lv.platforms.push_back({{460.f, 265.f, 120.f, 14.f}});
        lv.platforms.push_back({{630.f, 310.f, 110.f, 14.f}});
        lv.platforms.push_back({{350.f, 405.f,  80.f, 14.f}});  // stepping stone

        // Mur vertical pour montrer le wall-jump
        lv.platforms.push_back({{675.f, 200.f,  16.f, 110.f}});
        lv.platforms.push_back({{590.f, 200.f,  16.f,  80.f}});

        // Piques au sol (zones de danger)
        lv.spikes.push_back({{205.f, 450.f,  72.f, 10.f}});
        lv.spikes.push_back({{440.f, 450.f,  60.f, 10.f}});

        // Checkpoint au milieu du niveau
        lv.checkpoints.push_back({{356.f, 387.f, 20.f, 32.f}, false, 360.f, 375.f});

        // Sortie en haut à droite (accessible via wall-jump)
        lv.exit = {{740.f, 95.f, 34.f, 56.f}};

        levels.push_back(lv);
    }

    // ─────────────────────────────────────────────────────────
    // NIVEAU 2 : Les Ruines  (nuit violette, plus difficile)
    // Objectif : dash obligatoire pour franchir les gouffres
    // ─────────────────────────────────────────────────────────
    {
        Level lv;
        lv.name     = "Les Ruines";
        lv.startX   = 50.f;  lv.startY = 390.f;
        lv.bgColor    = { 22,  14,  42, 255};
        lv.groundColor= { 72,  55,  90, 255};
        lv.platColor  = { 55,  42,  75, 255};

        // Deux morceaux de sol séparés par un gouffre (force le dash)
        lv.platforms.push_back({{  0.f, 460.f, 280.f, 40.f}, true});
        lv.platforms.push_back({{520.f, 460.f, 280.f, 40.f}, true});

        // Série de petites plateformes au-dessus du gouffre
        lv.platforms.push_back({{150.f, 370.f,  80.f, 14.f}});
        lv.platforms.push_back({{295.f, 305.f,  75.f, 14.f}});
        lv.platforms.push_back({{420.f, 240.f,  75.f, 14.f}});
        lv.platforms.push_back({{560.f, 305.f,  80.f, 14.f}});
        lv.platforms.push_back({{680.f, 370.f,  80.f, 14.f}});

        // Section wall-jump à gauche (escalader pour atteindre la sortie)
        lv.platforms.push_back({{ 20.f, 210.f,  16.f, 150.f}});
        lv.platforms.push_back({{100.f, 185.f,  16.f, 175.f}});
        lv.platforms.push_back({{180.f, 200.f, 110.f,  14.f}});  // plateforme haute

        // Piques dans le gouffre et sur certaines plateformes
        lv.spikes.push_back({{280.f, 450.f, 240.f, 10.f}});       // gouffre
        lv.spikes.push_back({{295.f, 291.f,  75.f,  8.f}});       // dessus plateau
        lv.spikes.push_back({{560.f, 291.f,  80.f,  8.f}});       // dessus plateau

        // Checkpoint sur le sol de droite
        lv.checkpoints.push_back({{560.f, 420.f, 20.f, 32.f}, false, 565.f, 440.f});

        // Sortie en haut à gauche (atteinte via wall-jump)
        lv.exit = {{135.f, 70.f, 34.f, 56.f}};

        levels.push_back(lv);
    }

    return levels;
}
