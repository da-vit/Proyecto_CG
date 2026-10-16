#pragma once
#include "types.h"
#include <vector>

extern Window g_window;   // ventana de visualizacion

// Integrante 2: Cohen-Sutherland o Liang-Barsky
bool clipLine(int& x0, int& y0, int& x1, int& y1, const Window& w);

// Integrante 5: Sutherland-Hodgman
std::vector<Vec2> clipPolygon(const std::vector<Vec2>& pts, const Window& w);
