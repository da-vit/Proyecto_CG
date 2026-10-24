#include "objects.h"
#include "scene.h"
#include "config.h"
#include "types.h"
#include <vector>

// ---------------------------------------------------------------------------
// Integrante 3: vehiculos (auto y camion).
//   - Los vehiculos se agregan a g_escena (igual que el resto de objetos);
//     el rasterizado y el recorte contra g_ventana los hace Escena::renderizar
//     con Bresenham, punto medio, scan-line y clipping.
//   - Carroceria / contornos : rectangulos, poligonos y lineas (Bresenham)
//   - Ruedas, llantas, faros : circunferencias con punto medio
// Los vehiculos se guardan en una estructura de datos (vectores de Car/Truck)
// y luego se recorren para dibujarlos (requisito: no dibujar con instrucciones
// sueltas). El eje Y crece hacia ARRIBA. El orden de insercion es el orden de
// dibujo: carroceria, cabina, ventanas, faros y ruedas encima.
// ---------------------------------------------------------------------------

namespace {

// ---------------------------------------------------------------------------
// Medidas tomadas de la imagen de referencia, convertidas a la grilla 320x240
// (Y hacia arriba). La carretera va de Y_CARRETERA_INF a Y_CARRETERA_SUP y los
// vehiculos van en el carril inferior: las ruedas apoyan en y = WHEEL_BASE_Y,
// debajo de la linea central. Si Integrante 2 cambia la carretera, solo hay que
// tocar estas constantes.
// ---------------------------------------------------------------------------
const int WHEEL_BASE_Y = Y_CARRETERA_INF + 17;  // borde inferior de las ruedas
const int WHEEL_R      = 6;
const int CAR_X        = 92;   // auto: x = 92..136
const int TRUCK_X      = 240;  // camion: x = 240..297

struct Car   { int x; int y; Color body; };      // (x,y) = esquina inf. izq. (base de ruedas)
struct Truck { int x; int y; Color cargo; Color cab; };

// ----- helpers de dibujo -----------------------------------------------------

// Rueda: neumatico (disco negro) y llanta (disco gris liso), como en la imagen.
// Dos circunferencias rellenas con el algoritmo del punto medio.
void dibujarRueda(const char* nombre, int cx, int cy)
{
    g_escena.agregarCirculo(nombre, cx, cy, WHEEL_R,     colorRGB(25, 25, 25), true);
    g_escena.agregarCirculo(nombre, cx, cy, WHEEL_R - 3, colorRGB(150, 150, 155), true);
}

// ----- vehiculos -------------------------------------------------------------

void dibujarAuto(const Car& car)
{
    const Color outline = colorRGB(60, 10, 10);
    const Color glass   = colorRGB(190, 215, 235);
    const int x = car.x, y = car.y;            // y = base de las ruedas

    // Carroceria (44 x 13): de y+6 a y+19
    g_escena.agregarRectangulo("Auto - carroceria", x, y + 6, 44, 14, car.body, true, outline);
    g_escena.agregarLinea("Auto - franja lateral", x, y + 11, x + 43, y + 11, outline);

    // Cabina (trapecio, y+19 .. y+36) y ventanas
    g_escena.agregarPoligono("Auto - cabina",
        { {x + 7, y + 20}, {x + 36, y + 20}, {x + 30, y + 36}, {x + 13, y + 36} },
        car.body, true, outline);
    g_escena.agregarPoligono("Auto - ventana trasera",
        { {x + 11, y + 21}, {x + 20, y + 21}, {x + 20, y + 34}, {x + 14, y + 34} },
        glass);
    g_escena.agregarPoligono("Auto - ventana delantera",
        { {x + 22, y + 21}, {x + 32, y + 21}, {x + 29, y + 34}, {x + 22, y + 34} },
        glass);
    g_escena.agregarLinea("Auto - parante central", x + 21, y + 20, x + 21, y + 36, outline);
    g_escena.agregarLinea("Auto - division de puertas", x + 21, y + 8, x + 21, y + 18, outline);

    // Faros (circulos de punto medio)
    g_escena.agregarCirculo("Auto - faro delantero", x + 41, y + 15, 2, colorRGB(255, 240, 150), true);
    g_escena.agregarCirculo("Auto - faro trasero",   x + 2,  y + 15, 2, colorRGB(230, 60, 60), true);

    // Ruedas
    dibujarRueda("Auto - rueda", x + 10, y + WHEEL_R);
    dibujarRueda("Auto - rueda", x + 34, y + WHEEL_R);
}

void dibujarCamion(const Truck& t)
{
    const Color outline = colorRGB(15, 25, 50);
    const Color glass   = colorRGB(200, 225, 245);
    const int x = t.x, y = t.y;                 // y = base de las ruedas

    // Chasis (franja oscura bajo la caja y la cabina)
    g_escena.agregarRectangulo("Camion - chasis", x, y + 6, 57, 5, colorRGB(55, 55, 60), true, outline);

    // Caja de carga (37 x 43): y+10 .. y+53
    g_escena.agregarRectangulo("Camion - caja de carga", x, y + 10, 37, 44, t.cargo, true, outline);

    // Cabina: parte baja rectangular + parte alta con parabrisas inclinado
    g_escena.agregarRectangulo("Camion - cabina baja", x + 38, y + 10, 19, 21, t.cab, true, outline);
    g_escena.agregarPoligono("Camion - cabina alta",
        { {x + 38, y + 31}, {x + 56, y + 31}, {x + 50, y + 45}, {x + 38, y + 45} },
        t.cab, true, outline);
    g_escena.agregarPoligono("Camion - ventana",
        { {x + 42, y + 32}, {x + 53, y + 32}, {x + 49, y + 43}, {x + 42, y + 43} },
        glass);
    g_escena.agregarCirculo("Camion - faro", x + 55, y + 14, 2, colorRGB(255, 240, 150), true);

    // Ruedas: dos ejes traseros juntos y uno delantero (como en la imagen)
    dibujarRueda("Camion - rueda", x + 9,  y + WHEEL_R);
    dibujarRueda("Camion - rueda", x + 22, y + WHEEL_R);
    dibujarRueda("Camion - rueda", x + 48, y + WHEEL_R);
}

} // namespace

void dibujarVehiculos()
{
    // Estructuras de datos con los vehiculos de la escena.
    const std::vector<Car> cars = {
        { CAR_X, WHEEL_BASE_Y, colorRGB(215, 45, 40) },                       // auto rojo
    };
    const std::vector<Truck> trucks = {
        { TRUCK_X, WHEEL_BASE_Y, colorRGB(30, 80, 165), colorRGB(95, 150, 210) },  // camion azul
    };

    for (const Car& c : cars)     dibujarAuto(c);
    for (const Truck& t : trucks) dibujarCamion(t);
}
