#include "objects.h"
#include "types.h"
#include "framebuffer.h"
#include "line.h"
#include "circle.h"
#include "polygon.h"
#include "clipping.h"
#include <vector>

// =====================================================================
// Integrante 5: casa y arboles
// Coordenadas: la grilla es de 320 x 240 y y = 0 esta ABAJO de la pantalla.
// =====================================================================

const int SUELO_Y = 50;   // altura donde termina la pista y empieza el suelo (coordinar con Integrante 2)

// ---------- Estructuras de datos (requisito: no dibujar "instrucciones sueltas") ----------
struct Casa {
    int x, y;            // esquina inferior izquierda de las paredes
    int ancho, alto;     // tamano de las paredes
    int alturaTecho;     // cuanto sube la punta del techo
    Color pared, techo, puerta, ventana, borde;
};

enum TipoCopa { COPA_TRIANGULAR, COPA_CIRCULAR };

struct Arbol {
    int x, y;                     // centro de la base del tronco
    int anchoTronco, altoTronco;
    TipoCopa tipo;
    int tamCopa;                  // triangular: mitad del ancho; circular: radio
    Color tronco, copa;
};

// ---------- Datos de la escena ----------
static std::vector<Casa> casas = {
    { 205, SUELO_Y, 50, 32, 22,
      rgb(245, 215, 70), rgb(210, 40, 40), rgb(120, 70, 30), rgb(150, 200, 240), rgb(40, 40, 40) }
};

static std::vector<Arbol> arboles = {
    {  60, SUELO_Y, 4,  8, COPA_TRIANGULAR, 10, rgb(110, 70, 35), rgb(40, 150, 60) },
    { 180, SUELO_Y, 5, 14, COPA_TRIANGULAR, 14, rgb(110, 70, 35), rgb(30, 130, 50) },
    { 272, SUELO_Y, 5, 12, COPA_CIRCULAR,   12, rgb(110, 70, 35), rgb(50, 160, 70) },
    { 314, SUELO_Y, 6, 14, COPA_TRIANGULAR, 16, rgb(110, 70, 35), rgb(30, 120, 45) }  // sale de la ventana -> clipping
};

// ---------- Funciones auxiliares ----------

// Devuelve los 4 vertices de un rectangulo (sentido antihorario)
static std::vector<Vec2> rectangulo(int x, int y, int ancho, int alto)
{
    return { {x, y}, {x + ancho, y}, {x + ancho, y + alto}, {x, y + alto} };
}

// Recorta el poligono contra la ventana, lo rellena con scan-line y dibuja su borde
static void dibujarPoligono(const std::vector<Vec2>& pts, Color relleno, Color borde)
{
    std::vector<Vec2> recortado = clipPolygon(pts, g_window);
    if (recortado.size() < 3) return;          // quedo totalmente fuera
    fillPolygonScanline(recortado, relleno);
    drawPolygonOutline(recortado, borde);
}

// Recorta una linea y la dibuja con Bresenham
static void dibujarLinea(int x0, int y0, int x1, int y1, Color c)
{
    if (clipLine(x0, y0, x1, y1, g_window))
        drawLineBresenham(x0, y0, x1, y1, c);
}

// Circulo con punto medio; si esta totalmente fuera de la ventana se descarta
static void dibujarCirculo(int cx, int cy, int r, Color relleno, Color borde)
{
    if (cx + r < g_window.xmin || cx - r > g_window.xmax ||
        cy + r < g_window.ymin || cy - r > g_window.ymax) return;
    fillCircleMidpoint(cx, cy, r, relleno);
    drawCircleMidpoint(cx, cy, r, borde);
}

// ---------- Dibujo de cada objeto ----------

static void dibujarCasa(const Casa& c)
{
    // 1) Paredes
    dibujarPoligono(rectangulo(c.x, c.y, c.ancho, c.alto), c.pared, c.borde);

    // 2) Techo triangular (sobresale 6 px a cada lado)
    std::vector<Vec2> techo = {
        { c.x - 6,           c.y + c.alto },
        { c.x + c.ancho + 6, c.y + c.alto },
        { c.x + c.ancho / 2, c.y + c.alto + c.alturaTecho }
    };
    dibujarPoligono(techo, c.techo, c.borde);

    // 3) Puerta
    int anchoPuerta = c.ancho / 5;
    int altoPuerta  = c.alto * 3 / 5;
    dibujarPoligono(rectangulo(c.x + 8, c.y, anchoPuerta, altoPuerta), c.puerta, c.borde);

    // 4) Ventana con cruz (lineas de Bresenham)
    int vx = c.x + c.ancho - 22, vy = c.y + c.alto / 2 - 4;
    int va = 14, vh = 10;
    dibujarPoligono(rectangulo(vx, vy, va, vh), c.ventana, c.borde);
    dibujarLinea(vx + va / 2, vy, vx + va / 2, vy + vh, c.borde);   // vertical
    dibujarLinea(vx, vy + vh / 2, vx + va, vy + vh / 2, c.borde);   // horizontal
}

static void dibujarArbol(const Arbol& a)
{
    Color bordeOscuro = rgb(20, 60, 25);

    // 1) Tronco
    int xt = a.x - a.anchoTronco / 2;
    dibujarPoligono(rectangulo(xt, a.y, a.anchoTronco, a.altoTronco), a.tronco, rgb(60, 35, 15));

    int yTope = a.y + a.altoTronco;   // donde empieza la copa

    // 2) Copa
    if (a.tipo == COPA_TRIANGULAR) {
        std::vector<Vec2> copa = {
            { a.x - a.tamCopa, yTope },
            { a.x + a.tamCopa, yTope },
            { a.x,             yTope + a.tamCopa * 2 }
        };
        dibujarPoligono(copa, a.copa, bordeOscuro);
    } else {
        dibujarCirculo(a.x, yTope + a.tamCopa - 2, a.tamCopa, a.copa, bordeOscuro);
    }
}

// ---------- Funcion publica (la llama scene.cpp) ----------
void drawHouseAndTrees()
{
    for (const Casa& c : casas)    dibujarCasa(c);
    for (const Arbol& a : arboles) dibujarArbol(a);
}