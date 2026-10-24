// ============================================================
// sky_signs.cpp - Cielo
//
// Parte 1: degradado de cielo, ciudad lejana, sol y nubes.
// Parte 6: semaforo y senal de PARE (dibujarSenales). Se dibujan
// despues del pasto y la casa, si no el suelo los taparia.
//
// Todo se agrega a g_escena. Rejilla 320x240, Y hacia ARRIBA.
// ============================================================

#include <cmath>
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

// Altura donde se apoyan semaforo y senal (igual que Y_SUELO en house_trees.cpp)
const int Y_BASE_SENALES = Y_CARRETERA_SUP + 4;      // 72

// ------------------------------------------------------------
// Semaforo: poste delgado + caja gris oscuro con 3 luces.
//   x       centro del poste
//   yBase   donde el poste sale del suelo
// ------------------------------------------------------------
void agregarSemaforo(int x, int yBase)
{
    const Color poste    = colorRGB(35, 35, 38);
    const Color caja     = colorRGB(65, 68, 74);
    const Color negro    = colorRGB(10, 10, 10);
    const Color contorno = colorRGB(25, 25, 25);
    const Color rojo     = colorRGB(230, 40, 40);
    const Color amarillo = colorRGB(250, 200, 30);
    const Color verde    = colorRGB(40, 190, 70);

    const int alturaPoste = 46;
    const int anchoCaja   = 11;
    const int altoCaja    = 34;
    const int radioLuz    = 3;
    const int separacion  = 10;                       // entre centros de luces

    // Poste (sube hasta el tope de la caja) y caja encima
    g_escena.agregarRectangulo("Semaforo - poste", x - 1, yBase, 3, alturaPoste, poste);

    const int yCaja = yBase + alturaPoste - altoCaja;
    g_escena.agregarRectangulo("Semaforo - caja", x - anchoCaja / 2, yCaja,
                               anchoCaja, altoCaja, caja, true, negro);

    // Luces: roja arriba, amarilla en medio, verde abajo
    const int yVerde = yCaja + altoCaja / 2 - separacion;
    const int yMedio = yVerde + separacion;
    const int yRoja  = yMedio + separacion;

    g_escena.agregarCirculo("Semaforo - luz roja",     x, yRoja,  radioLuz, rojo,     true);
    g_escena.agregarCirculo("Semaforo - luz amarilla", x, yMedio, radioLuz, amarillo, true);
    g_escena.agregarCirculo("Semaforo - luz verde",    x, yVerde, radioLuz, verde,    true);

    g_escena.agregarCirculo("Semaforo - borde rojo",     x, yRoja,  radioLuz, contorno, false);
    g_escena.agregarCirculo("Semaforo - borde amarillo", x, yMedio, radioLuz, contorno, false);
    g_escena.agregarCirculo("Semaforo - borde verde",    x, yVerde, radioLuz, contorno, false);
}

// ------------------------------------------------------------
// Letras de "PARE": cada una es una lista de segmentos en una
// cuadricula de 3 x 5 px (x de 0 a 2, y de 0 a 4, Y hacia ARRIBA).
// ------------------------------------------------------------
struct Segmento { int x0, y0, x1, y1; };

std::vector<Segmento> segmentosLetra(char letra)
{
    switch (letra) {
        case 'P': return { {0,0,0,4}, {0,4,2,4}, {2,4,2,2}, {2,2,0,2} };
        case 'A': return { {0,0,0,3}, {0,3,1,4}, {1,4,2,3}, {2,3,2,0}, {0,2,2,2} };
        case 'R': return { {0,0,0,4}, {0,4,2,4}, {2,4,2,2}, {2,2,0,2}, {1,2,2,0} };
        case 'E': return { {0,0,0,4}, {0,4,2,4}, {0,2,1,2}, {0,0,2,0} };
    }
    return {};
}

// Texto blanco centrado en (cx, cy), solo con segmentos (Bresenham)
void agregarTextoPare(int cx, int cy)
{
    const Color blanco = colorRGB(255, 255, 255);
    const char  texto[] = "PARE";
    const int   ancho   = 3;                          // ancho de una letra
    const int   alto    = 5;                          // alto de una letra
    const int   paso    = ancho + 1;                  // letra + 1 px de espacio
    const int   total   = 4 * paso - 1 - 1;           // ancho del texto en coordenadas (0..14)

    const int xInicio = cx - total / 2;
    const int yInicio = cy - (alto - 1) / 2;

    for (int i = 0; i < 4; i++)
        for (const Segmento& s : segmentosLetra(texto[i]))
            g_escena.agregarLinea("PARE - letra",
                                  xInicio + i * paso + s.x0, yInicio + s.y0,
                                  xInicio + i * paso + s.x1, yInicio + s.y1, blanco);
}

// Octogono regular centrado en (cx, cy); radio = distancia al vertice
std::vector<Punto2D> verticesOctogono(int cx, int cy, float radio)
{
    const float pi = 3.14159265f;
    std::vector<Punto2D> vertices;
    for (int k = 0; k < 8; k++) {
        // Empieza en 22.5 grados para que los lados queden planos arriba/abajo/izq/der
        const float angulo = (22.5f + 45.0f * k) * pi / 180.0f;
        vertices.push_back({ cx + static_cast<int>(std::lround(radio * std::cos(angulo))),
                             cy + static_cast<int>(std::lround(radio * std::sin(angulo))) });
    }
    return vertices;
}

// ------------------------------------------------------------
// Senal de PARE: poste gris claro + octogono rojo con borde blanco
// y el texto "PARE".
//   x       centro del poste
//   yBase   donde el poste sale del suelo
// ------------------------------------------------------------
void agregarSenalPare(int x, int yBase)
{
    const Color poste  = colorRGB(185, 190, 195);
    const Color rojo   = colorRGB(200, 30, 35);
    const Color blanco = colorRGB(255, 255, 255);

    const int alturaPoste = 50;
    const int yCentro     = yBase + alturaPoste - 8;   // el octogono tapa la punta del poste

    g_escena.agregarRectangulo("PARE - poste", x - 1, yBase, 3, alturaPoste, poste,
                               true, colorRGB(120, 125, 130));

    // Octogono blanco (borde) un poco mayor, y el rojo encima
    g_escena.agregarPoligono("PARE - borde", verticesOctogono(x, yCentro, 12.0f), blanco);
    g_escena.agregarPoligono("PARE - octogono", verticesOctogono(x, yCentro, 10.0f), rojo);

    agregarTextoPare(x, yCentro);
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

// Semaforo y senal de PARE (declarada en objects.h). Se llama despues
// del pasto y la casa porque el orden de insercion es el orden de dibujo.
void dibujarSenales()
{
    agregarSemaforo(16, Y_BASE_SENALES);
    agregarSenalPare(303, Y_BASE_SENALES);
}