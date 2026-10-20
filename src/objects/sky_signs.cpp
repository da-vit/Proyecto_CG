// ============================================================
// sky_signs.cpp - Cielo
//
// Parte 1: degradado de cielo, ciudad lejana, sol y nubes.
// (El semaforo y la senal de PARE se agregan en la Parte 6.)
//
// Todo se agrega a g_escena. Rejilla 320x240, Y hacia ARRIBA.
// ============================================================

#include <vector>
#include "objects.h"
#include "scene.h"
#include "types.h"
#include "config.h"

namespace {

// ------------------------------------------------------------
// Cielo con degradado: franjas horizontales de color.
// Abajo (cerca del horizonte) es mas claro; arriba, mas azul.
// ------------------------------------------------------------
void agregarCielo()
{
    const int yInicio   = 60;                 // empieza detras del suelo
    const int yFin      = 240;
    const int numFranjas = 12;
    const int altoFranja = (yFin - yInicio) / numFranjas;

    // Color en el horizonte y color en lo mas alto
    const Color abajo  = colorRGB(215, 235, 248);
    const Color arriba = colorRGB(165, 212, 240);

    for (int i = 0; i < numFranjas; i++) {
        // t va de 0 (abajo) a 1 (arriba)
        const float t = static_cast<float>(i) / (numFranjas - 1);

        const Color color = {
            abajo.rojo  + (arriba.rojo  - abajo.rojo)  * t,
            abajo.verde + (arriba.verde - abajo.verde) * t,
            abajo.azul  + (arriba.azul  - abajo.azul)  * t
        };

        // Un poco mas ancho que el lienzo para cubrir los bordes
        g_escena.agregarRectangulo("Cielo - franja", -5, yInicio + i * altoFranja,
                                   330, altoFranja + 1, color);
    }
}

// ------------------------------------------------------------
// Ciudad lejana: siluetas azul palido sin ventanas (dan profundidad)
// ------------------------------------------------------------
void agregarCiudadLejana()
{
    const Color silueta = colorRGB(150, 195, 225);

    //                          nombre          x    y                  ancho  alto
    g_escena.agregarRectangulo("Lejano 1",      10, Y_BASE_EDIFICIOS,   32,    55, silueta);
    g_escena.agregarRectangulo("Lejano 2",     125, Y_BASE_EDIFICIOS,   22,    78, silueta);
    g_escena.agregarRectangulo("Lejano 3",     150, Y_BASE_EDIFICIOS,   30,    45, silueta);
    g_escena.agregarRectangulo("Lejano 4",     215, Y_BASE_EDIFICIOS,   28,    60, silueta);
}

// ------------------------------------------------------------
// Sol: circulo relleno + borde + 8 rayos (Bresenham)
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
// Nubes: union de circulos blancos + base plana
// ------------------------------------------------------------
struct Burbuja { int dx, dy, radio; };   // un circulo de la nube

void agregarNube(int cx, int cy)
{
    const Color blanco = colorRGB(252, 252, 255);

    const std::vector<Burbuja> burbujas = {
        { -14, 0, 6 }, { -5, 5, 9 }, { 7, 4, 10 }, { 17, 0, 6 }
    };

    for (const Burbuja& b : burbujas)
        g_escena.agregarCirculo("Nube", cx + b.dx, cy + b.dy, b.radio, blanco, true);

    // Base plana (como en la imagen de referencia)
    g_escena.agregarRectangulo("Nube - base", cx - 20, cy - 6, 43, 7, blanco);
}

} // namespace

// ------------------------------------------------------------
// Funcion publica (declarada en objects.h)
// ------------------------------------------------------------
void dibujarCieloYSenales()
{
    agregarCielo();
    agregarCiudadLejana();
    agregarSol(135, 200);

    // Posiciones tomadas de la imagen de referencia
    agregarNube(52, 205);
    agregarNube(215, 222);
    agregarNube(268, 205);
}