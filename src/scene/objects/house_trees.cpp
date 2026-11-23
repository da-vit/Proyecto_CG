// ============================================================
// house_trees.cpp - Samuel Sanchez: casa, arboles y arbustos
//
// SEGUNDA ENTREGA - ESCALA del arbol central, respecto a la base
// de su tronco B = (110, Y_SUELO) (punto fijo, diapositiva 1.9):
//   M = T(B) * S(s, s) * T(-B)
//   1) T(-B)  : lleva la base del tronco al origen
//   2) S(s,s) : agranda o achica (escala uniforme)
//   3) T(B)   : lo regresa a su lugar
//   Resultado: el arbol crece sin despegarse del suelo.
//
// Todo se dibuja sobre el pasto (Y_CARRETERA_SUP a
// Y_BASE_EDIFICIOS, ver config.h). Se agrega a g_escena; el
// relleno scan-line, los circulos de punto medio y el recorte
// los hace Escena::renderizar().
// ============================================================

#include "objects.h"
#include "scene.h"
#include "config.h"
#include "types.h"
#include "matriz.h"
#include "animacion.h"
#include <cstdio>

namespace {

// Y_SUELO (altura donde se apoyan los objetos) esta en config.h

// ------------------------------------------------------------
// Arbol de copa redonda: tronco (rectangulo) + copa formada por
// varios circulos verdes solapados (punto medio).
//   x        centro del arbol
//   altura   altura del tronco
//   radio    radio de la copa (los circulos se calculan a partir de el)
// ------------------------------------------------------------
void agregarArbol(int x, int altura, int radio)
{
    const Color tronco     = colorRGB(120, 75, 40);
    const Color copaOscura = colorRGB(60, 140, 50);
    const Color copaClara  = colorRGB(95, 175, 65);
    const Color contorno   = colorRGB(35, 90, 35);

    // Tronco
    g_escena.agregarRectangulo("Arbol - tronco", x - 2, Y_SUELO, 5, altura, tronco,
                               true, colorRGB(70, 40, 20));

    // Copa: un circulo central grande y cuatro alrededor
    const int cy = Y_SUELO + altura + radio / 2;
    const int r2 = radio * 2 / 3;

    g_escena.agregarCirculo("Arbol - copa", x - radio / 2, cy - radio / 3, r2, copaOscura, true);
    g_escena.agregarCirculo("Arbol - copa", x + radio / 2, cy - radio / 3, r2, copaOscura, true);
    g_escena.agregarCirculo("Arbol - copa", x,             cy + radio / 3, r2, copaOscura, true);
    g_escena.agregarCirculo("Arbol - copa", x,             cy,             radio, copaClara, true);

    // Contorno suave de la copa central
    g_escena.agregarCirculo("Arbol - contorno", x, cy, radio, contorno, false);
}

// ------------------------------------------------------------
// Arbusto: tres circulos verdes bajos y solapados
// ------------------------------------------------------------
void agregarArbusto(int x, int radio)
{
    const Color verde  = colorRGB(80, 160, 55);
    const Color oscuro = colorRGB(55, 125, 45);

    const int cy = Y_SUELO + radio - 2;

    g_escena.agregarCirculo("Arbusto", x - radio,     cy, radio * 3 / 4, oscuro, true);
    g_escena.agregarCirculo("Arbusto", x + radio,     cy, radio * 3 / 4, oscuro, true);
    g_escena.agregarCirculo("Arbusto", x,             cy + 2, radio,     verde,  true);
}

// ------------------------------------------------------------
// Casa: cuerpo rectangular, techo triangular, puerta y ventana
//   (x, y) esquina inferior izquierda
// ------------------------------------------------------------
void agregarCasa(int x, int y)
{
    const int ancho = 44;
    const int alto  = 24;

    const Color pared    = colorRGB(245, 215, 70);
    const Color techo    = colorRGB(205, 40, 40);
    const Color puerta   = colorRGB(120, 70, 35);
    const Color vidrio   = colorRGB(150, 205, 240);
    const Color contorno = colorRGB(60, 45, 30);

    // Cuerpo
    g_escena.agregarRectangulo("Casa - pared", x, y, ancho, alto, pared, true, contorno);

    // Techo triangular (un poco mas ancho que la pared)
    g_escena.agregarPoligono("Casa - techo",
        { {x - 4, y + alto}, {x + ancho + 3, y + alto}, {x + ancho / 2, y + alto + 16} },
        techo, true, colorRGB(110, 20, 20));

    // Puerta
    g_escena.agregarRectangulo("Casa - puerta", x + 7, y, 9, 15, puerta, true, contorno);

    // Ventana con cruz
    g_escena.agregarRectangulo("Casa - ventana", x + 25, y + 9, 12, 10, vidrio, true, contorno);
    g_escena.agregarLinea("Casa - ventana cruz", x + 31, y + 9,  x + 31, y + 18, contorno);
    g_escena.agregarLinea("Casa - ventana cruz", x + 25, y + 14, x + 36, y + 14, contorno);
}

} // namespace

// ------------------------------------------------------------
// Funcion publica (declarada en objects.h)
// ------------------------------------------------------------
void dibujarCasaYArboles()
{
    // Orden: lo que esta mas atras primero (arbustos), luego arboles y casa

    // Arbustos (uno sale por el borde izquierdo: prueba de recorte)
    agregarArbusto(2,   7);
    agregarArbusto(150, 6);
    agregarArbusto(205, 6);
    agregarArbusto(318, 7);      // sale por el borde derecho

    // Arboles de distintos tamanos
    agregarArbol(35,  14, 12);
    // Arbol que CRECE y se achica (escala respecto a la base del tronco)
    const double    s = escalaArbol();
    const Matriz3x3 M = componer({ traslacion(110, Y_SUELO), escalamiento(s, s),
                                   traslacion(-110, -Y_SUELO) });
    g_escena.establecerMatriz(M);
    agregarArbol(110, 18, 14);
    g_escena.restablecerMatriz();

    if (g_animacion.imprimirMatrices) {
        imprimirMatriz("[3] ESCALA - Arbol:  M = T(110,72) * S(s,s) * T(-110,-72)", M);
        const PuntoF tope = transformar(M, { 110, 72 + 32 });
        std::printf("    s = %.3f | punto (110, 104) -> (%.2f, %.2f)  (la base (110,72) no se mueve)\n",
                    s, tope.x, tope.y);
    }
    agregarArbol(180, 12, 11);
    agregarArbol(300, 16, 13);

    // Casa
    agregarCasa(222, Y_SUELO);
}
