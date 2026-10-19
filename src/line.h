#pragma once
#include "types.h"
//dibujo una linea de (x0,y0) a (x1,y1) con el algoritmo de Bresenham
//funciona en los 8 octantes (cualquier direccion y cualquier pendiente)
void dibujarLineaBresenham(int x0, int y0, int x1, int y1, Color color);
