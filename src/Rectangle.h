#pragma once
#include "Constants.h"

struct Rect { float x, y, w, h; };

inline bool overlaps(const Rect& a, const Rect& b)
{
    return a.x < b.x + b.w && a.x + a.w > b.x &&
           a.y < b.y + b.h && a.y + a.h > b.y;
}