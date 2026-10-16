#include "line.h"
#include "framebuffer.h"

// TODO (Integrante 2): Bresenham para todos los octantes.
void drawLineBresenham(int x0, int y0, int x1, int y1, Color c)
{
    putPixel(x0, y0, c);   // stub: solo los extremos
    putPixel(x1, y1, c);
}
