#include "objects.h"
#include "types.h"
#include "framebuffer.h"
#include "line.h"
#include "circle.h"
#include "polygon.h"
#include "clipping.h"
#include <vector>

// ---------------------------------------------------------------------------
// Integrante 6: semaforo, senal de PARE, sol y nubes.
//   - Sol y nubes      : circulos de punto medio (circle.h)
//   - Semaforo         : poste/caja (lineas Bresenham) + 3 circulos de punto medio
//   - Senal de PARE    : octagono relleno con scan-line (polygon.h) + texto
//                        dibujado con lineas Bresenham
//   - Clipping         : lineas con clipLine(g_window) y poligonos con
//                        clipPolygon(g_window) antes de dibujarse.
// Coordenadas en la grilla 320x240, Y hacia ARRIBA. Las posiciones salen de la
// imagen de referencia del enunciado.
// Los objetos se guardan en estructuras (vectores) y luego se recorren.
// ---------------------------------------------------------------------------

namespace {

const int GROUND_Y = 60;   // nivel del suelo / borde superior de la carretera

// ----- estructuras de datos ---------------------------------------------------

struct Sun   { int cx, cy, r; Color fill, edge; };
struct Blob  { int dx, dy, r; };                        // circulo de una nube
struct Cloud { int cx, cy; Color color; std::vector<Blob> blobs; };
struct TrafficLight { int x; int groundY; int boxW, boxH; };   // x = centro del poste
struct StopSign { int cx, cy; int poleBottomY; };

// ----- helpers de dibujo -----------------------------------------------------

void lineClipped(int x0, int y0, int x1, int y1, Color c)
{
    if (clipLine(x0, y0, x1, y1, g_window))
        drawLineBresenham(x0, y0, x1, y1, c);
}

void fillRect(int x0, int y0, int x1, int y1, Color c)
{
    if (x0 < g_window.xmin) x0 = g_window.xmin;
    if (y0 < g_window.ymin) y0 = g_window.ymin;
    if (x1 > g_window.xmax) x1 = g_window.xmax;
    if (y1 > g_window.ymax) y1 = g_window.ymax;
    for (int y = y0; y <= y1; y++)
        for (int x = x0; x <= x1; x++)
            putPixel(x, y, c);
}

void rectOutline(int x0, int y0, int x1, int y1, Color c)
{
    lineClipped(x0, y0, x1, y0, c);
    lineClipped(x1, y0, x1, y1, c);
    lineClipped(x1, y1, x0, y1, c);
    lineClipped(x0, y1, x0, y0, c);
}

// Poligono relleno (scan-line, Integrante 4) con contorno, recortado antes
// contra la ventana (Sutherland-Hodgman, Integrante 5).
void polygonFilled(const std::vector<Vec2>& pts, Color fill, Color edge)
{
    std::vector<Vec2> poly = clipPolygon(pts, g_window);
    if (poly.size() < 3) return;
    fillPolygonScanline(poly, fill);
    drawPolygonOutline(poly, edge);
}

// ----- sol y nubes -----------------------------------------------------------

void drawSun(const Sun& s)
{
    fillCircleMidpoint(s.cx, s.cy, s.r, s.fill);
    drawCircleMidpoint(s.cx, s.cy, s.r, s.edge);
}

void drawCloud(const Cloud& c)
{
    // Union de circulos rellenos (punto medio) = silueta de nube.
    for (const Blob& b : c.blobs)
        fillCircleMidpoint(c.cx + b.dx, c.cy + b.dy, b.r, c.color);
}

// ----- semaforo --------------------------------------------------------------

void drawTrafficLight(const TrafficLight& t)
{
    const Color metal   = rgb(35, 35, 38);
    const Color outline = rgb(10, 10, 12);

    // Poste (rectangulo delgado) y base
    fillRect(t.x - 1, t.groundY, t.x + 1, t.groundY + 50, metal);
    fillRect(t.x - 4, t.groundY, t.x + 4, t.groundY + 2, rgb(70, 110, 60));

    // Caja del semaforo
    const int x0 = t.x - t.boxW / 2, x1 = t.x + t.boxW / 2;
    const int y0 = t.groundY + 50,   y1 = y0 + t.boxH;
    fillRect(x0, y0, x1, y1, metal);
    rectOutline(x0, y0, x1, y1, outline);

    // Tres luces (circulos de punto medio): rojo, amarillo, verde
    const int cx = t.x;
    const int step = t.boxH / 3;
    const int ry = y1 - step / 2 - 1;
    const int r = step / 2 - 1;
    const Color lights[3] = { rgb(225, 45, 45), rgb(245, 205, 45), rgb(45, 175, 65) };
    for (int i = 0; i < 3; i++) {
        const int cy = ry - i * step;
        fillCircleMidpoint(cx, cy, r, lights[i]);
        drawCircleMidpoint(cx, cy, r, outline);
    }
}

// ----- senal de PARE ---------------------------------------------------------

// Letras de 3x5 como segmentos (x0,y0,x1,y1) en celdas; Y hacia arriba.
struct Seg { int x0, y0, x1, y1; };
const std::vector<Seg> GLYPH_P = { {0,0,0,4}, {0,4,2,4}, {2,4,2,2}, {2,2,0,2} };
const std::vector<Seg> GLYPH_A = { {0,0,0,3}, {0,3,1,4}, {1,4,2,3}, {2,3,2,0}, {0,2,2,2} };
const std::vector<Seg> GLYPH_R = { {0,0,0,4}, {0,4,2,4}, {2,4,2,2}, {2,2,0,2}, {1,2,2,0} };
const std::vector<Seg> GLYPH_E = { {0,0,0,4}, {0,4,2,4}, {0,2,1,2}, {0,0,2,0} };

void drawGlyph(const std::vector<Seg>& g, int ox, int oy, Color c)
{
    for (const Seg& s : g)
        lineClipped(ox + s.x0, oy + s.y0, ox + s.x1, oy + s.y1, c);
}

// Octagono regular aproximado de "radio" k alrededor de (cx,cy).
std::vector<Vec2> octagon(int cx, int cy, int a, int b)
{
    // a = semi-lado corto, b = semi-ancho: vertices (+-a,+-b) y (+-b,+-a)
    return { {cx + a, cy + b}, {cx + b, cy + a}, {cx + b, cy - a}, {cx + a, cy - b},
             {cx - a, cy - b}, {cx - b, cy - a}, {cx - b, cy + a}, {cx - a, cy + b} };
}

void drawStopSign(const StopSign& s)
{
    const Color red    = rgb(205, 35, 40);
    const Color dark   = rgb(120, 15, 20);
    const Color white  = rgb(250, 250, 250);
    const Color pole   = rgb(150, 150, 155);

    // Poste gris
    fillRect(s.cx - 1, s.poleBottomY, s.cx + 1, s.cy - 12, pole);

    // Octagono: relleno rojo (scan-line) con borde oscuro, y aro blanco interior
    polygonFilled(octagon(s.cx, s.cy, 5, 12), red, dark);
    drawPolygonOutline(clipPolygon(octagon(s.cx, s.cy, 4, 10), g_window), white);

    // Texto "PARE" (4 letras de 3 px + 1 px de espacio = 15 px de ancho)
    const int ox = s.cx - 7, oy = s.cy - 2;
    drawGlyph(GLYPH_P, ox,      oy, white);
    drawGlyph(GLYPH_A, ox + 4,  oy, white);
    drawGlyph(GLYPH_R, ox + 8,  oy, white);
    drawGlyph(GLYPH_E, ox + 12, oy, white);
}

} // namespace

// Integrante 6: cielo (sol, nubes) y senalizacion (semaforo, PARE).
void drawSkyAndSigns()
{
    // Estructuras de datos con los objetos de esta parte de la escena.
    const Sun sun = { 155, 198, 13, rgb(255, 225, 110), rgb(190, 150, 30) };

    const Color white = rgb(250, 250, 252);
    const std::vector<Blob> puffs = { {-11, -1, 5}, {-4, 2, 7}, {4, 3, 8}, {12, 0, 6}, {0, -2, 6} };
    const std::vector<Cloud> clouds = {
        { 93,  218, white, puffs },
        { 205, 203, white, puffs },
        { 256, 216, white, puffs },
    };

    const TrafficLight light = { 16, GROUND_Y, 12, 36 };
    const StopSign stop      = { 300, 117, GROUND_Y };

    drawSun(sun);
    for (const Cloud& c : clouds) drawCloud(c);
    drawTrafficLight(light);
    drawStopSign(stop);
}