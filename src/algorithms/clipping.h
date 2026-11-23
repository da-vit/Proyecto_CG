#pragma once
// ============================================================
// clipping.h - Ventana de visualizacion y algoritmos de recorte
// ============================================================
#include "types.h"
#include <vector>

extern Ventana g_ventana;   // ventana de visualizacion actual

// Control de la ventana con el teclado
void moverVentana(int dx, int dy);       // desplaza la ventana
void cambiarTamanoVentana(int delta);    // delta > 0 agranda, delta < 0 achica
void restaurarVentana();                 // vuelve a la ventana inicial

// Cohen-Sutherland: recorta la linea contra la ventana. Modifica los extremos
// para que queden dentro. Devuelve true si algo de la linea queda visible,
// false si queda totalmente fuera.
bool recortarLinea(int& x0, int& y0, int& x1, int& y1, const Ventana& ventana);

// Sutherland-Hodgman: recorta el poligono contra la ventana. Devuelve los
// vertices del poligono recortado (si quedan menos de 3, estaba totalmente fuera).
std::vector<Punto2D> recortarPoligono(const std::vector<Punto2D>& puntos, const Ventana& ventana);
