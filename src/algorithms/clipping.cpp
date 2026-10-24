#include "clipping.h"
#include "config.h"
#include <cmath>

// Ventana de visualizacion: un poco mas pequena que el lienzo (320x240),
// asi se ve claramente que los objetos que se salen quedan recortados.
Ventana g_ventana = { 0, 0, ANCHO_REJILLA - 1, ALTO_REJILLA - 1 };

// =====================================================================
// Integrante 2: recorte de lineas con Cohen-Sutherland
// =====================================================================

// Codigos de region (4 bits). Recordar: aqui y=0 es ABAJO de la pantalla.
const int DENTRO    = 0;   // 0000
const int IZQUIERDA = 1;   // 0001  x < xmin
const int DERECHA   = 2;   // 0010  x > xmax
const int ABAJO     = 4;   // 0100  y < ymin
const int ARRIBA    = 8;   // 1000  y > ymax

// Calcula en que region esta el punto (x, y) respecto a la ventana
static int calcularCodigo(double x, double y, const Ventana& v)
{
    int codigo = DENTRO;
    if (x < v.x_min)      codigo |= IZQUIERDA;
    else if (x > v.x_max) codigo |= DERECHA;
    if (y < v.y_min)      codigo |= ABAJO;
    else if (y > v.y_max) codigo |= ARRIBA;
    return codigo;
}

bool recortarLinea(int& x0, int& y0, int& x1, int& y1, const Ventana& v)
{
    // Trabajamos con double para que las intersecciones sean precisas
    double ax = x0, ay = y0, bx = x1, by = y1;
    int codigoA = calcularCodigo(ax, ay, v);
    int codigoB = calcularCodigo(bx, by, v);

    while (true) {
        if ((codigoA | codigoB) == 0) {
            // Ambos extremos dentro -> ACEPTAR
            x0 = (int)std::lround(ax);  y0 = (int)std::lround(ay);
            x1 = (int)std::lround(bx);  y1 = (int)std::lround(by);
            return true;
        }
        if ((codigoA & codigoB) != 0) {
            // Ambos extremos fuera del MISMO lado -> RECHAZAR
            return false;
        }

        // Caso intermedio: tomamos un extremo que este fuera
        int codigoFuera = (codigoA != 0) ? codigoA : codigoB;
        double x = 0, y = 0;

        // Interseccion con el borde correspondiente (ecuacion de la recta)
        if (codigoFuera & ARRIBA) {
            x = ax + (bx - ax) * (v.y_max - ay) / (by - ay);
            y = v.y_max;
        }
        else if (codigoFuera & ABAJO) {
            x = ax + (bx - ax) * (v.y_min - ay) / (by - ay);
            y = v.y_min;
        }
        else if (codigoFuera & DERECHA) {
            y = ay + (by - ay) * (v.x_max - ax) / (bx - ax);
            x = v.x_max;
        }
        else if (codigoFuera & IZQUIERDA) {
            y = ay + (by - ay) * (v.x_min - ax) / (bx - ax);
            x = v.x_min;
        }

        // Reemplazamos el extremo que estaba fuera por la interseccion
        if (codigoFuera == codigoA) {
            ax = x; ay = y;
            codigoA = calcularCodigo(ax, ay, v);
        }
        else {
            bx = x; by = y;
            codigoB = calcularCodigo(bx, by, v);
        }
    }
}

// =====================================================================
// Integrante 5: recorte de poligonos con Sutherland-Hodgman
// Idea: recortar el poligono contra UN borde de la ventana a la vez
// (izquierdo, derecho, abajo, arriba). La salida de un borde es la
// entrada del siguiente.
// (Los nombres llevan BORDE_ para no chocar con ABAJO/ARRIBA de Cohen-Sutherland.)
// =====================================================================

enum Borde { BORDE_IZQUIERDO, BORDE_DERECHO, BORDE_ABAJO, BORDE_ARRIBA };

// ¿El punto p esta del lado "visible" de este borde?
static bool estaDentro(const Punto2D& p, Borde borde, const Ventana& v)
{
    switch (borde) {
        case BORDE_IZQUIERDO: return p.x >= v.x_min;
        case BORDE_DERECHO:   return p.x <= v.x_max;
        case BORDE_ABAJO:     return p.y >= v.y_min;
        case BORDE_ARRIBA:    return p.y <= v.y_max;
    }
    return false;
}

// Punto donde el segmento a-b corta la recta del borde
static Punto2D interseccion(const Punto2D& a, const Punto2D& b, Borde borde, const Ventana& v)
{
    float x, y;
    if (borde == BORDE_IZQUIERDO || borde == BORDE_DERECHO) {
        x = (borde == BORDE_IZQUIERDO) ? (float)v.x_min : (float)v.x_max;  // recta vertical
        float t = (x - a.x) / (float)(b.x - a.x);                          // cuanto avanzar de a hacia b
        y = a.y + t * (b.y - a.y);
    } else {
        y = (borde == BORDE_ABAJO) ? (float)v.y_min : (float)v.y_max;      // recta horizontal
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
    resultado = recortarContraBorde(resultado, BORDE_IZQUIERDO, v);
    resultado = recortarContraBorde(resultado, BORDE_DERECHO,   v);
    resultado = recortarContraBorde(resultado, BORDE_ABAJO,     v);
    resultado = recortarContraBorde(resultado, BORDE_ARRIBA,    v);
    return resultado;   // si queda con menos de 3 puntos, el poligono estaba fuera
}