// ============================================================
// polygon.cpp  -  Integrante 4: relleno de poligonos (scan-line)
//
// Algoritmo:
//   1. Para cada arista (no horizontal) se guarda: y minima,
//      y maxima, x en la y minima y la pendiente inversa (1/m).
//   2. Se recorre cada fila y desde y_min hasta y_max del poligono.
//   3. En cada fila se buscan las aristas "activas" (las que la cruzan)
//      y se calcula la x donde la cruzan.
//   4. Se ordenan las x y se rellena por pares: (x1..x2), (x3..x4)...
//
// Regla para no contar dos veces un vertice compartido por dos
// aristas: una arista cubre las filas y_min <= y < y_max
// (incluye la fila inferior y excluye la superior).
// ============================================================

#include "polygon.h"
#include "framebuffer.h"
#include "line.h"
#include <algorithm>
#include <cmath>
#include <vector>

// Una arista del poligono (ya orientada de abajo hacia arriba)
struct Arista {
    int   yMin;        // fila donde empieza
    int   yMax;        // fila donde termina (no se incluye)
    float xActual;     // x donde la arista cruza la fila yMin
    float pendienteInv; // cuanto cambia x al subir una fila (dx/dy)
};

void rellenarPoligonoScanline(const std::vector<Punto2D>& puntos, Color color)
{
    const size_t n = puntos.size();
    if (n < 3) return;

    // ---- 1. Tabla de aristas (ET) ----
    std::vector<Arista> tabla;
    int yMinPoligono = puntos[0].y;
    int yMaxPoligono = puntos[0].y;

    for (size_t i = 0; i < n; i++) {
        Punto2D a = puntos[i];
        Punto2D b = puntos[(i + 1) % n];

        yMinPoligono = std::min(yMinPoligono, a.y);
        yMaxPoligono = std::max(yMaxPoligono, a.y);

        if (a.y == b.y) continue;          // aristas horizontales: se ignoran
        if (a.y > b.y) std::swap(a, b);    // a = extremo inferior, b = superior

        Arista e;
        e.yMin         = a.y;
        e.yMax         = b.y;
        e.xActual      = static_cast<float>(a.x);
        e.pendienteInv = static_cast<float>(b.x - a.x) / static_cast<float>(b.y - a.y);
        tabla.push_back(e);
    }

    // ---- 2. Recorrer cada fila ----
    for (int y = yMinPoligono; y <= yMaxPoligono; y++) {

        // ---- 3. Aristas activas: las que cruzan esta fila ----
        std::vector<float> cruces;
        for (const Arista& e : tabla) {
            if (y >= e.yMin && y < e.yMax) {
                // x en esta fila = x inicial + (filas recorridas) * dx/dy
                cruces.push_back(e.xActual + (y - e.yMin) * e.pendienteInv);
            }
        }

        // ---- 4. Ordenar y rellenar por pares ----
        std::sort(cruces.begin(), cruces.end());
        for (size_t k = 0; k + 1 < cruces.size(); k += 2) {
            const int xIzq = static_cast<int>(std::ceil(cruces[k]));
            const int xDer = static_cast<int>(std::floor(cruces[k + 1]));
            for (int x = xIzq; x <= xDer; x++)
                ponerPixel(x, y, color);
        }
    }
}

// Contorno del poligono: une cada vertice con el siguiente (Bresenham)
void dibujarContornoPoligono(const std::vector<Punto2D>& puntos, Color color)
{
    const size_t n = puntos.size();
    for (size_t i = 0; i < n; i++) {
        const Punto2D& a = puntos[i];
        const Punto2D& b = puntos[(i + 1) % n];
        dibujarLineaBresenham(a.x, a.y, b.x, b.y, color);
    }
}
