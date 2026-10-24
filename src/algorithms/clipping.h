#pragma once
#include "types.h"
#include <vector>

extern Ventana g_ventana;   // ventana de visualizacion

// Integrante 2: Cohen-Sutherland
// Recorta la linea contra la ventana. Modifica los extremos para que queden dentro.
// Devuelve true si algo de la linea queda visible, false si queda totalmente fuera.
bool recortarLinea(int& x0, int& y0, int& x1, int& y1, const Ventana& ventana);

// Integrante 5: Sutherland-Hodgman
// Recorta el poligono contra la ventana. Devuelve los vertices del poligono recortado
// (si quedan menos de 3, el poligono estaba totalmente fuera).
std::vector<Punto2D> recortarPoligono(const std::vector<Punto2D>& puntos, const Ventana& ventana);