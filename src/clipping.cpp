#include "clipping.h"
#include "config.h"

Window g_window = { 0, 0, GRID_WIDTH - 1, GRID_HEIGHT - 1 };

// TODO (Integrante 2)
bool clipLine(int& x0, int& y0, int& x1, int& y1, const Window& w)
{
    return true;   // stub: no recorta
}

// TODO (Integrante 5)
std::vector<Vec2> clipPolygon(const std::vector<Vec2>& pts, const Window& w)
{
    return pts;    // stub: no recorta
}
