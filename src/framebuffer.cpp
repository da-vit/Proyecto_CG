#include "framebuffer.h"
#include "config.h"
#include <GLFW/glfw3.h>
#include <vector>

static std::vector<Color> buffer(GRID_WIDTH * GRID_HEIGHT, Color{0, 0, 0});

void putPixel(int x, int y, Color c)
{
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return;
    buffer[y * GRID_WIDTH + x] = c;
}

void clearBuffer(Color c)
{
    for (auto& p : buffer) p = c;
}

void presentBuffer()
{
    const float dx = 2.0f / GRID_WIDTH;
    const float dy = 2.0f / GRID_HEIGHT;

    glBegin(GL_QUADS);
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            const Color& c = buffer[y * GRID_WIDTH + x];
            const float x0 = -1.0f + x * dx;
            const float y0 = -1.0f + y * dy;
            glColor3f(c.r, c.g, c.b);
            glVertex2f(x0,      y0);
            glVertex2f(x0 + dx, y0);
            glVertex2f(x0 + dx, y0 + dy);
            glVertex2f(x0,      y0 + dy);
        }
    }
    glEnd();
}
