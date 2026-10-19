#include "clipping.h"
#include "config.h"
#include <cmath>

Window g_window = { 0, 0, GRID_WIDTH - 1, GRID_HEIGHT - 1 };

// TODO (Integrante 2)
bool clipLine(int& x0, int& y0, int& x1, int& y1, const Window& w)
{
    return true;   // stub: no recorta
}

// =====================================================================
// Integrante 5: recorte de poligonos con Sutherland-Hodgman
// Idea: recortar el poligono contra UN borde de la ventana a la vez
// (izquierdo, derecho, abajo, arriba). La salida de un borde es la
// entrada del siguiente.
// =====================================================================

enum Borde { IZQUIERDO, DERECHO, ABAJO, ARRIBA };

// ¿El punto p esta del lado "visible" de este borde?
static bool estaDentro(const Vec2& p, Borde borde, const Window& w)
{
    switch (borde) {
        case IZQUIERDO: return p.x >= w.xmin;
        case DERECHO:   return p.x <= w.xmax;
        case ABAJO:     return p.y >= w.ymin;
        case ARRIBA:    return p.y <= w.ymax;
    }
    return false;
}

// Punto donde el segmento a-b corta la recta del borde
static Vec2 interseccion(const Vec2& a, const Vec2& b, Borde borde, const Window& w)
{
    float x, y;
    if (borde == IZQUIERDO || borde == DERECHO) {
        x = (borde == IZQUIERDO) ? (float)w.xmin : (float)w.xmax;  // recta vertical
        float t = (x - a.x) / (float)(b.x - a.x);                  // cuanto avanzar de a hacia b
        y = a.y + t * (b.y - a.y);
    } else {
        y = (borde == ABAJO) ? (float)w.ymin : (float)w.ymax;      // recta horizontal
        float t = (y - a.y) / (float)(b.y - a.y);
        x = a.x + t * (b.x - a.x);
    }
    return { (int)std::lround(x), (int)std::lround(y) };
}

// Recorta el poligono contra un solo borde
static std::vector<Vec2> recortarContraBorde(const std::vector<Vec2>& entrada,
                                             Borde borde, const Window& w)
{
    std::vector<Vec2> salida;
    const size_t n = entrada.size();
    if (n == 0) return salida;

    for (size_t i = 0; i < n; i++) {
        const Vec2& actual   = entrada[i];
        const Vec2& anterior = entrada[(i + n - 1) % n];   // el vertice previo (cierra el poligono)

        bool actualDentro   = estaDentro(actual, borde, w);
        bool anteriorDentro = estaDentro(anterior, borde, w);

        if (actualDentro) {
            if (!anteriorDentro)                     // Caso: fuera -> dentro
                salida.push_back(interseccion(anterior, actual, borde, w));
            salida.push_back(actual);                // Caso: dentro -> dentro (y el anterior)
        } else if (anteriorDentro) {                 // Caso: dentro -> fuera
            salida.push_back(interseccion(anterior, actual, borde, w));
        }
        // Caso: fuera -> fuera: no se agrega nada
    }
    return salida;
}

std::vector<Vec2> clipPolygon(const std::vector<Vec2>& pts, const Window& w)
{
    std::vector<Vec2> resultado = pts;
    resultado = recortarContraBorde(resultado, IZQUIERDO, w);
    resultado = recortarContraBorde(resultado, DERECHO,   w);
    resultado = recortarContraBorde(resultado, ABAJO,     w);
    resultado = recortarContraBorde(resultado, ARRIBA,    w);
    return resultado;   // si queda con menos de 3 puntos, el poligono estaba fuera
}
