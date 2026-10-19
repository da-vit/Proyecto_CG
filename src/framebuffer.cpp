#include "framebuffer.h"
#include "config.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <vector>

// Buffer propio: un Color por cada celda de la rejilla
static std::vector<Color> bufferPixeles(ANCHO_REJILLA * ALTO_REJILLA, Color{0, 0, 0});

// Ventana de recorte actual (por defecto, todo el lienzo)
static Ventana ventanaRecorte = { 0, 0, ANCHO_REJILLA - 1, ALTO_REJILLA - 1 };

void establecerVentanaRecorte(const Ventana& ventana)
{
    ventanaRecorte.x_min = std::max(0, ventana.x_min);
    ventanaRecorte.y_min = std::max(0, ventana.y_min);
    ventanaRecorte.x_max = std::min(ANCHO_REJILLA - 1, ventana.x_max);
    ventanaRecorte.y_max = std::min(ALTO_REJILLA  - 1, ventana.y_max);
}

void ponerPixel(int x, int y, Color color)
{
    if (x < ventanaRecorte.x_min || x > ventanaRecorte.x_max ||
        y < ventanaRecorte.y_min || y > ventanaRecorte.y_max)
        return;

    bufferPixeles[y * ANCHO_REJILLA + x] = color;
}

void limpiarBuffer(Color color)
{
    for (auto& pixel : bufferPixeles)
        pixel = color;
}

void mostrarBuffer()
{
    // El buffer se escala al tamano actual de la ventana
    GLint area[4];
    glGetIntegerv(GL_VIEWPORT, area);

    glRasterPos2f(-1.0f, -1.0f);
    glPixelZoom(static_cast<float>(area[2]) / ANCHO_REJILLA,
                static_cast<float>(area[3]) / ALTO_REJILLA);
    glDrawPixels(ANCHO_REJILLA, ALTO_REJILLA, GL_RGB, GL_FLOAT, bufferPixeles.data());
}
