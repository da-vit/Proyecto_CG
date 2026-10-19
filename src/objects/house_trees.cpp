#include "objects.h"
#include "types.h"
#include "scene.h"
#include <string>
#include <vector>

// =====================================================================
// Integrante 5: casa y arboles
// Estos objetos NO se dibujan aqui: se AGREGAN a g_escena.
// Luego Escena::renderizar() (scene.cpp) los recorta contra g_ventana
// (recortarPoligono / recortarLinea) y los dibuja con scan-line,
// Bresenham y punto medio.
// Coordenadas: rejilla 320 x 240, y = 0 ABAJO de la pantalla.
// =====================================================================

const int SUELO_Y = 60;   // borde superior de la carretera (igual que GROUND_Y del Integrante 6)

// ---------- Estructuras de datos ----------
struct Casa {
    int x, y;            // esquina inferior izquierda de las paredes
    int ancho, alto;     // tamano de las paredes
    int alturaTecho;     // cuanto sube la punta del techo
    Color pared, techo, puerta, ventana, borde;
};

enum TipoCopa { COPA_TRIANGULAR, COPA_CIRCULAR };

struct Arbol {
    int x, y;                     // centro de la base del tronco
    int anchoTronco, altoTronco;
    TipoCopa tipo;
    int tamCopa;                  // triangular: mitad del ancho; circular: radio
    Color tronco, copa;
};

// ---------- Dibujo (agregar a la escena) de cada objeto ----------

static void agregarCasa(const Casa& c)
{
    // 1) Paredes (rectangulo relleno con borde)
    g_escena.agregarRectangulo("Casa - paredes", c.x, c.y, c.ancho, c.alto,
                               c.pared, true, c.borde);

    // 2) Techo triangular (sobresale 5 px a cada lado)
    std::vector<Punto2D> techo = {
        { c.x - 5,               c.y + c.alto },
        { c.x + c.ancho - 1 + 5, c.y + c.alto },
        { c.x + c.ancho / 2,     c.y + c.alto + c.alturaTecho }
    };
    g_escena.agregarPoligono("Casa - techo", techo, c.techo, true, c.borde);

    // 3) Puerta
    g_escena.agregarRectangulo("Casa - puerta", c.x + 6, c.y, c.ancho / 5, c.alto * 3 / 5,
                               c.puerta, true, c.borde);

    // 4) Ventana con cruz (dos lineas de Bresenham)
    int vx = c.x + c.ancho - 19, vy = c.y + c.alto / 2 - 4;
    int va = 13, vh = 11;
    g_escena.agregarRectangulo("Casa - ventana", vx, vy, va, vh, c.ventana, true, c.borde);
    g_escena.agregarLinea("Casa - ventana cruz V", vx + va / 2, vy, vx + va / 2, vy + vh - 1, c.borde);
    g_escena.agregarLinea("Casa - ventana cruz H", vx, vy + vh / 2, vx + va - 1, vy + vh / 2, c.borde);
}

static void agregarArbol(const Arbol& a, int numero)
{
    const std::string nombre = "Arbol " + std::to_string(numero);
    const Color bordeCopa   = colorRGB(20, 60, 25);
    const Color bordeTronco = colorRGB(60, 35, 15);

    // 1) Tronco
    g_escena.agregarRectangulo(nombre + " - tronco", a.x - a.anchoTronco / 2, a.y,
                               a.anchoTronco, a.altoTronco, a.tronco, true, bordeTronco);

    int yTope = a.y + a.altoTronco;   // donde empieza la copa

    // 2) Copa
    if (a.tipo == COPA_TRIANGULAR) {
        std::vector<Punto2D> copa = {
            { a.x - a.tamCopa, yTope },
            { a.x + a.tamCopa, yTope },
            { a.x,             yTope + a.tamCopa * 2 }
        };
        g_escena.agregarPoligono(nombre + " - copa", copa, a.copa, true, bordeCopa);
    } else {
        int cy = yTope + a.tamCopa - 2;
        g_escena.agregarCirculo(nombre + " - copa",  a.x, cy, a.tamCopa, a.copa, true);       // relleno
        g_escena.agregarCirculo(nombre + " - borde", a.x, cy, a.tamCopa, bordeCopa, false);  // contorno
    }
}

// ---------- Funcion publica (la llama construirEscena en scene.cpp) ----------
void dibujarCasaYArboles()
{
    const Color tronco = colorRGB(110, 70, 35);

    const std::vector<Casa> casas = {
        { 220, SUELO_Y, 42, 30, 20,
          colorRGB(245, 215, 70), colorRGB(210, 40, 40), colorRGB(120, 70, 30),
          colorRGB(150, 200, 240), colorRGB(40, 40, 40) }
    };

    const std::vector<Arbol> arboles = {
        {  69, SUELO_Y, 4,  6, COPA_TRIANGULAR,  8, tronco, colorRGB(40, 150, 60) },
        { 130, SUELO_Y, 4,  6, COPA_CIRCULAR,    8, tronco, colorRGB(50, 160, 70) },
        { 197, SUELO_Y, 6, 14, COPA_TRIANGULAR, 13, tronco, colorRGB(30, 130, 50) },
        { 318, SUELO_Y, 6, 14, COPA_TRIANGULAR, 13, tronco, colorRGB(30, 120, 45) }  // sale de la ventana -> clipping
    };

    for (const Casa& c : casas) agregarCasa(c);
    for (size_t i = 0; i < arboles.size(); i++) agregarArbol(arboles[i], (int)i + 1);
}