#include "objects.h"
#include "scene.h"
#include "types.h"
#include "config.h"

// ============================================================
// Carretera (Integrante 2)
// Los objetos se GUARDAN en g_escena; Escena::renderizar() los dibuja
// (lineas con Bresenham + recorte Cohen-Sutherland, rectangulos con scan-line).
// Coordenadas: lienzo de 320 x 240, y = 0 es la parte de ABAJO.
// La carretera empieza en x = -20 y termina en x = 339: se sale de la
// ventana por ambos lados a proposito, para evidenciar el clipping.
// ============================================================

//Medidas de la carretera
const int CARRETERA_X_INICIO = -20;
const int CARRETERA_X_FIN = ANCHO_REJILLA + 19;   // 339
const int CARRETERA_Y_ABAJO = 10;
const int CARRETERA_Y_ARRIBA = 55;
void dibujarCarretera()
{
    const Color gris_asfalto = colorRGB(60, 60, 60);
    const Color gris_vereda = colorRGB(170, 170, 170);
    const Color verde_pasto = colorRGB(80, 160, 70);
    const Color blanco = colorRGB(255, 255, 255);
    const Color amarillo = colorRGB(255, 210, 0);
    const int ancho = CARRETERA_X_FIN - CARRETERA_X_INICIO + 1;   // 360
    //primero franja de pasto y vereda sobre la pista (poligonos rellenos)
    g_escena.agregarRectangulo("Pasto", CARRETERA_X_INICIO, CARRETERA_Y_ARRIBA + 7, ancho, 4, verde_pasto);
    g_escena.agregarRectangulo("Vereda", CARRETERA_X_INICIO, CARRETERA_Y_ARRIBA + 1, ancho, 6, gris_vereda);

    //segundo asfalto (poligono relleno con scan-line)
    g_escena.agregarRectangulo("Asfalto", CARRETERA_X_INICIO, CARRETERA_Y_ABAJO,
        ancho, CARRETERA_Y_ARRIBA - CARRETERA_Y_ABAJO + 1, gris_asfalto);
    //tercero bordes blancos continuos (lineas con Bresenham)
    g_escena.agregarLinea("Borde inferior", CARRETERA_X_INICIO, CARRETERA_Y_ABAJO + 2,
        CARRETERA_X_FIN, CARRETERA_Y_ABAJO + 2, blanco);
    g_escena.agregarLinea("Borde superior", CARRETERA_X_INICIO, CARRETERA_Y_ARRIBA - 2,
        CARRETERA_X_FIN, CARRETERA_Y_ARRIBA - 2, blanco);

    //linea central discontinua: segmentos de 16 px separados por 10 px
    const int yCentro = (CARRETERA_Y_ABAJO + CARRETERA_Y_ARRIBA) / 2;   // 32
    const int largo = 16;
    const int espacio = 10;
    for (int x = CARRETERA_X_INICIO; x <= CARRETERA_X_FIN; x += largo + espacio) {
        g_escena.agregarLinea("Divisoria", x, yCentro, x + largo - 1, yCentro, amarillo);
        g_escena.agregarLinea("Divisoria", x, yCentro + 1, x + largo - 1, yCentro + 1, amarillo);
    }
}
