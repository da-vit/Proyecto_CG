// Sustituto headless de src/core/framebuffer.cpp: mismo buffer en memoria y misma
// logica de recorte, sin GLFW/OpenGL. Opcionalmente registra cada pixel aceptado
// por ponerPixel, en el orden real en que llega.
#include "framebuffer.h"
#include "framebuffer_stub.h"
#include "config.h"
#include <algorithm>
#include <vector>

static std::vector<Color> bufferPixeles(ANCHO_REJILLA * ALTO_REJILLA, Color{0, 0, 0});
static Ventana ventanaRecorte = { 0, 0, ANCHO_REJILLA - 1, ALTO_REJILLA - 1 };
static std::vector<EventoPixel>* registro = nullptr;

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
    if (registro) registro->push_back({x, y, color});
}

void limpiarBuffer(Color color)
{
    for (auto& pixel : bufferPixeles)
        pixel = color;
}

void mostrarBuffer() {}   // headless: nada que mostrar

const std::vector<Color>& stubBuffer() { return bufferPixeles; }
void stubRegistrar(std::vector<EventoPixel>* destino) { registro = destino; }
