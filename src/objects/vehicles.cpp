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
//   - Clipping: toda linea pasa por recortarLinea(g_ventana) antes de Bresenham y
//     los rellenos se recortan contra g_ventana; los circulos se recortan en
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
void lineaRecortada(int x0, int y0, int x1, int y1, Color c)
{
    if (recortarLinea(x0, y0, x1, y1, g_ventana))
        dibujarLineaBresenham(x0, y0, x1, y1, c);
}

void rellenarRectangulo(int x0, int y0, int x1, int y1, Color c)
{
    if (x0 < g_ventana.x_min) x0 = g_ventana.x_min;
    if (y0 < g_ventana.y_min) y0 = g_ventana.y_min;
    if (x1 > g_ventana.x_max) x1 = g_ventana.x_max;
    if (y1 > g_ventana.y_max) y1 = g_ventana.y_max;
    for (int y = y0; y <= y1; y++)
        for (int x = x0; x <= x1; x++)
            ponerPixel(x, y, c);
}

// Contorno de rectangulo con 4 lineas de Bresenham.
void contornoRectangulo(int x0, int y0, int x1, int y1, Color c)
{
    lineaRecortada(x0, y0, x1, y0, c);
    lineaRecortada(x1, y0, x1, y1, c);
    lineaRecortada(x1, y1, x0, y1, c);
    lineaRecortada(x0, y1, x0, y0, c);
}

// Trapecio horizontal: base inferior [xb0,xb1] en yb, base superior [xt0,xt1]
// en yt. Cada fila se interpola linealmente (relleno fila por fila).
void rellenarTrapecio(int xb0, int xb1, int yb, int xt0, int xt1, int yt, Color c)
{
    const int h = yt - yb;
    for (int y = yb; y <= yt; y++) {
        const float t = (h == 0) ? 0.0f : float(y - yb) / h;
        const int xa = int(xb0 + (xt0 - xb0) * t + 0.5f);
        const int xb = int(xb1 + (xt1 - xb1) * t + 0.5f);
        if (y < g_ventana.y_min || y > g_ventana.y_max) continue;
        for (int x = std::max(xa, g_ventana.x_min); x <= std::min(xb, g_ventana.x_max); x++)
            ponerPixel(x, y, c);
    }
}

void contornoTrapecio(int xb0, int xb1, int yb, int xt0, int xt1, int yt, Color c)
{
    lineaRecortada(xb0, yb, xb1, yb, c);
    lineaRecortada(xb1, yb, xt1, yt, c);
    lineaRecortada(xt1, yt, xt0, yt, c);
    lineaRecortada(xt0, yt, xb0, yb, c);
}

// Rueda: neumatico (disco negro) y llanta (disco gris liso), como en la imagen.
// Dos circunferencias rellenas con el algoritmo del punto medio.
void dibujarRueda(int cx, int cy)
{
    rellenarCircunferenciaPuntoMedio(cx, cy, WHEEL_R,     colorRGB(25, 25, 25));
    rellenarCircunferenciaPuntoMedio(cx, cy, WHEEL_R - 3, colorRGB(150, 150, 155));
}

// ----- vehiculos -------------------------------------------------------------

void dibujarAuto(const Car& car)
{
    const Color outline = colorRGB(60, 10, 10);
    const Color glass   = colorRGB(190, 215, 235);
    const int x = car.x, y = car.y;            // y = base de las ruedas

    // Carroceria (44 x 13): de y+6 a y+19
    rellenarRectangulo(x, y + 6, x + 43, y + 19, car.body);
    contornoRectangulo(x, y + 6, x + 43, y + 19, outline);
    lineaRecortada(x, y + 11, x + 43, y + 11, outline);            // franja lateral

    // Cabina (trapecio, y+19 .. y+36) y ventanas
    rellenarTrapecio(x + 7, x + 36, y + 20, x + 13, x + 30, y + 36, car.body);
    contornoTrapecio(x + 7, x + 36, y + 20, x + 13, x + 30, y + 36, outline);
    rellenarTrapecio(x + 11, x + 20, y + 21, x + 14, x + 20, y + 34, glass);   // ventana trasera
    rellenarTrapecio(x + 22, x + 32, y + 21, x + 22, x + 29, y + 34, glass);   // ventana delantera
    lineaRecortada(x + 21, y + 20, x + 21, y + 36, outline);                 // parante central
    lineaRecortada(x + 21, y + 8,  x + 21, y + 18, outline);                 // division de puertas

    // Faros (circulos de punto medio)
    rellenarCircunferenciaPuntoMedio(x + 41, y + 15, 2, colorRGB(255, 240, 150));
    rellenarCircunferenciaPuntoMedio(x + 2,  y + 15, 2, colorRGB(230, 60, 60));

    // Ruedas
    dibujarRueda(x + 10, y + WHEEL_R);
    dibujarRueda(x + 34, y + WHEEL_R);
}

void dibujarCamion(const Truck& t)
{
    const Color outline = colorRGB(15, 25, 50);
    const Color glass   = colorRGB(200, 225, 245);
    const int x = t.x, y = t.y;                 // y = base de las ruedas

    // Chasis (franja oscura bajo la caja y la cabina)
    rellenarRectangulo(x, y + 6, x + 56, y + 10, colorRGB(55, 55, 60));
    contornoRectangulo(x, y + 6, x + 56, y + 10, outline);

    // Caja de carga (37 x 43): y+10 .. y+53
    rellenarRectangulo(x, y + 10, x + 36, y + 53, t.cargo);
    contornoRectangulo(x, y + 10, x + 36, y + 53, outline);

    // Cabina: parte baja rectangular + parte alta con parabrisas inclinado
    rellenarRectangulo(x + 38, y + 10, x + 56, y + 30, t.cab);
    contornoRectangulo(x + 38, y + 10, x + 56, y + 30, outline);
    rellenarTrapecio(x + 38, x + 56, y + 31, x + 38, x + 50, y + 45, t.cab);
    contornoTrapecio(x + 38, x + 56, y + 31, x + 38, x + 50, y + 45, outline);
    rellenarTrapecio(x + 42, x + 53, y + 32, x + 42, x + 49, y + 43, glass);   // ventana
    rellenarCircunferenciaPuntoMedio(x + 55, y + 14, 2, colorRGB(255, 240, 150));              // faro

    // Ruedas: dos ejes traseros juntos y uno delantero (como en la imagen)
    dibujarRueda(x + 9,  y + WHEEL_R);
    dibujarRueda(x + 22, y + WHEEL_R);
    dibujarRueda(x + 48, y + WHEEL_R);
}

} // namespace

void dibujarVehiculos()
{
    // Estructuras de datos con los vehiculos de la escena.
    const std::vector<Car> cars = {
        { CAR_X, WHEEL_BASE_Y, colorRGB(215, 45, 40) },                       // auto rojo
    };
    const std::vector<Truck> trucks = {
        { TRUCK_X, WHEEL_BASE_Y, colorRGB(30, 80, 165), colorRGB(95, 150, 210) },  // camion azul
    };

    for (const Car& c : cars)     dibujarAuto(c);
    for (const Truck& t : trucks) dibujarCamion(t);
}