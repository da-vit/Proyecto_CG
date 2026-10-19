#pragma once
#include "types.h"
#include <vector>

extern Ventana g_ventana;   // ventana de visualizacion

// Integrante 2: Cohen-Sutherland o Liang-Barsky
bool recortarLinea(int& x0, int& y0, int& x1, int& y1, const Ventana& v);

// Integrante 5: Sutherland-Hodgman
std::vector<Punto2D> recortarPoligono(const std::vector<Punto2D>& puntos, const Ventana& v);