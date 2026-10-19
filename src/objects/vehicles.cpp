#include "objects.h"
#include "types.h"
#include "framebuffer.h"
#include "line.h"
#include "circle.h"
#include "polygon.h"
#include "clipping.h"
#include <vector>
#include <algorithm>

// ---------------------------------------------------------------------------
// Integrante 3: vehiculos (auto y camion).
//   - Clipping: toda linea pasa por clipLine(g_window) antes de Bresenham y
//     los rellenos se recortan contra g_window; los circulos se recortan en
//     circle.cpp. Asi un vehiculo que salga de la ventana se corta bien.
//   - Carroceria / contornos : rectangulos y lineas con Bresenham (line.h)
//   - Ruedas, llantas, faros : circunferencias con punto medio (circle.h)
// Los vehiculos se guardan en una estructura de datos (vectores de Car/Truck)
// y luego se recorren para dibujarlos (requisito: no dibujar con instrucciones
// sueltas). El eje Y crece hacia ARRIBA (ver framebuffer.cpp).
// ---------------------------------------------------------------------------

namespace {

// ---------------------------------------------------------------------------
// Medidas tomadas de la imagen de referencia, convertidas a la grilla 320x240
// (Y hacia arriba). La carretera de la imagen va de y=0 a y~60, con la linea
// discontinua en y~34. Las ruedas apoyan en y = WHEEL_BASE_Y (carril inferior).
// Si Integrante 2 cambia la carretera, solo hay que tocar estas constantes.
// ---------------------------------------------------------------------------
const int WHEEL_BASE_Y = 17;   // borde inferior de las ruedas
const int WHEEL_R      = 6;
const int CAR_X        = 92;   // auto: x = 92..136
const int TRUCK_X      = 240;  // camion: x = 240..297

struct Car   { int x; int y; Color body; };      // (x,y) = esquina inf. izq. (base de ruedas)
struct Truck { int x; int y; Color cargo; Color cab; };

// ----- helpers de dibujo -----------------------------------------------------

// Linea recortada contra la ventana (Integrante 2) y trazada con Bresenham.
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

// Contorno de rectangulo con 4 lineas de Bresenham.
void rectOutline(int x0, int y0, int x1, int y1, Color c)
{
    lineClipped(x0, y0, x1, y0, c);
    lineClipped(x1, y0, x1, y1, c);
    lineClipped(x1, y1, x0, y1, c);
    lineClipped(x0, y1, x0, y0, c);
}

// Trapecio horizontal: base inferior [xb0,xb1] en yb, base superior [xt0,xt1]
// en yt. Cada fila se interpola linealmente (relleno fila por fila).
void fillTrapezoid(int xb0, int xb1, int yb, int xt0, int xt1, int yt, Color c)
{
    const int h = yt - yb;
    for (int y = yb; y <= yt; y++) {
        const float t = (h == 0) ? 0.0f : float(y - yb) / h;
        const int xa = int(xb0 + (xt0 - xb0) * t + 0.5f);
        const int xb = int(xb1 + (xt1 - xb1) * t + 0.5f);
        if (y < g_window.ymin || y > g_window.ymax) continue;
        for (int x = std::max(xa, g_window.xmin); x <= std::min(xb, g_window.xmax); x++)
            putPixel(x, y, c);
    }
}

void trapezoidOutline(int xb0, int xb1, int yb, int xt0, int xt1, int yt, Color c)
{
    lineClipped(xb0, yb, xb1, yb, c);
    lineClipped(xb1, yb, xt1, yt, c);
    lineClipped(xt1, yt, xt0, yt, c);
    lineClipped(xt0, yt, xb0, yb, c);
}

// Rueda: neumatico (disco negro) y llanta (disco gris liso), como en la imagen.
// Dos circunferencias rellenas con el algoritmo del punto medio.
void drawWheel(int cx, int cy)
{
    fillCircleMidpoint(cx, cy, WHEEL_R,     rgb(25, 25, 25));
    fillCircleMidpoint(cx, cy, WHEEL_R - 3, rgb(150, 150, 155));
}

// ----- vehiculos -------------------------------------------------------------

void drawCar(const Car& car)
{
    const Color outline = rgb(60, 10, 10);
    const Color glass   = rgb(190, 215, 235);
    const int x = car.x, y = car.y;            // y = base de las ruedas

    // Carroceria (44 x 13): de y+6 a y+19
    fillRect(x, y + 6, x + 43, y + 19, car.body);
    rectOutline(x, y + 6, x + 43, y + 19, outline);
    lineClipped(x, y + 11, x + 43, y + 11, outline);            // franja lateral

    // Cabina (trapecio, y+19 .. y+36) y ventanas
    fillTrapezoid(x + 7, x + 36, y + 20, x + 13, x + 30, y + 36, car.body);
    trapezoidOutline(x + 7, x + 36, y + 20, x + 13, x + 30, y + 36, outline);
    fillTrapezoid(x + 11, x + 20, y + 21, x + 14, x + 20, y + 34, glass);   // ventana trasera
    fillTrapezoid(x + 22, x + 32, y + 21, x + 22, x + 29, y + 34, glass);   // ventana delantera
    lineClipped(x + 21, y + 20, x + 21, y + 36, outline);                 // parante central
    lineClipped(x + 21, y + 8,  x + 21, y + 18, outline);                 // division de puertas

    // Faros (circulos de punto medio)
    fillCircleMidpoint(x + 41, y + 15, 2, rgb(255, 240, 150));
    fillCircleMidpoint(x + 2,  y + 15, 2, rgb(230, 60, 60));

    // Ruedas
    drawWheel(x + 10, y + WHEEL_R);
    drawWheel(x + 34, y + WHEEL_R);
}

void drawTruck(const Truck& t)
{
    const Color outline = rgb(15, 25, 50);
    const Color glass   = rgb(200, 225, 245);
    const int x = t.x, y = t.y;                 // y = base de las ruedas

    // Chasis (franja oscura bajo la caja y la cabina)
    fillRect(x, y + 6, x + 56, y + 10, rgb(55, 55, 60));
    rectOutline(x, y + 6, x + 56, y + 10, outline);

    // Caja de carga (37 x 43): y+10 .. y+53
    fillRect(x, y + 10, x + 36, y + 53, t.cargo);
    rectOutline(x, y + 10, x + 36, y + 53, outline);

    // Cabina: parte baja rectangular + parte alta con parabrisas inclinado
    fillRect(x + 38, y + 10, x + 56, y + 30, t.cab);
    rectOutline(x + 38, y + 10, x + 56, y + 30, outline);
    fillTrapezoid(x + 38, x + 56, y + 31, x + 38, x + 50, y + 45, t.cab);
    trapezoidOutline(x + 38, x + 56, y + 31, x + 38, x + 50, y + 45, outline);
    fillTrapezoid(x + 42, x + 53, y + 32, x + 42, x + 49, y + 43, glass);   // ventana
    fillCircleMidpoint(x + 55, y + 14, 2, rgb(255, 240, 150));              // faro

    // Ruedas: dos ejes traseros juntos y uno delantero (como en la imagen)
    drawWheel(x + 9,  y + WHEEL_R);
    drawWheel(x + 22, y + WHEEL_R);
    drawWheel(x + 48, y + WHEEL_R);
}

} // namespace

void drawVehicles()
{
    // Estructuras de datos con los vehiculos de la escena.
    const std::vector<Car> cars = {
        { CAR_X, WHEEL_BASE_Y, rgb(215, 45, 40) },                       // auto rojo
    };
    const std::vector<Truck> trucks = {
        { TRUCK_X, WHEEL_BASE_Y, rgb(30, 80, 165), rgb(95, 150, 210) },  // camion azul
    };

    for (const Car& c : cars)     drawCar(c);
    for (const Truck& t : trucks) drawTruck(t);
}