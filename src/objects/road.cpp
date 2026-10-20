// ============================================================
// road.cpp  -  Integrante 2: suelo de la escena
//
// De atras hacia adelante:
//   pasto grande -> carretera -> vereda -> pasto de frente
//
// La vereda se arma con baldosas (poligonos rellenos por scan-line)
// que se ensanchan hacia abajo para dar perspectiva.
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

// ------------------------------------------------------------
// Posicion X de una junta en una altura dada.
// Arriba (ySup) la junta esta en x0; hacia abajo se separa del
// centro hasta 1.8 veces: efecto de perspectiva.
// ------------------------------------------------------------
int xEnAltura(int x0, int y, int ySup, int yInf)
{
    const int xCentro = ANCHO_REJILLA / 2;
    const float avance = static_cast<float>(ySup - y) / (ySup - yInf);   // 0 arriba, 1 abajo
    const float escala = 1.0f + 0.8f * avance;
    return xCentro + static_cast<int>((x0 - xCentro) * escala);
}

// ------------------------------------------------------------
// Vereda: UNA franja con 2 filas de baldosas en tonos alternados
// ------------------------------------------------------------
void agregarVereda()
{
    const Color tono1 = colorRGB(226, 236, 245);
    const Color tono2 = colorRGB(208, 222, 236);
    const Color junta = colorRGB(160, 178, 196);
    const Color bordillo = colorRGB(120, 135, 150);

    const int ySup = Y_CARRETERA_INF - 1;
    const int yInf = Y_VEREDA_INF;
    const int yMedio = ySup - (ySup - yInf) * 4 / 10;      // la fila de arriba es mas baja

    const int ancho = 44;      // ancho de baldosa en la linea de arriba
    int indice = 0;
    for (int x = -400; x < ANCHO_REJILLA + 400; x += ancho, indice++) {
        const int xa = x, xb = x + ancho;

        // Fila superior (de ySup a yMedio) y fila inferior (de yMedio a yInf)
        const int filas[2][2] = { { ySup, yMedio }, { yMedio, yInf } };
        for (int f = 0; f < 2; f++) {
            const int yA = filas[f][0], yB = filas[f][1];
            const Color color = ((indice + f) % 2 == 0) ? tono1 : tono2;

            g_escena.agregarPoligono("Vereda - baldosa",
                { { xEnAltura(xa, yA, ySup, yInf), yB },
                  { xEnAltura(xb, yA, ySup, yInf), yB },
                  { xEnAltura(xb, yA, ySup, yInf), yA },
                  { xEnAltura(xa, yA, ySup, yInf), yA } },
                color, false);
        }
    }

    // Bordillo pegado a la pista
    g_escena.agregarLinea("Vereda - bordillo", 0, ySup, ANCHO_REJILLA - 1, ySup, bordillo);
}

// ------------------------------------------------------------
// Pasto de frente: dos tonos
// ------------------------------------------------------------
void agregarPastoFrente()
{
    agregarFranja("Pasto frente", 0, Y_VEREDA_INF, colorRGB(112, 176, 56));
    agregarFranja("Pasto frente - borde", Y_VEREDA_INF - 8, Y_VEREDA_INF,
                  colorRGB(132, 192, 66));
}

} // namespace

void dibujarCarretera()
{
    agregarPastoGrande();
    agregarCarretera();
    agregarVereda();
    agregarPastoFrente();
}
