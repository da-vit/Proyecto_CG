// ============================================================
// circle.cpp - Lurancy Chipa: circunferencia con PUNTO MEDIO
// ============================================================
#include "circle.h"
#include "framebuffer.h"

// ---------------------------------------------------------------------------
// Circunferencia con el algoritmo del PUNTO MEDIO.
//
// Teoria (Sesion 4): la forma implicita es f(x,y) = x^2 + y^2 - r^2.
//   f < 0 -> punto dentro del circulo, f = 0 -> sobre el, f > 0 -> fuera.
// Se calcula solo el octante que va de (0,r) a x = y (pendiente de 0 a -1);
// en ese octante x avanza siempre 1 y solo y decide si baja 1 o no.
// Se evalua f en el punto medio M = (x+1, y-1/2) entre los candidatos
//   p1 = (x+1, y)   y   p2 = (x+1, y-1):
//   d = f(M) = (x+1)^2 + (y-1/2)^2 - r^2
//   d < 0  -> M dentro  -> se elige p1 ; d_nuevo = d + 2x + 3
//   d >= 0 -> M fuera   -> se elige p2 ; d_nuevo = d + 2(x - y) + 5
// Valor inicial en (0,r): d0 = 5/4 - r. Como 5/4 solo desplaza el signo en
// valores enteros, se usa d0 = 1 - r y todo el algoritmo es aritmetica entera.
// El resto de los puntos sale por SIMETRIA DE 8 VIAS:
//   (x,y) (y,x) (-y,x) (-x,y) (-x,-y) (-y,-x) (y,-x) (x,-y)  + centro (cx,cy)
// ---------------------------------------------------------------------------

// Pinta los 8 puntos simetricos de (x,y) respecto al centro (cx,cy).
static void ponerOchoPuntos(int cx, int cy, int x, int y, Color color)
{
    ponerPixel(cx + x, cy + y, color);
    ponerPixel(cx - x, cy + y, color);
    ponerPixel(cx + x, cy - y, color);
    ponerPixel(cx - x, cy - y, color);
    ponerPixel(cx + y, cy + x, color);
    ponerPixel(cx - y, cy + x, color);
    ponerPixel(cx + y, cy - x, color);
    ponerPixel(cx - y, cy - x, color);
}

// Segmento horizontal [xa, xb] en la fila y (para el relleno del disco).
static void lineaHorizontal(int xa, int xb, int y, Color color)
{
    for (int x = xa; x <= xb; x++) ponerPixel(x, y, color);
}

void dibujarCircunferenciaPuntoMedio(int cx, int cy, int radio, Color color)
{
    if (radio <= 0) { ponerPixel(cx, cy, color); return; }

    int x = 0;
    int y = radio;
    int d = 1 - radio;               // d0 = 5/4 - r  (version entera)

    while (x <= y) {
        ponerOchoPuntos(cx, cy, x, y, color);
        if (d < 0) {
            d += 2 * x + 3;      // se elige p1 = (x+1, y)
        } else {
            d += 2 * (x - y) + 5; // se elige p2 = (x+1, y-1)
            y--;
        }
        x++;
    }
}

// Disco: mismo recorrido del octante, pero en vez de puntos se rellenan los
// segmentos horizontales entre puntos simetricos (cada fila del disco).
void rellenarCircunferenciaPuntoMedio(int cx, int cy, int radio, Color color)
{
    if (radio <= 0) { ponerPixel(cx, cy, color); return; }

    int x = 0;
    int y = radio;
    int d = 1 - radio;

    while (x <= y) {
        lineaHorizontal(cx - x, cx + x, cy + y, color);   // filas cerca de los polos
        lineaHorizontal(cx - x, cx + x, cy - y, color);
        lineaHorizontal(cx - y, cx + y, cy + x, color);   // filas cerca del ecuador
        lineaHorizontal(cx - y, cx + y, cy - x, color);
        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}