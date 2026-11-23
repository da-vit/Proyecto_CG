#pragma once
// ============================================================
// scene.h - ESTRUCTURAS DE DATOS DE LA ESCENA
// David Araoz: estructura base
//
// Toda la escena se guarda en un solo objeto Escena (g_escena),
// que tiene una lista (vector) de piezas basicas: lineas,
// circulos y poligonos. Nada se dibuja "suelto": primero se
// guarda en la lista y despues Escena::renderizar() la recorre
// y aplica los algoritmos:
//
//   Linea    -> recorte Cohen-Sutherland   + Bresenham
//   Circulo  -> rechazo por caja + recorte por pixel + punto medio
//   Poligono -> recorte Sutherland-Hodgman + relleno scan-line
//               (+ contorno: Cohen-Sutherland + Bresenham)
//
// SEGUNDA ENTREGA - "matriz actual":
//   Antes de agregar las piezas de un objeto se elige su matriz con
//   establecerMatriz(M). Cada punto que se agrega se TRANSFORMA
//   (p' = M * p) y recien despues se guarda. Asi el orden completo es:
//
//       TRANSFORMAR  ->  RECORTAR  ->  RASTERIZAR
//
//   La escena se vuelve a armar en cada frame desde las coordenadas
//   ORIGINALES, por eso el redondeo no acumula errores.
// ============================================================
#include <string>
#include <vector>
#include "types.h"
#include "matriz.h"

enum class TipoObjeto { Linea, Circulo, Poligono };

struct ObjetoEscena {
    std::string          nombre;                       // "Edificio torre", "Auto - rueda"...
    TipoObjeto           tipo = TipoObjeto::Linea;
    std::vector<Punto2D> puntos;                       // Linea: 2 | Circulo: 1 (centro) | Poligono: N
    int                  radio = 0;                    // solo Circulo
    Color                color{1, 1, 1};               // relleno o trazo
    bool                 relleno = false;              // Circulo: relleno o solo contorno
    bool                 conContorno = false;          // Poligono: dibujar borde
    Color                colorContorno{0, 0, 0};
};

// Resultado del recorte de un objeto contra la ventana de visualizacion
enum class EstadoRecorte { Visible, Parcial, Fuera };

class Escena {
public:
    // El orden de insercion es el orden de dibujo: lo ultimo tapa a lo anterior
    std::vector<ObjetoEscena> objetos;

    void limpiar();

    // Matriz que se aplica a todo lo que se agregue desde ahora
    void establecerMatriz(const Matriz3x3& M) { matrizActual = M; }
    void restablecerMatriz()                  { matrizActual = identidad(); }
    const Matriz3x3& matriz() const           { return matrizActual; }

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

    // Texto con letras hechas de segmentos (cada segmento es una Linea -> Bresenham)
    //   (x, y) esquina inferior izquierda del texto | escala: tamano de la letra
    void agregarTexto(const std::string& nombre, const std::string& texto,
                      int x, int y, Color color, int escala = 1);

    // Ancho en celdas que ocupa un texto (para centrarlo en un letrero)
    static int anchoTexto(const std::string& texto, int escala = 1);

    // Dibuja todos los objetos aplicando recorte contra g_ventana.
    // Si reportar = true, imprime en consola un resumen del recorte.
    void renderizar(bool reportar) const;

private:
    Matriz3x3 matrizActual = identidad();
    Punto2D   aplicarMatriz(int x, int y) const;   // transforma y redondea
};

extern Escena g_escena;
extern bool   g_mostrarMarcoVentana;   // tecla V

void construirEscena();    // limpia g_escena y llama a las funciones de objects.h
// fondo gris + g_escena.renderizar() + marco rojo opcional
void renderizarEscena(bool reportarRecorte = false);
