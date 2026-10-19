#include "polygon.h"
#include "framebuffer.h"

#include <algorithm>
#include <cmath>
#include <vector>

void rellenarPoligonoScanline(const std::vector<Punto2D>& puntos, Color color)
{
    const int n = static_cast<int>(puntos.size());
    if (n < 3) return;

    // 1. Rango vertical del poligono
    int yMin = puntos[0].y, yMax = puntos[0].y;
    for (const Punto2D& p : puntos) {
        yMin = std::min(yMin, p.y);
        yMax = std::max(yMax, p.y);
    }

    // 2. Para cada linea de barrido, calcular intersecciones con las aristas
    for (int y = yMin; y < yMax; y++) {
        std::vector<double> cruces;

        for (int i = 0; i < n; i++) {
            const Punto2D& a = puntos[i];
            const Punto2D& b = puntos[(i + 1) % n];

            if (a.y == b.y) continue;                       // arista horizontal: se ignora

            const int ySup = std::max(a.y, b.y);
            const int yInf = std::min(a.y, b.y);

            // Intervalo semiabierto [yInf, ySup): evita contar dos veces los vertices
            if (y < yInf || y >= ySup) continue;

            const double t = static_cast<double>(y - a.y) / (b.y - a.y);
            cruces.push_back(a.x + t * (b.x - a.x));
        }

        // 3. Ordenar y rellenar entre pares de intersecciones
        std::sort(cruces.begin(), cruces.end());
        for (size_t k = 0; k + 1 < cruces.size(); k += 2) {
            const int xIni = static_cast<int>(std::ceil(cruces[k]));
            const int xFin = static_cast<int>(std::floor(cruces[k + 1]));
            for (int x = xIni; x <= xFin; x++)
                ponerPixel(x, y, color);
        }
    }

    // 4. Ultima fila (y = yMax): el intervalo semiabierto la omite,
    //    asi que se completa con las aristas horizontales y los vertices de esa fila.
    for (int i = 0; i < n; i++) {
        const Punto2D& a = puntos[i];
        const Punto2D& b = puntos[(i + 1) % n];
        if (a.y == yMax && b.y == yMax) {
            for (int x = std::min(a.x, b.x); x <= std::max(a.x, b.x); x++)
                ponerPixel(x, yMax, color);
        } else if (a.y == yMax) {
            ponerPixel(a.x, yMax, color);
        }
    }
}
