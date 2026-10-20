#include "objects.h"
#include "scene.h"
#include "config.h"
#include "types.h"

// Altura (en la rejilla) donde se apoyan los edificios; la carretera se dibuja despues y los tapa.
static const int BASE_Y = 60;

// Dibuja una cuadricula de ventanas dentro de un edificio rectangular.
// Cada ventana es un rectangulo (poligono relleno por scan-line) guardado en g_escena.
static void agregarVentanas(const std::string& nombre, int x, int y, int ancho, int alto,
                            int columnas, int filas)
{
    const int anchoV = 6;
    const int altoV  = 8;

    const int sepX = (ancho - columnas * anchoV) / (columnas + 1);
    const int sepY = (alto  - filas    * altoV)  / (filas + 1);
    if (sepX < 1 || sepY < 1) return;   // no caben las ventanas

    const Color encendida = colorRGB(255, 235, 130);
    const Color apagada   = colorRGB(120, 170, 210);

    for (int f = 0; f < filas; f++) {
        for (int c = 0; c < columnas; c++) {
            const int vx = x + sepX + c * (anchoV + sepX);
            const int vy = y + sepY + f * (altoV + sepY);
            const Color col = ((f + c) % 3 == 0) ? encendida : apagada;
            g_escena.agregarRectangulo(nombre + " - ventana", vx, vy, anchoV, altoV,
                                       col, true, colorRGB(40, 40, 60));
        }
    }
}

void dibujarEdificios()
{
    const Color negro = colorRGB(30, 30, 30);

    // Edificio 1: rojo, sale parcialmente por la izquierda (prueba de recorte)
    g_escena.agregarRectangulo("Edificio rojo", -15, BASE_Y, 50, 110,
                               colorRGB(178, 60, 55), true, negro);
    agregarVentanas("Edificio rojo", -15, BASE_Y, 50, 110, 3, 6);

    // Edificio 2: gris azulado
    g_escena.agregarRectangulo("Edificio gris", 45, BASE_Y, 45, 85,
                               colorRGB(110, 120, 140), true, negro);
    agregarVentanas("Edificio gris", 45, BASE_Y, 45, 85, 3, 4);

    // Edificio 3: marron claro, el mas alto
    g_escena.agregarRectangulo("Edificio marron", 100, BASE_Y, 50, 140,
                               colorRGB(150, 110, 80), true, negro);
    agregarVentanas("Edificio marron", 100, BASE_Y, 50, 140, 3, 8);

    // Edificio 4: irregular (escalonado), poligono de 8 vertices
    g_escena.agregarPoligono("Edificio escalonado",
        { {160, BASE_Y}, {160, BASE_Y + 80}, {180, BASE_Y + 80}, {180, BASE_Y + 110},
          {215, BASE_Y + 110}, {215, BASE_Y + 65}, {225, BASE_Y + 65}, {225, BASE_Y} },
        colorRGB(95, 140, 110), true, negro);
    agregarVentanas("Edificio escalonado", 160, BASE_Y, 20, 80, 2, 4);       // cuerpo bajo-izquierdo
    agregarVentanas("Edificio escalonado", 180, BASE_Y + 65, 35, 45, 3, 2);  // torre central

    // Edificio 5: azul oscuro, sale parcialmente por la derecha (prueba de recorte)
    g_escena.agregarRectangulo("Edificio azul", 270, BASE_Y, 60, 95,
                               colorRGB(60, 80, 130), true, negro);
    agregarVentanas("Edificio azul", 270, BASE_Y, 60, 95, 4, 4);
}
