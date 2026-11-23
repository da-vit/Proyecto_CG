// ============================================================
// animacion.cpp - Avance de la animacion en el tiempo
// David Araoz: bucle de animacion y control por teclado
// ============================================================
#include "animacion.h"
#include "config.h"
#include <cmath>
#include <cstdio>

EstadoAnimacion g_animacion;

namespace {

const double PI = 3.14159265358979323846;

// Velocidades (unidades de la rejilla por segundo)
// El auto y el camion van a la misma velocidad para no chocar en su carril
const double VEL_AUTO      = 22.0;
const double VEL_CAMION    = 22.0;
const double VEL_REFLEJADO = 24.0;
const double RADIO_RUEDA   = 6.0;   // igual que en vehicles.cpp

// Una rueda que rueda sin resbalar gira  (distancia / radio)  radianes.
// Hacia la derecha el giro es horario, por eso el signo negativo.
double gradosPorAvance(double distancia)
{
    return -(distancia / RADIO_RUEDA) * 180.0 / PI;
}

// Los vehiculos dan la vuelta: cuando salen por la derecha, entran por
// la izquierda. El recorrido va de -70 a 330 (400 unidades).
double envolver(double x)
{
    const double inicio = -70.0, largo = 400.0;
    double r = std::fmod(x - inicio, largo);
    if (r < 0) r += largo;
    return inicio + r;
}

} // namespace

void actualizarAnimacion(double dt)
{
    EstadoAnimacion& a = g_animacion;
    if (a.pausa) return;

    if (a.activa[TRASLACION]) {
        a.distanciaAuto      += VEL_AUTO      * dt;
        a.distanciaCamion    += VEL_CAMION    * dt;
        a.distanciaReflejado += VEL_REFLEJADO * dt;
        a.relojNubes         += dt;
    }
    if (a.activa[ROTACION]) {
        // Las ruedas giran a la velocidad que corresponde a su vehiculo
        a.anguloRuedasAuto      += gradosPorAvance(VEL_AUTO      * dt);
        a.anguloRuedasCamion    += gradosPorAvance(VEL_CAMION    * dt);
        a.anguloRuedasReflejado += gradosPorAvance(VEL_REFLEJADO * dt);
        // Se deja el angulo entre -360 y 360 (una vuelta completa es lo mismo)
        a.anguloRuedasAuto      = std::fmod(a.anguloRuedasAuto, 360.0);
        a.anguloRuedasCamion    = std::fmod(a.anguloRuedasCamion, 360.0);
        a.anguloRuedasReflejado = std::fmod(a.anguloRuedasReflejado, 360.0);
    }
    if (a.activa[ESCALA])      a.relojEscala      += dt;
    if (a.activa[DISTORSION])  a.relojDistorsion  += dt;
    if (a.activa[COMPOSICION]) a.relojComposicion += dt;
}

void alternarTransformacion(int numero)
{
    if (numero < 1 || numero > 6) return;
    g_animacion.activa[numero] = !g_animacion.activa[numero];
    std::printf("[%d] %-12s -> %s\n", numero, nombreTransformacion(numero),
                g_animacion.activa[numero] ? "ACTIVADA" : "desactivada");
}

const char* nombreTransformacion(int numero)
{
    switch (numero) {
        case TRASLACION:  return "Traslacion";
        case ROTACION:    return "Rotacion";
        case ESCALA:      return "Escala";
        case REFLEXION:   return "Reflexion";
        case DISTORSION:  return "Distorsion";
        case COMPOSICION: return "Composicion";
    }
    return "?";
}

void mostrarEstadoAnimacion()
{
    std::printf("Animacion %s | ", g_animacion.pausa ? "EN PAUSA" : "en marcha");
    for (int i = 1; i <= 6; i++)
        std::printf("%d:%s%s ", i, nombreTransformacion(i), g_animacion.activa[i] ? "(si)" : "(no)");
    std::printf("\n");
}

// ------------------------------------------------------------
// Valores actuales
// ------------------------------------------------------------
double xAuto()          { return envolver( 92.0 + g_animacion.distanciaAuto); }
double xCamion()        { return envolver(240.0 + g_animacion.distanciaCamion); }
double xAutoReflejado() { return envolver(200.0 - g_animacion.distanciaReflejado); }

// Las nubes van y vienen con un seno: nunca salen de la ventana,
// por eso siempre se ven COMPLETAS.
double desplazamientoNube(int i)
{
    const double amplitud[3]  = { 8.0, 20.0, 6.0 };    // cuanto se alejan de su lugar
    const double velocidad[3] = { 0.35, 0.25, 0.45 };  // rapidez del vaiven
    const double fase[3]      = { 0.0, 1.7, 3.1 };     // para que no vayan sincronizadas
    if (i < 0 || i > 2) return 0;
    return amplitud[i] * std::sin(velocidad[i] * g_animacion.relojNubes + fase[i]);
}

// Escala: el arbol crece y se achica entre 0.75 y 1.25
double escalaArbol()
{
    if (!g_animacion.activa[ESCALA]) return 1.0;
    return 1.0 + 0.25 * std::sin(1.2 * g_animacion.relojEscala);
}

// Distorsion: el edificio se inclina a un lado y al otro (shx entre -0.15 y 0.15)
double deformacionEdificio()
{
    if (!g_animacion.activa[DISTORSION]) return 0.0;
    return 0.15 * std::sin(0.9 * g_animacion.relojDistorsion);
}

// Composicion: la senal se balancea (+-20 grados) y late (escala 0.9 a 1.2)
double anguloSenal()
{
    if (!g_animacion.activa[COMPOSICION]) return 0.0;
    return 20.0 * std::sin(1.3 * g_animacion.relojComposicion);
}

double escalaSenal()
{
    if (!g_animacion.activa[COMPOSICION]) return 1.0;
    return 1.05 + 0.15 * std::sin(2.6 * g_animacion.relojComposicion);
}
