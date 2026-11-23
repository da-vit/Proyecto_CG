// ============================================================
// scene.cpp - Escena: guardar objetos y dibujarlos con recorte
// David Araoz: estructura base de la escena
// ============================================================
#include <cmath>
#include <iostream>
#include <set>
#include "scene.h"
#include "objects.h"
#include "framebuffer.h"
#include "line.h"
#include "circle.h"
#include "polygon.h"
#include "clipping.h"
#include "config.h"
#include "animacion.h"

Escena g_escena;
bool   g_mostrarMarcoVentana = true;   // el marco rojo se ve desde el inicio

// ------------------------------------------------------------
// 1. Agregar objetos a la lista
// ------------------------------------------------------------

void Escena::limpiar()
{
    objetos.clear();
    restablecerMatriz();
}

// p' = M * p, y se redondea a la celda mas cercana
Punto2D Escena::aplicarMatriz(int x, int y) const
{
    const PuntoF p = transformar(matrizActual, { static_cast<double>(x), static_cast<double>(y) });
    return { static_cast<int>(std::lround(p.x)), static_cast<int>(std::lround(p.y)) };
}

void Escena::agregarLinea(const std::string& nombre, int x0, int y0, int x1, int y1,
                          Color color)
{
    ObjetoEscena objeto;
    objeto.nombre  = nombre;
    objeto.tipo    = TipoObjeto::Linea;
    objeto.puntos  = { aplicarMatriz(x0, y0), aplicarMatriz(x1, y1) };
    objeto.color   = color;
    objetos.push_back(objeto);
}

void Escena::agregarCirculo(const std::string& nombre, int cx, int cy, int radio,
                            Color color, bool relleno)
{
    ObjetoEscena objeto;
    objeto.nombre  = nombre;
    objeto.tipo    = TipoObjeto::Circulo;
    objeto.puntos  = { aplicarMatriz(cx, cy) };   // se transforma el CENTRO

    // El algoritmo de punto medio solo dibuja circunferencias, asi que el
    // radio se multiplica por el factor de escala de la matriz:
    //   factor = raiz(|det|), con det = m00*m11 - m01*m10
    // (rotacion, traslacion, reflexion y deformacion tienen |det| = 1)
    const Matriz3x3& M = matrizActual;
    const double factor = std::sqrt(std::fabs(M.m[0][0] * M.m[1][1] - M.m[0][1] * M.m[1][0]));
    objeto.radio   = static_cast<int>(std::lround(radio * factor));
    objeto.color   = color;
    objeto.relleno = relleno;
    objetos.push_back(objeto);
}

void Escena::agregarPoligono(const std::string& nombre, const std::vector<Punto2D>& puntos,
                             Color colorRelleno, bool conContorno, Color colorContorno)
{
    ObjetoEscena objeto;
    objeto.nombre        = nombre;
    objeto.tipo          = TipoObjeto::Poligono;
    for (const Punto2D& p : puntos)
        objeto.puntos.push_back(aplicarMatriz(p.x, p.y));   // cada vertice: p' = M * p
    objeto.color         = colorRelleno;
    objeto.relleno       = true;
    objeto.conContorno   = conContorno;
    objeto.colorContorno = colorContorno;
    objetos.push_back(objeto);
}

void Escena::agregarRectangulo(const std::string& nombre, int x, int y, int ancho, int alto,
                               Color colorRelleno, bool conContorno, Color colorContorno)
{
    agregarPoligono(nombre,
                    { {x, y}, {x + ancho - 1, y},
                      {x + ancho - 1, y + alto - 1}, {x, y + alto - 1} },
                    colorRelleno, conContorno, colorContorno);
}

// ------------------------------------------------------------
// 2. Dibujar un objeto: primero se RECORTA, luego se RASTERIZA.
//    Devuelve si el objeto quedo visible, parcial o fuera.
// ------------------------------------------------------------

static bool puntoDentro(const Punto2D& p, const Ventana& v)
{
    return p.x >= v.x_min && p.x <= v.x_max && p.y >= v.y_min && p.y <= v.y_max;
}

static EstadoRecorte renderizarObjeto(const ObjetoEscena& objeto)
{
    const Ventana& v = g_ventana;

    switch (objeto.tipo) {

    case TipoObjeto::Linea: {
        int x0 = objeto.puntos[0].x, y0 = objeto.puntos[0].y;
        int x1 = objeto.puntos[1].x, y1 = objeto.puntos[1].y;

        if (!recortarLinea(x0, y0, x1, y1, v))                 // Cohen-Sutherland
            return EstadoRecorte::Fuera;
        dibujarLineaBresenham(x0, y0, x1, y1, objeto.color);   // Bresenham

        const bool completa = puntoDentro(objeto.puntos[0], v) && puntoDentro(objeto.puntos[1], v);
        return completa ? EstadoRecorte::Visible : EstadoRecorte::Parcial;
    }

    case TipoObjeto::Circulo: {
        const int cx = objeto.puntos[0].x;
        const int cy = objeto.puntos[0].y;
        const int r  = objeto.radio;

        // a) Rechazo rapido: la caja que encierra al circulo esta totalmente fuera
        if (cx + r < v.x_min || cx - r > v.x_max || cy + r < v.y_min || cy - r > v.y_max)
            return EstadoRecorte::Fuera;

        // b) Si la caja esta parcialmente fuera, el recorte se hace pixel por pixel:
        //    ponerPixel() descarta los puntos fuera de la ventana (framebuffer.cpp)
        if (objeto.relleno)
            rellenarCircunferenciaPuntoMedio(cx, cy, r, objeto.color);   // punto medio
        else
            dibujarCircunferenciaPuntoMedio(cx, cy, r, objeto.color);

        const bool completo = cx - r >= v.x_min && cx + r <= v.x_max &&
                              cy - r >= v.y_min && cy + r <= v.y_max;
        return completo ? EstadoRecorte::Visible : EstadoRecorte::Parcial;
    }

    case TipoObjeto::Poligono: {
        std::vector<Punto2D> recortado = recortarPoligono(objeto.puntos, v);   // Sutherland-Hodgman
        if (recortado.size() < 3)
            return EstadoRecorte::Fuera;

        rellenarPoligonoScanline(recortado, objeto.color);                    // scan-line

        if (objeto.conContorno) {
            const size_t n = objeto.puntos.size();
            for (size_t i = 0; i < n; i++) {
                int x0 = objeto.puntos[i].x,           y0 = objeto.puntos[i].y;
                int x1 = objeto.puntos[(i + 1) % n].x, y1 = objeto.puntos[(i + 1) % n].y;
                if (recortarLinea(x0, y0, x1, y1, v))
                    dibujarLineaBresenham(x0, y0, x1, y1, objeto.colorContorno);
            }
        }

        for (const Punto2D& p : objeto.puntos)
            if (!puntoDentro(p, v)) return EstadoRecorte::Parcial;
        return EstadoRecorte::Visible;
    }
    }
    return EstadoRecorte::Fuera;
}

// ------------------------------------------------------------
// 3. Dibujar toda la lista y mostrar en consola que se recorto
// ------------------------------------------------------------

void Escena::renderizar(bool reportar) const
{
    int visibles = 0, parciales = 0, fuera = 0;
    std::set<std::string> nombresParciales, nombresFuera;   // sin repetir

    for (const ObjetoEscena& objeto : objetos) {
        switch (renderizarObjeto(objeto)) {
            case EstadoRecorte::Visible: visibles++;                                  break;
            case EstadoRecorte::Parcial: parciales++; nombresParciales.insert(objeto.nombre); break;
            case EstadoRecorte::Fuera:   fuera++;     nombresFuera.insert(objeto.nombre);     break;
        }
    }

    if (!reportar) return;

    const Ventana& v = g_ventana;
    std::cout << "----------------------------------------------------\n"
              << "Ventana de visualizacion: x[" << v.x_min << ", " << v.x_max
              << "]  y[" << v.y_min << ", " << v.y_max << "]\n"
              << "Piezas: " << objetos.size() << "  | completas: " << visibles
              << "  | recortadas: " << parciales << "  | fuera: " << fuera << "\n";

    std::cout << "Recortadas parcialmente:";
    for (const std::string& n : nombresParciales) std::cout << " [" << n << "]";
    std::cout << "\nTotalmente fuera (no se dibujan):";
    for (const std::string& n : nombresFuera) std::cout << " [" << n << "]";
    std::cout << "\n";
}

// ------------------------------------------------------------
// 4. Construccion y renderizado de la escena completa
// ------------------------------------------------------------

void construirEscena()
{
    g_escena.limpiar();

    if (g_animacion.imprimirMatrices)
        std::cout << "\n================ MATRICES ACTUALES (p' = M * p) ================\n";

    // Orden de dibujo: del fondo hacia el frente
    dibujarCielo();          // 1. cielo, ciudad lejana, sol, nubes
    dibujarEdificios();      // 2. edificios con ventanas y letreros
    dibujarCarretera();      // 3. pasto, pista, lineas y cruce peatonal
    dibujarCasaYArboles();   // 4. casa, arboles y arbustos (sobre el pasto)
    dibujarSenales();        // 5. semaforo, senal PARE y postes de luz
    dibujarVehiculos();      // 6. auto y camion (sobre la pista)

    g_escena.restablecerMatriz();
    if (g_animacion.imprimirMatrices) {
        std::cout << "=================================================================\n\n";
        g_animacion.imprimirMatrices = false;   // solo se imprimen una vez por tecla M
    }
}

void renderizarEscena(bool reportarRecorte)
{
    const Ventana lienzoCompleto = { 0, 0, ANCHO_REJILLA - 1, ALTO_REJILLA - 1 };

    // 1. Todo el lienzo en gris oscuro: es la zona FUERA de la ventana de visualizacion
    establecerVentanaRecorte(lienzoCompleto);
    limpiarBuffer(colorRGB(40, 40, 40));

    // 2. Objetos de la escena, recortados contra g_ventana
    establecerVentanaRecorte(g_ventana);
    g_escena.renderizar(reportarRecorte);

    // 3. Marco rojo de la ventana de visualizacion (tecla V), con Bresenham
    establecerVentanaRecorte(lienzoCompleto);
    if (g_mostrarMarcoVentana) {
        const Color rojo = colorRGB(255, 0, 0);
        const Ventana& v = g_ventana;
        dibujarLineaBresenham(v.x_min, v.y_min, v.x_max, v.y_min, rojo);
        dibujarLineaBresenham(v.x_max, v.y_min, v.x_max, v.y_max, rojo);
        dibujarLineaBresenham(v.x_max, v.y_max, v.x_min, v.y_max, rojo);
        dibujarLineaBresenham(v.x_min, v.y_max, v.x_min, v.y_min, rojo);
    }
}
