#include "clipping.h"
#include "config.h"
#include <cmath>

// Ventana de visualizacion (por ahora, todo el lienzo; el grupo puede achicarla)
Ventana g_ventana = { 0, 0, ANCHO_REJILLA - 1, ALTO_REJILLA - 1 };

// TODO (Integrante 2)
bool recortarLinea(int& x0, int& y0, int& x1, int& y1, const Ventana& v)
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
static bool estaDentro(const Punto2D& p, Borde borde, const Ventana& v)
{
    switch (borde) {
        case IZQUIERDO: return p.x >= v.x_min;
        case DERECHO:   return p.x <= v.x_max;
        case ABAJO:     return p.y >= v.y_min;
        case ARRIBA:    return p.y <= v.y_max;
    }
    return false;
}

// Punto donde el segmento a-b corta la recta del borde
static Punto2D interseccion(const Punto2D& a, const Punto2D& b, Borde borde, const Ventana& v)
{
    float x, y;
    if (borde == IZQUIERDO || borde == DERECHO) {
        x = (borde == IZQUIERDO) ? (float)v.x_min : (float)v.x_max;  // recta vertical
        float t = (x - a.x) / (float)(b.x - a.x);                    // cuanto avanzar de a hacia b
        y = a.y + t * (b.y - a.y);
    } else {
        y = (borde == ABAJO) ? (float)v.y_min : (float)v.y_max;      // recta horizontal
        float t = (y - a.y) / (float)(b.y - a.y);
        x = a.x + t * (b.x - a.x);
    }
    return { (int)std::lround(x), (int)std::lround(y) };
}

// Recorta el poligono contra un solo borde
static std::vector<Punto2D> recortarContraBorde(const std::vector<Punto2D>& entrada,
                                                Borde borde, const Ventana& v)
{
    std::vector<Punto2D> salida;
    const size_t n = entrada.size();
    if (n == 0) return salida;

    for (size_t i = 0; i < n; i++) {
        const Punto2D& actual   = entrada[i];
        const Punto2D& anterior = entrada[(i + n - 1) % n];   // vertice previo (cierra el poligono)

        bool actualDentro   = estaDentro(actual, borde, v);
        bool anteriorDentro = estaDentro(anterior, borde, v);

        if (actualDentro) {
            if (!anteriorDentro)                     // Caso: fuera -> dentro
                salida.push_back(interseccion(anterior, actual, borde, v));
            salida.push_back(actual);                // Caso: dentro -> dentro (y el anterior)
        } else if (anteriorDentro) {                 // Caso: dentro -> fuera
            salida.push_back(interseccion(anterior, actual, borde, v));
        }
        // Caso: fuera -> fuera: no se agrega nada
    }
    return salida;
}

std::vector<Punto2D> recortarPoligono(const std::vector<Punto2D>& puntos, const Ventana& v)
{
    std::vector<Punto2D> resultado = puntos;
    resultado = recortarContraBorde(resultado, IZQUIERDO, v);
    resultado = recortarContraBorde(resultado, DERECHO,   v);
    resultado = recortarContraBorde(resultado, ABAJO,     v);
    resultado = recortarContraBorde(resultado, ARRIBA,    v);
    return resultado;   // si queda con menos de 3 puntos, el poligono estaba fuera
}