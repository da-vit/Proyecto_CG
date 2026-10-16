#include "polygon.h"
#include "framebuffer.h"
#include "line.h"

// TODO (Integrante 4): scan-line con tabla de aristas y aristas activas.
void fillPolygonScanline(const std::vector<Vec2>& pts, Color c)
{
    // stub
}

void drawPolygonOutline(const std::vector<Vec2>& pts, Color c)
{
    const size_t n = pts.size();
    for (size_t i = 0; i < n; i++) {
        const Vec2& a = pts[i];
        const Vec2& b = pts[(i + 1) % n];
        drawLineBresenham(a.x, a.y, b.x, b.y, c);
    }
}
