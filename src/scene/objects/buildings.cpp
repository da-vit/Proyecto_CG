// ============================================================
// buildings.cpp - Coral Cuadros: edificios
//
// SEGUNDA ENTREGA - DISTORSION (shear en X) del edificio azul:
//   M = T(0, yb) * Sh_x(shx) * T(0, -yb)        yb = base del edificio
//   1) T(0,-yb): baja la base del edificio hasta el eje x (y = 0)
//   2) Sh_x    : x' = x + shx*y  -> los puntos con y = 0 no se mueven
//   3) T(0, yb): lo regresa a su altura
//   Resultado: el edificio se inclina pero su base queda en el suelo.
//
// Edificios con ventanas repetidas, techos de cristal y letreros
// (HOTEL, BANCO, CAFE, TIENDA), apoyados en
// Y_BASE_EDIFICIOS (config.h). Todo se agrega a g_escena; el
// relleno scan-line y el recorte los hace Escena::renderizar().
// ============================================================

#include <cstdio>
#include <string>
#include "objects.h"
#include "scene.h"
#include "config.h"
#include "types.h"
#include "matriz.h"
#include "animacion.h"

namespace {

// ------------------------------------------------------------
// agregarVentanas: cuadricula de ventanas dentro de un edificio.
//   (x, y)  esquina inferior izquierda del edificio
// La separacion se calcula repartiendo el espacio libre en
// partes iguales entre las ventanas.
// ------------------------------------------------------------
void agregarVentanas(const std::string& nombre, int x, int y, int ancho, int alto,
                     int columnas, int filas)
{
    const int anchoVentana = 5;
    const int altoVentana  = 6;

    const int separacionX = (ancho - columnas * anchoVentana) / (columnas + 1);
    const int separacionY = (alto  - filas    * altoVentana)  / (filas + 1);
    if (separacionX < 1 || separacionY < 1) return;   // no caben

    const Color encendida = colorRGB(255, 235, 130);  // amarillo
    const Color vidrio    = colorRGB(120, 175, 220);  // azul de cristal
    const Color borde     = colorRGB(50, 70, 110);

    for (int fila = 0; fila < filas; fila++) {
        for (int col = 0; col < columnas; col++) {
            const int vx = x + separacionX + col * (anchoVentana + separacionX);
            const int vy = y + separacionY + fila * (altoVentana + separacionY);

            // Una de cada cuatro ventanas encendida
            const Color color = ((fila * 2 + col) % 4 == 0) ? encendida : vidrio;

            g_escena.agregarRectangulo(nombre + " - ventana", vx, vy,
                                       anchoVentana, altoVentana, color, true, borde);
        }
    }
}

// ------------------------------------------------------------
// agregarTechoCristal: trapecio inclinado sobre el edificio
// (poligono relleno por scan-line).
// ------------------------------------------------------------
void agregarTechoCristal(const std::string& nombre, int x, int yTope, int ancho)
{
    const Color cristal = colorRGB(205, 225, 240);
    const Color borde   = colorRGB(60, 80, 110);

    g_escena.agregarPoligono(nombre + " - techo",
        { {x, yTope}, {x + ancho - 1, yTope},
          {x + ancho - 7, yTope + 8}, {x + 7, yTope + 8} },
        cristal, true, borde);
}

// ------------------------------------------------------------
// agregarEdificio: cuerpo + ventanas (+ techo opcional)
// ------------------------------------------------------------
void agregarEdificio(const std::string& nombre, int x, int ancho, int alto,
                     Color color, int columnas, int filas, bool conTecho)
{
    const Color contorno = colorRGB(40, 50, 70);

    g_escena.agregarRectangulo(nombre, x, Y_BASE_EDIFICIOS, ancho, alto, color,
                               true, contorno);
    agregarVentanas(nombre, x, Y_BASE_EDIFICIOS, ancho, alto, columnas, filas);

    if (conTecho)
        agregarTechoCristal(nombre, x, Y_BASE_EDIFICIOS + alto, ancho);
}

// ------------------------------------------------------------
// agregarLetrero: cartel sobre la azotea de un edificio.
//   Tablero = rectangulo relleno (scan-line) con borde (Bresenham)
//   Patas   = dos lineas cortas (Bresenham)
//   Texto   = letras hechas de segmentos (Bresenham, ver text.cpp)
//
//   cx     centro horizontal del letrero
//   yBase  azotea del edificio (donde se apoyan las patas)
// ------------------------------------------------------------
void agregarLetrero(const std::string& texto, int cx, int yBase,
                    Color fondo, Color colorTexto)
{
    const Color borde = colorRGB(40, 40, 45);

    const int anchoTexto = Escena::anchoTexto(texto);
    const int ancho      = anchoTexto + 6;          // 3 celdas de margen a cada lado
    const int alto       = 9;
    const int xIzq       = cx - ancho / 2;
    const int yTablero   = yBase + 3;               // las patas miden 3 celdas

    // Patas
    g_escena.agregarLinea("Letrero " + texto + " - pata", xIzq + 3, yBase,
                          xIzq + 3, yTablero, borde);
    g_escena.agregarLinea("Letrero " + texto + " - pata", xIzq + ancho - 4, yBase,
                          xIzq + ancho - 4, yTablero, borde);

    // Tablero y texto centrado
    g_escena.agregarRectangulo("Letrero " + texto, xIzq, yTablero, ancho, alto,
                               fondo, true, borde);
    g_escena.agregarTexto("Letrero " + texto + " - letra", texto,
                          cx - anchoTexto / 2, yTablero + 2, colorTexto);
}

} // namespace

// ------------------------------------------------------------
// Funcion publica (declarada en objects.h)
// ------------------------------------------------------------
void dibujarEdificios()
{
    const int Y = Y_BASE_EDIFICIOS;

    //                nombre               x    ancho alto  color                    col fil techo
    // Izquierda: sale parcialmente por el borde (prueba de recorte)
    agregarEdificio("Edificio crema",     -12,  46,   60, colorRGB(232, 222, 190),  4,  5, false);

    // Hotel, con letrero en la azotea
    agregarEdificio("Hotel",               30,  44,   78, colorRGB(235, 238, 242),  4,  6, false);
    agregarLetrero("HOTEL", 30 + 44 / 2, Y + 78, colorRGB(200, 40, 60), colorRGB(255, 255, 255));

    // El mas alto, con techo de cristal
    agregarEdificio("Edificio torre",      78,  36,  108, colorRGB( 70, 110, 190),  3,  9, true);

    // Banco: debajo del sol (altura baja para no taparlo)
    agregarEdificio("Banco",              118,  42,   68, colorRGB(195, 200, 210),  4,  5, false);
    agregarLetrero("BANCO", 118 + 42 / 2, Y + 68, colorRGB(30, 90, 60), colorRGB(255, 230, 120));

    // Azul con techo inclinado: se DEFORMA (shear en X) respecto a su base
    const double    shx = deformacionEdificio();
    const Matriz3x3 M   = componer({ traslacion(0, Y), deformacionX(shx), traslacion(0, -Y) });
    g_escena.establecerMatriz(M);
    agregarEdificio("Edificio azul",      166,  40,   92, colorRGB( 60,  95, 170),  3,  7, true);
    g_escena.restablecerMatriz();

    if (g_animacion.imprimirMatrices) {
        imprimirMatriz("[5] DISTORSION - Edificio azul:  M = T(0,96) * Sh_x(shx) * T(0,-96)", M);
        const PuntoF esquina = transformar(M, { 166, 96 + 91 });   // esquina sup. izquierda
        std::printf("    shx = %.3f | esquina (166, 187) -> (%.2f, %.2f)  (la base y = 96 no se mueve)\n",
                    shx, esquina.x, esquina.y);
    }

    // Cafe
    agregarEdificio("Cafe",               210,  34,   66, colorRGB(240, 232, 205),  3,  5, false);
    agregarLetrero("CAFE", 210 + 34 / 2, Y + 66, colorRGB(110, 70, 40), colorRGB(255, 240, 210));

    // Tienda: rosa y estrecho
    agregarEdificio("Tienda",             248,  26,   86, colorRGB(215, 110, 125),  2,  7, false);
    agregarLetrero("TIENDA", 248 + 26 / 2, Y + 86, colorRGB(255, 215, 80), colorRGB(120, 30, 50));

    // Derecha: sale parcialmente por el borde (prueba de recorte)
    agregarEdificio("Edificio gris der.", 278,  50,   58, colorRGB(175, 180, 195),  4,  4, false);
}