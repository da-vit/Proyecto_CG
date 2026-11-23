#pragma once
// Extras del stub headless de framebuffer (no existen en el framebuffer real).
// ponerPixel / limpiarBuffer / establecerVentanaRecorte / mostrarBuffer conservan
// exactamente la firma y la semantica de src/core/framebuffer.cpp.
#include <vector>
#include "types.h"

struct EventoPixel { int x, y; Color color; };

const std::vector<Color>& stubBuffer();                    // ANCHO_REJILLA*ALTO_REJILLA, fila 0 = y 0
void stubRegistrar(std::vector<EventoPixel>* destino);     // nullptr: dejar de registrar
