#pragma once

struct Color  { float r, g, b; };
struct Vec2   { int x, y; };
struct Window { int xmin, ymin, xmax, ymax; };

// Crea un Color desde valores 0-255
inline Color rgb(int r, int g, int b) {
    return { r / 255.0f, g / 255.0f, b / 255.0f };
}
