#pragma once

struct Color   { float rojo, verde, azul; };
struct Punto2D { int x, y; };
struct Ventana { int x_min, y_min, x_max, y_max; };

// Crea un Color a partir de valores 0-255
inline Color colorRGB(int rojo, int verde, int azul)
{
    return { rojo / 255.0f, verde / 255.0f, azul / 255.0f };
}
