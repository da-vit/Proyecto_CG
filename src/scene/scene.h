#pragma once
#include <string>
#include <vector>
#include "types.h"

// ============================================================
// ESTRUCTURAS DE DATOS DE LA ESCENA
// ============================================================

enum class TipoObjeto { Linea, Circulo, Poligono };

struct ObjetoEscena {
    std::string          nombre;                       // "Edificio rojo", "Rueda delantera"...
    TipoObjeto           tipo = TipoObjeto::Linea;
    std::vector<Punto2D> puntos;                       // Linea: 2 | Circulo: 1 (centro) | Poligono: N
    int                  radio = 0;                    // solo Circulo
    Color                color{1, 1, 1};               // relleno o trazo
    bool                 relleno = false;              // Circulo: relleno o solo contorno
    bool                 conContorno = false;          // Poligono: dibujar borde
    Color                colorContorno{0, 0, 0};
};

class Escena {
public:
    // El orden de insercion es el orden de dibujo: lo ultimo tapa a lo anterior
    std::vector<ObjetoEscena> objetos;

    void limpiar();

    void agregarLinea(const std::string& nombre, int x0, int y0, int x1, int y1,
                      Color color);

    void agregarCirculo(const std::string& nombre, int cx, int cy, int radio,
                        Color color, bool relleno = true);

    // Poligono siempre relleno (scan-line); opcionalmente con borde
    void agregarPoligono(const std::string& nombre, const std::vector<Punto2D>& puntos,
                         Color colorRelleno, bool conContorno = false,
                         Color colorContorno = Color{0, 0, 0});

    // Rectangulo [x, x+ancho-1] x [y, y+alto-1], guardado como poligono
    void agregarRectangulo(const std::string& nombre, int x, int y, int ancho, int alto,
                           Color colorRelleno, bool conContorno = false,
                           Color colorContorno = Color{0, 0, 0});

    // Dibuja todos los objetos aplicando recorte contra g_ventana
    void renderizar() const;
};

extern Escena g_escena;
extern bool   g_mostrarMarcoVentana;   // tecla V

void construirEscena();    // limpia g_escena y llama a las funciones de objects.h
void renderizarEscena();   // fondo + g_escena.renderizar() + marco opcional