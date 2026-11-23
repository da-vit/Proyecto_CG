// ============================================================
// road.cpp - Keidy Cochama: suelo y carretera
//
// De atras hacia adelante:
//   pasto -> asfalto -> bordes de la pista -> linea central
//   discontinua -> cruce peatonal (paso de cebra)
//
//   Asfalto, pasto y franjas del cruce : poligonos + scan-line
//   Bordes y linea central             : lineas con Bresenham
//
// Alturas en config.h (Y hacia ARRIBA).
// ============================================================

#include "objects.h"
#include "scene.h"
#include "config.h"
#include "types.h"

namespace {

// Cruce peatonal: va junto al semaforo (signs.cpp)
const int X_CRUCE     = 30;   // borde izquierdo de las franjas
const int ANCHO_CRUCE = 20;

// Rectangulo que cubre todo el ancho del lienzo (con margen: el recorte
// de poligonos quita lo que sobresale de la ventana)
void agregarFranja(const char* nombre, int yInf, int ySup, Color color)
{
    g_escena.agregarRectangulo(nombre, -5, yInf, ANCHO_REJILLA + 10, ySup - yInf, color);
}

// ------------------------------------------------------------
// Pasto (entre la carretera y los edificios): dos tonos para
// que no se vea plano.
// ------------------------------------------------------------
void agregarPasto()
{
    agregarFranja("Pasto", Y_CARRETERA_SUP, Y_BASE_EDIFICIOS, colorRGB(122, 186, 72));
    agregarFranja("Pasto - franja oscura", Y_BASE_EDIFICIOS - 6, Y_BASE_EDIFICIOS,
                  colorRGB(98, 165, 60));
}

// ------------------------------------------------------------
// Carretera: asfalto, bordes y linea central discontinua
// ------------------------------------------------------------
void agregarCarretera()
{
    const Color asfalto      = colorRGB(80, 88, 98);
    const Color bordeVereda  = colorRGB(205, 205, 200);
    const Color lineaBlanca  = colorRGB(240, 240, 240);

    agregarFranja("Carretera - asfalto", Y_CARRETERA_INF, Y_CARRETERA_SUP, asfalto);

    // Borde de la vereda (2 lineas) y linea blanca de borde de pista
    g_escena.agregarLinea("Carretera - borde de vereda", -5, Y_CARRETERA_SUP - 1,
                          ANCHO_REJILLA + 5, Y_CARRETERA_SUP - 1, bordeVereda);
    g_escena.agregarLinea("Carretera - borde de vereda", -5, Y_CARRETERA_SUP - 2,
                          ANCHO_REJILLA + 5, Y_CARRETERA_SUP - 2, bordeVereda);
    g_escena.agregarLinea("Carretera - linea de borde", -5, Y_CARRETERA_SUP - 5,
                          ANCHO_REJILLA + 5, Y_CARRETERA_SUP - 5, lineaBlanca);

    // Linea central discontinua: tramos de 26 celdas cada 52, de 2 de grosor.
    // Empieza despues del cruce peatonal para no pintarse encima.
    const int yCentro = (Y_CARRETERA_INF + Y_CARRETERA_SUP) / 2;
    for (int x = X_CRUCE + ANCHO_CRUCE + 8; x < ANCHO_REJILLA + 26; x += 52)
        for (int g = 0; g < 2; g++)
            g_escena.agregarLinea("Carretera - linea central", x, yCentro + g,
                                  x + 26, yCentro + g, lineaBlanca);
}

// ------------------------------------------------------------
// Cruce peatonal (paso de cebra): franjas blancas horizontales
// que atraviesan la pista. Cada franja es un rectangulo relleno.
// ------------------------------------------------------------
void agregarCrucePeatonal()
{
    const Color blanco = colorRGB(235, 235, 235);
    const int   alto   = 4;    // grosor de cada franja
    const int   hueco  = 4;    // separacion entre franjas

    for (int y = Y_CARRETERA_INF + 4; y + alto <= Y_CARRETERA_SUP - 7; y += alto + hueco)
        g_escena.agregarRectangulo("Cruce peatonal - franja", X_CRUCE, y,
                                   ANCHO_CRUCE, alto, blanco);
}

} // namespace

// ------------------------------------------------------------
// Funcion publica (declarada en objects.h)
// ------------------------------------------------------------
void dibujarCarretera()
{
    agregarPasto();
    agregarCarretera();
    agregarCrucePeatonal();
}
