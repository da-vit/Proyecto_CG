// ============================================================
// De atras hacia adelante:
//   pasto grande -> carretera (hasta el borde inferior)
//
// Alturas en config.h (Y hacia ARRIBA).
// ============================================================

#include "objects.h"
#include "scene.h"
#include "config.h"
#include "types.h"

namespace {

// Rectangulo que cubre todo el ancho del lienzo (con margen)
void agregarFranja(const char* nombre, int yInf, int ySup, Color color)
{
    g_escena.agregarRectangulo(nombre, -5, yInf, ANCHO_REJILLA + 10, ySup - yInf, color);
}

// ------------------------------------------------------------
// Pasto grande (entre la carretera y los edificios):
// dos tonos para que no se vea plano.
// ------------------------------------------------------------
void agregarPastoGrande()
{
    agregarFranja("Pasto grande", Y_CARRETERA_SUP, Y_BASE_EDIFICIOS, colorRGB(122, 186, 72));
    agregarFranja("Pasto grande - franja", Y_BASE_EDIFICIOS - 6, Y_BASE_EDIFICIOS,
                  colorRGB(98, 165, 60));
}

// ------------------------------------------------------------
// Carretera: asfalto y linea central discontinua (Bresenham)
// ------------------------------------------------------------
void agregarCarretera()
{
    const Color asfalto      = colorRGB(80, 88, 98);
    const Color lineaCentral = colorRGB(240, 240, 240);

    agregarFranja("Carretera", Y_CARRETERA_INF, Y_CARRETERA_SUP, asfalto);

    // Tramos de 26 px cada 52 px, 2 px de grosor
    const int yCentro = (Y_CARRETERA_INF + Y_CARRETERA_SUP) / 2;
    for (int x = 6; x < ANCHO_REJILLA; x += 52)
        for (int g = 0; g < 2; g++)
            g_escena.agregarLinea("Carretera - linea central", x, yCentro + g,
                                  x + 26, yCentro + g, lineaCentral);
}

} // namespace

void dibujarCarretera()
{
    agregarPastoGrande();
    agregarCarretera();
}
