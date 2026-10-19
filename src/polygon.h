#pragma once
#include "types.h"
#include <vector>

void fillPolygonScanline(const std::vector<Vec2>& pts, Color c);
void drawPolygonOutline(const std::vector<Vec2>& pts, Color c);
void rellenarPoligonoScanline(const std::vector<Punto2D>& puntos, Color color);
