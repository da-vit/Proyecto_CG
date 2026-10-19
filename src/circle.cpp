#include "circle.h"
#include "framebuffer.h"

// ---------------------------------------------------------------------------
// Integrante 3: circunferencia con el algoritmo del PUNTO MEDIO.
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
static void plotOctants(int cx, int cy, int x, int y, Color c)
{
    putPixel(cx + x, cy + y, c);
    putPixel(cx - x, cy + y, c);
    putPixel(cx + x, cy - y, c);
    putPixel(cx - x, cy - y, c);
    putPixel(cx + y, cy + x, c);
    putPixel(cx - y, cy + x, c);
    putPixel(cx + y, cy - x, c);
    putPixel(cx - y, cy - x, c);
}

// Segmento horizontal [xa, xb] en la fila y (para el relleno del disco).
static void hspan(int xa, int xb, int y, Color c)
{
    for (int x = xa; x <= xb; x++) putPixel(x, y, c);
}

void drawCircleMidpoint(int cx, int cy, int r, Color c)
{
    if (r <= 0) { putPixel(cx, cy, c); return; }

    int x = 0;
    int y = r;
    int d = 1 - r;               // d0 = 5/4 - r  (version entera)

    while (x <= y) {
        plotOctants(cx, cy, x, y, c);
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
void fillCircleMidpoint(int cx, int cy, int r, Color c)
{
    if (r <= 0) { putPixel(cx, cy, c); return; }

    int x = 0;
    int y = r;
    int d = 1 - r;

    while (x <= y) {
        hspan(cx - x, cx + x, cy + y, c);   // filas cerca de los polos
        hspan(cx - x, cx + x, cy - y, c);
        hspan(cx - y, cx + y, cy + x, c);   // filas cerca del ecuador
        hspan(cx - y, cx + y, cy - x, c);
        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}