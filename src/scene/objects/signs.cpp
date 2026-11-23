// ============================================================
// signs.cpp - David Araoz: senales y mobiliario urbano
//   - Semaforo      : poste + caja + 3 luces (circulos de punto medio)
//   - Senal de PARE : octogono (poligono + scan-line) + texto (Bresenham)
//   - Postes de luz : poste (poligono) + brazo (Bresenham) + foco (punto medio)
//
// SEGUNDA ENTREGA - COMPOSICION en la senal de PARE (Keidy Cochama):
//   la senal primero ESCALA y luego ROTA respecto a su centro C:
//       M = T(C) * R(theta) * S(s, s) * T(-C)
//   Se lee de DERECHA a IZQUIERDA (la de la derecha actua primero):
//       1) T(-C)   : lleva el centro de la senal al origen
//       2) S(s,s)  : escala respecto al origen (= respecto al centro)
//       3) R(theta): rota respecto al origen   (= respecto al centro)
//       4) T(C)    : devuelve la senal a su lugar
//   El poste no se transforma: solo el octogono y el texto.
//
// Se dibujan DESPUES del pasto y la casa: el orden de insercion es
// el orden de dibujo, si no el suelo los taparia.
// Todo se agrega a g_escena. Rejilla 320x240, Y hacia ARRIBA.
// ============================================================

#include <cmath>
#include <vector>
#include "objects.h"
#include "scene.h"
#include "types.h"
#include "config.h"
#include "matriz.h"
#include "animacion.h"
#include <cstdio>

namespace {

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

    g_escena.agregarCirculo("Semaforo - borde de luz", x, yRoja,  radioLuz, contorno, false);
    g_escena.agregarCirculo("Semaforo - borde de luz", x, yMedio, radioLuz, contorno, false);
    g_escena.agregarCirculo("Semaforo - borde de luz", x, yVerde, radioLuz, contorno, false);
}

// ------------------------------------------------------------
// Octogono regular centrado en (cx, cy); radio = distancia al vertice.
//   vertice k:  ( cx + r*cos(a), cy + r*sin(a) ),  a = 22.5 + 45*k grados
// Empieza en 22.5 grados para que los lados queden planos
// arriba, abajo, a la izquierda y a la derecha.
// ------------------------------------------------------------
std::vector<Punto2D> verticesOctogono(int cx, int cy, float radio)
{
    const float pi = 3.14159265f;
    std::vector<Punto2D> vertices;
    for (int k = 0; k < 8; k++) {
        const float angulo = (22.5f + 45.0f * k) * pi / 180.0f;
        vertices.push_back({ cx + static_cast<int>(std::lround(radio * std::cos(angulo))),
                             cy + static_cast<int>(std::lround(radio * std::sin(angulo))) });
    }
    return vertices;
}

// ------------------------------------------------------------
// Senal de PARE: poste gris + octogono rojo con borde blanco + texto.
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

    // COMPOSICION: M = T(C) * R(theta) * S(s,s) * T(-C), con C = centro del octogono
    const double    theta = anguloSenal();
    const double    s     = escalaSenal();
    const Matriz3x3 M     = componer({ traslacion(x, yCentro), rotacion(theta),
                                       escalamiento(s, s), traslacion(-x, -yCentro) });
    g_escena.establecerMatriz(M);

    // Octogono blanco (borde) un poco mayor, y el rojo encima
    g_escena.agregarPoligono("PARE - borde",    verticesOctogono(x, yCentro, 12.0f), blanco);
    g_escena.agregarPoligono("PARE - octogono", verticesOctogono(x, yCentro, 10.0f), rojo);

    // Texto centrado (cada letra: segmentos de Bresenham, ver text.cpp)
    const int ancho = Escena::anchoTexto("PARE");
    g_escena.agregarTexto("PARE - letra", "PARE", x - ancho / 2, yCentro - 2, blanco);

    g_escena.restablecerMatriz();

    if (g_animacion.imprimirMatrices) {
        std::printf("[6] COMPOSICION - Senal PARE:  M = T(C) * R(%.2f) * S(%.3f) * T(-C),  C = (%d, %d)\n",
                    theta, s, x, yCentro);
        std::printf("    Orden: primero T(-C), luego S, luego R, al final T(C)"
                    " (la matriz de la derecha actua primero)\n");
        imprimirMatriz("    Matriz resultante M:", M);
        const PuntoF centro = transformar(M, { static_cast<double>(x), static_cast<double>(yCentro) });
        const PuntoF vertice = transformar(M, { x + 12.0, static_cast<double>(yCentro) });
        std::printf("    Centro (%d, %d) -> (%.2f, %.2f)  (punto fijo)\n", x, yCentro, centro.x, centro.y);
        std::printf("    Punto (%d, %d) -> (%.2f, %.2f)\n", x + 12, yCentro, vertice.x, vertice.y);
    }
}

// ------------------------------------------------------------
// Poste de luz: poste (rectangulo), brazo hacia la pista (dos
// lineas de Bresenham) y foco (circulo relleno + borde).
//   x       centro del poste
//   yBase   donde el poste sale del suelo
// ------------------------------------------------------------
void agregarPosteDeLuz(int x, int yBase)
{
    const Color metal   = colorRGB(70, 75, 85);
    const Color oscuro  = colorRGB(40, 42, 48);
    const Color foco    = colorRGB(255, 245, 170);

    const int altura = 44;
    const int yTope  = yBase + altura - 1;

    g_escena.agregarRectangulo("Poste de luz", x - 1, yBase, 2, altura, metal, true, oscuro);

    // Brazo: sale del tope hacia la derecha y baja un poco
    g_escena.agregarLinea("Poste de luz - brazo", x, yTope, x + 6, yTope, oscuro);
    g_escena.agregarLinea("Poste de luz - brazo", x + 6, yTope, x + 6, yTope - 2, oscuro);

    // Foco
    g_escena.agregarCirculo("Poste de luz - foco", x + 6, yTope - 4, 2, foco, true);
    g_escena.agregarCirculo("Poste de luz - foco", x + 6, yTope - 4, 2, oscuro, false);
}

} // namespace

// ------------------------------------------------------------
// Funcion publica (declarada en objects.h)
// ------------------------------------------------------------
void dibujarSenales()
{
    // Postes de luz a lo largo de la vereda
    agregarPosteDeLuz(68,  Y_SUELO);
    agregarPosteDeLuz(140, Y_SUELO);
    agregarPosteDeLuz(204, Y_SUELO);

    // Semaforo junto al cruce peatonal y senal de PARE al final de la calle
    agregarSemaforo(20, Y_SUELO);
    agregarSenalPare(290, Y_SUELO);
}
