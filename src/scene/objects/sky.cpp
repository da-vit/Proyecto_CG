// ============================================================
// sky.cpp - Fondo de la escena
// David Araoz: cielo con degradado, ciudad lejana,
//              sol y nubes (circulos de punto medio)
//
// SEGUNDA ENTREGA - TRASLACION de las nubes:
//   M = T(dx, 0)   con dx = amplitud * sen(velocidad * t)
//   Las nubes van y vienen sin salir de la ventana de
//   visualizacion: siempre se ven completas.
//
// Todo se agrega a g_escena. Rejilla 320x240, Y hacia ARRIBA.
// ============================================================

#include <vector>
#include "objects.h"
#include "scene.h"
#include "types.h"
#include "config.h"
#include "matriz.h"
#include "animacion.h"

namespace {

// ------------------------------------------------------------
// Cielo con degradado: franjas horizontales (rectangulos rellenos
// con scan-line). Abajo, cerca del horizonte, es mas claro;
// arriba, mas azul.
// ------------------------------------------------------------
void agregarCielo()
{
    const int yInicio    = 60;                 // empieza detras del suelo
    const int yFin       = ALTO_REJILLA;
    const int numFranjas = 12;
    const int altoFranja = (yFin - yInicio) / numFranjas;

    // Color en el horizonte y color en lo mas alto
    const Color abajo  = colorRGB(215, 235, 248);
    const Color arriba = colorRGB(165, 212, 240);

    for (int i = 0; i < numFranjas; i++) {
        // t va de 0 (abajo) a 1 (arriba): mezcla lineal de los dos colores
        const float t = static_cast<float>(i) / (numFranjas - 1);

        const Color color = {
            abajo.rojo  + (arriba.rojo  - abajo.rojo)  * t,
            abajo.verde + (arriba.verde - abajo.verde) * t,
            abajo.azul  + (arriba.azul  - abajo.azul)  * t
        };

        // Un poco mas ancho que el lienzo: el recorte quita lo que sobra
        g_escena.agregarRectangulo("Cielo - franja", -5, yInicio + i * altoFranja,
                                   ANCHO_REJILLA + 10, altoFranja + 1, color);
    }
}

// ------------------------------------------------------------
// Ciudad lejana: siluetas azul palido sin ventanas (dan profundidad).
// Se colocan en los huecos entre los edificios del frente.
// ------------------------------------------------------------
void agregarCiudadLejana()
{
    const Color silueta = colorRGB(150, 195, 225);

    //                          nombre                x    y                 ancho alto
    g_escena.agregarRectangulo("Ciudad lejana",      66, Y_BASE_EDIFICIOS,  22,   96, silueta);
    g_escena.agregarRectangulo("Ciudad lejana",     154, Y_BASE_EDIFICIOS,  20,   84, silueta);
    g_escena.agregarRectangulo("Ciudad lejana",     236, Y_BASE_EDIFICIOS,  18,   78, silueta);
    g_escena.agregarRectangulo("Ciudad lejana",     268, Y_BASE_EDIFICIOS,  20,   72, silueta);
}

// ------------------------------------------------------------
// Sol: circulo relleno + borde (punto medio) + 8 rayos (Bresenham)
// ------------------------------------------------------------
void agregarSol(int cx, int cy)
{
    const Color amarillo = colorRGB(255, 225, 110);
    const Color borde    = colorRGB(235, 175, 40);
    const int   radio    = 12;

    g_escena.agregarCirculo("Sol", cx, cy, radio, amarillo, true);
    g_escena.agregarCirculo("Sol - borde", cx, cy, radio, borde, false);

    // Rayos: cada uno es (x inicial, y inicial, x final, y final)
    // relativos al centro. Primero los 4 ejes, luego las 4 diagonales.
    struct Rayo { int x0, y0, x1, y1; };
    const Rayo rayos[8] = {
        {  15,   0,  21,   0 },  { -15,   0, -21,   0 },
        {   0,  15,   0,  21 },  {   0, -15,   0, -21 },
        {  11,  11,  15,  15 },  { -11,  11, -15,  15 },
        {  11, -11,  15, -15 },  { -11, -11, -15, -15 }
    };

    for (const Rayo& r : rayos)
        g_escena.agregarLinea("Sol - rayo", cx + r.x0, cy + r.y0,
                              cx + r.x1, cy + r.y1, borde);
}

// ------------------------------------------------------------
// Nubes: union de circulos blancos (punto medio) + base plana
// ------------------------------------------------------------
struct Burbuja { int dx, dy, radio; };   // un circulo de la nube, relativo al centro

void agregarNube(int cx, int cy)
{
    const Color blanco = colorRGB(252, 252, 255);

    const std::vector<Burbuja> burbujas = {
        { -14, 0, 6 }, { -5, 5, 9 }, { 7, 4, 10 }, { 17, 0, 6 }
    };

    for (const Burbuja& b : burbujas)
        g_escena.agregarCirculo("Nube", cx + b.dx, cy + b.dy, b.radio, blanco, true);

    // Base plana de la nube (rectangulo)
    g_escena.agregarRectangulo("Nube - base", cx - 20, cy - 6, 43, 7, blanco);
}

} // namespace

// ------------------------------------------------------------
// Funcion publica (declarada en objects.h)
// ------------------------------------------------------------
void dibujarCielo()
{
    agregarCielo();
    agregarCiudadLejana();
    agregarSol(135, 200);

    // Nubes: posicion de reposo (x, y). Estan elegidas para que, con su
    // vaiven, queden siempre dentro de la ventana y por encima de los edificios.
    const Punto2D nubes[3] = { {46, 212}, {205, 214}, {276, 204} };

    for (int i = 0; i < 3; i++) {
        const Matriz3x3 M = traslacion(desplazamientoNube(i), 0);   // TRASLACION
        g_escena.establecerMatriz(M);
        agregarNube(nubes[i].x, nubes[i].y);

        if (i == 0 && g_animacion.imprimirMatrices)
            imprimirMatriz("[1] TRASLACION - Nube:  M = T(dx, 0)", M);
    }
    g_escena.restablecerMatriz();
}
