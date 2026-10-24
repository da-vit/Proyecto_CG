// ============================================================
//
// Edificios con ventanales y techos de cristal, apoyados en
// Y_BASE_EDIFICIOS (config.h). Todo se agrega a g_escena; el
// relleno scan-line y el recorte los hace Escena::renderizar().
// ============================================================

#include <string>
#include "objects.h"
#include "scene.h"
#include "config.h"
#include "types.h"

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

} // namespace

// ------------------------------------------------------------
// Funcion publica (declarada en objects.h)
// ------------------------------------------------------------
void dibujarEdificios()
{
    //                nombre             x    ancho alto  color                    col fil techo
    // Izquierda: sale parcialmente por el borde (prueba de recorte)
    agregarEdificio("Edificio crema",   -12,  46,   60, colorRGB(232, 222, 190),  4,  5, false);
    agregarEdificio("Edificio blanco",   30,  44,   78, colorRGB(235, 238, 242),  4,  6, false);

    // El mas alto, con techo de cristal
    agregarEdificio("Edificio torre",    78,  36,  108, colorRGB( 70, 110, 190),  3,  9, true);

    // Debajo del sol (altura baja para no taparlo)
    agregarEdificio("Edificio gris",    118,  42,   68, colorRGB(195, 200, 210),  4,  5, false);

    // Azul con techo inclinado
    agregarEdificio("Edificio azul",    166,  40,   92, colorRGB( 60,  95, 170),  3,  7, true);

    agregarEdificio("Edificio claro",   210,  34,   66, colorRGB(240, 232, 205),  3,  5, false);

    // Rosa, estrecho (como en la referencia)
    agregarEdificio("Edificio rosa",    248,  26,   86, colorRGB(215, 110, 125),  2,  7, false);

    // Derecha: sale parcialmente por el borde (prueba de recorte)
    agregarEdificio("Edificio gris der.", 278, 50,  58, colorRGB(175, 180, 195),  4,  4, false);
}