#pragma once
// polygon.h - Coral Cuadros: relleno de poligonos con SCAN-LINE
#include "types.h"
#include <vector>

void rellenarPoligonoScanline(const std::vector<Punto2D>& puntos, Color color);
