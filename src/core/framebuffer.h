#pragma once
#include "types.h"

// Escribe un pixel en el buffer (descarta lo que cae fuera de la ventana de recorte)
void ponerPixel(int x, int y, Color color);

// Rellena todo el buffer con un color
void limpiarBuffer(Color color);

// Muestra el buffer en pantalla
void mostrarBuffer();

// Ventana de recorte a nivel de pixel (la usa Escena al renderizar)
void establecerVentanaRecorte(const Ventana& ventana);