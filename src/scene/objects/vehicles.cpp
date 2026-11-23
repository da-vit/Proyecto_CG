// ============================================================
// vehicles.cpp - Lurancy Chipa: vehiculos (auto y camion)
//   Reflexion del auto: Samuel Sanchez
//
// PRIMERA ENTREGA
//   - Carroceria / contornos : rectangulos, poligonos y lineas (Bresenham)
//   - Ruedas, llantas, faros : circunferencias con punto medio
//   - Los vehiculos se guardan en vectores de Auto y Camion y luego se
//     recorren para dibujarlos (no son instrucciones sueltas).
//
// SEGUNDA ENTREGA
//   Cada vehiculo se dibuja en coordenadas LOCALES: su esquina inferior
//   izquierda (base de las ruedas) es el punto (0,0). Su matriz lo coloca
//   en la escena:
//
//   [1] TRASLACION  : M = T(x, y)                     el vehiculo avanza
//   [2] ROTACION    : Mrueda = M * T(c) * R(theta) * T(-c)
//                     los rayos giran respecto al CENTRO c de cada rueda;
//                     luego M los lleva junto con el vehiculo
//                     (primero rota, luego se traslada)
//   [4] REFLEXION   : Mref = T(x, y) * T(22,0) * S(-1,1) * T(-22,0)
//                     auto reflejado respecto a su linea vertical central
//                     (x = 22): va por el carril de arriba hacia la izquierda.
//                     Se compara con el auto rojo original del carril de abajo.
// ============================================================
#include "objects.h"
#include "scene.h"
#include "config.h"
#include "types.h"
#include "matriz.h"
#include "animacion.h"
#include <cstdio>
#include <vector>

namespace {

// ---------------------------------------------------------------------------
// Medidas en la rejilla 320x240 (Y hacia arriba)
// ---------------------------------------------------------------------------
const int Y_CARRIL_ABAJO  = Y_CARRETERA_INF + 14;   // base de ruedas, carril de abajo
const int Y_CARRIL_ARRIBA = Y_CARRETERA_INF + 37;   // base de ruedas, carril de arriba
const int RADIO_RUEDA     = 6;
const int ANCHO_AUTO      = 44;

struct Auto   { double x; int y; Color carroceria; double anguloRuedas; bool reflejado; };
struct Camion { double x; int y; Color carga; Color cabina; double anguloRuedas; };

// ----- funciones auxiliares ------------------------------------------------

// Rueda con centro LOCAL (cx, cy):
//   neumatico (disco negro) y llanta (disco gris): punto medio
//   2 rayos cruzados (Bresenham) que GIRAN theta grados respecto al centro:
//       Mrueda = Mvehiculo * T(cx,cy) * R(theta) * T(-cx,-cy)
void dibujarRueda(const char* nombre, int cx, int cy, double theta,
                  const Matriz3x3& Mvehiculo)
{
    const Color neumatico = colorRGB(25, 25, 25);
    const Color llanta    = colorRGB(170, 170, 175);
    const Color rayo      = colorRGB(60, 60, 66);
    const int   rl        = RADIO_RUEDA - 2;            // radio de la llanta

    // Neumatico y llanta: solo los mueve el vehiculo
    g_escena.establecerMatriz(Mvehiculo);
    g_escena.agregarCirculo(nombre, cx, cy, RADIO_RUEDA, neumatico, true);
    g_escena.agregarCirculo(nombre, cx, cy, rl, llanta, true);

    // Rayos: ROTACION respecto al centro de la rueda y luego el vehiculo
    const Matriz3x3 Mrueda = componer({ Mvehiculo, traslacion(cx, cy), rotacion(theta),
                                        traslacion(-cx, -cy) });
    g_escena.establecerMatriz(Mrueda);
    g_escena.agregarLinea("Rueda - rayo", cx - rl, cy, cx + rl, cy, rayo);
    g_escena.agregarLinea("Rueda - rayo", cx, cy - rl, cx, cy + rl, rayo);

    g_escena.establecerMatriz(Mvehiculo);
    g_escena.agregarCirculo("Rueda - eje", cx, cy, 1, neumatico, true);
}

// ----- vehiculos (dibujados con la esquina inferior izquierda en (0,0)) ----

void dibujarAuto(const Auto& a)
{
    const Color contorno = colorRGB(60, 10, 10);
    const Color vidrio   = colorRGB(190, 215, 235);

    // Matriz del auto: traslacion, y si es el reflejado, ademas la reflexion
    Matriz3x3 M;
    if (!a.reflejado)
        M = traslacion(a.x, a.y);                                          // [1]
    else
        M = componer({ traslacion(a.x, a.y), traslacion(ANCHO_AUTO / 2, 0),
                       reflexionEjeY(), traslacion(-ANCHO_AUTO / 2, 0) });   // [4]
    g_escena.establecerMatriz(M);

    // Carroceria (44 x 14): de y=6 a y=19
    g_escena.agregarRectangulo("Auto - carroceria", 0, 6, 44, 14, a.carroceria, true, contorno);
    g_escena.agregarLinea("Auto - franja lateral", 0, 11, 43, 11, contorno);

    // Cabina (trapecio, y=20 .. y=36) y ventanas
    g_escena.agregarPoligono("Auto - cabina",
        { {7, 20}, {36, 20}, {30, 36}, {13, 36} }, a.carroceria, true, contorno);
    g_escena.agregarPoligono("Auto - ventana trasera",
        { {11, 21}, {20, 21}, {20, 34}, {14, 34} }, vidrio);
    g_escena.agregarPoligono("Auto - ventana delantera",
        { {22, 21}, {32, 21}, {29, 34}, {22, 34} }, vidrio);
    g_escena.agregarLinea("Auto - parante central", 21, 20, 21, 36, contorno);
    g_escena.agregarLinea("Auto - division de puertas", 21, 8, 21, 18, contorno);

    // Faros (circulos de punto medio): delantero amarillo, trasero rojo
    g_escena.agregarCirculo("Auto - faro delantero", 41, 15, 2, colorRGB(255, 240, 150), true);
    g_escena.agregarCirculo("Auto - faro trasero",    2, 15, 2, colorRGB(230, 60, 60), true);

    // Ruedas (con rayos que giran)
    dibujarRueda("Auto - rueda", 10, RADIO_RUEDA, a.anguloRuedas, M);
    dibujarRueda("Auto - rueda", 34, RADIO_RUEDA, a.anguloRuedas, M);

    g_escena.restablecerMatriz();

    // Evidencia en consola (tecla M)
    if (g_animacion.imprimirMatrices) {
        if (!a.reflejado) {
            imprimirMatriz("[1] TRASLACION - Auto rojo:  M = T(x, y)", M);
            const Matriz3x3 Mr = componer({ M, traslacion(10, RADIO_RUEDA), rotacion(a.anguloRuedas),
                                            traslacion(-10, -RADIO_RUEDA) });
            std::printf("[2] ROTACION - Rayos de la rueda trasera (theta = %.2f grados):\n"
                        "    Mrueda = T(x,y) * T(10,6) * R(theta) * T(-10,-6)"
                        "  (primero rota respecto al centro de la rueda, luego se traslada)\n",
                        a.anguloRuedas);
            imprimirMatriz("    Matriz resultante Mrueda:", Mr);
        } else {
            imprimirMatriz("[4] REFLEXION - Auto del carril de arriba:"
                           "  M = T(x,y) * T(22,0) * S(-1,1) * T(-22,0)", M);
            const PuntoF faro = transformar(M, { 41, 15 });
            std::printf("    El faro delantero local (41, 15) queda en (%.2f, %.2f):"
                        " ahora el auto mira a la IZQUIERDA\n", faro.x, faro.y);
        }
    }
}

void dibujarCamion(const Camion& c)
{
    const Color contorno = colorRGB(15, 25, 50);
    const Color vidrio   = colorRGB(200, 225, 245);

    const Matriz3x3 M = traslacion(c.x, c.y);                                // [1]
    g_escena.establecerMatriz(M);

    // Chasis (franja oscura bajo la caja y la cabina)
    g_escena.agregarRectangulo("Camion - chasis", 0, 6, 57, 5, colorRGB(55, 55, 60), true, contorno);

    // Caja de carga (37 x 44): y=10 .. y=53
    g_escena.agregarRectangulo("Camion - caja de carga", 0, 10, 37, 44, c.carga, true, contorno);

    // Cabina: parte baja rectangular + parte alta con parabrisas inclinado
    g_escena.agregarRectangulo("Camion - cabina baja", 38, 10, 19, 21, c.cabina, true, contorno);
    g_escena.agregarPoligono("Camion - cabina alta",
        { {38, 31}, {56, 31}, {50, 45}, {38, 45} }, c.cabina, true, contorno);
    g_escena.agregarPoligono("Camion - ventana",
        { {42, 32}, {53, 32}, {49, 43}, {42, 43} }, vidrio);

    // Rotulo en la caja de carga (segmentos de Bresenham)
    const int anchoRotulo = Escena::anchoTexto("CUSCO");
    g_escena.agregarTexto("Camion - rotulo", "CUSCO", 37 / 2 - anchoRotulo / 2, 30,
                          colorRGB(255, 255, 255));

    g_escena.agregarCirculo("Camion - faro", 55, 14, 2, colorRGB(255, 240, 150), true);

    // Ruedas: dos ejes traseros juntos y uno delantero
    dibujarRueda("Camion - rueda", 9,  RADIO_RUEDA, c.anguloRuedas, M);
    dibujarRueda("Camion - rueda", 22, RADIO_RUEDA, c.anguloRuedas, M);
    dibujarRueda("Camion - rueda", 48, RADIO_RUEDA, c.anguloRuedas, M);

    g_escena.restablecerMatriz();
}

} // namespace

void dibujarVehiculos()
{
    const EstadoAnimacion& anim = g_animacion;

    // Estructuras de datos con los vehiculos de la escena
    std::vector<Auto> autos;
    if (anim.activa[REFLEXION])   // [4] auto reflejado (carril de arriba, se dibuja primero: va atras)
        autos.push_back({ xAutoReflejado(), Y_CARRIL_ARRIBA, colorRGB(215, 45, 40),
                          anim.anguloRuedasReflejado, true });
    autos.push_back({ xAuto(), Y_CARRIL_ABAJO, colorRGB(215, 45, 40),          // auto rojo original
                      anim.anguloRuedasAuto, false });

    const std::vector<Camion> camiones = {
        { xCamion(), Y_CARRIL_ABAJO, colorRGB(30, 80, 165), colorRGB(95, 150, 210),
          anim.anguloRuedasCamion },                                         // camion azul
    };

    for (const Auto& a : autos)        dibujarAuto(a);
    for (const Camion& c : camiones)   dibujarCamion(c);
}
