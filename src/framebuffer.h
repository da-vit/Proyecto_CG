#pragma once
#include "types.h"

void putPixel(int x, int y, Color c);  // escribe en el buffer propio
void clearBuffer(Color c);
void presentBuffer();                  // dibuja el buffer en pantalla
