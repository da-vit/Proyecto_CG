#pragma once
#include "types.h"
// Dibuja una linea de (x0,y0) a (x1,y1) con el algoritmo de Bresenham.
// Funciona en los 8 octantes (cualquier direccion y cualquier pendiente).
void dibujarLineaBresenham(int x0, int y0, int x1, int y1, Color color);
