#include "scene.h"
#include "objects.h"
#include "framebuffer.h"
#include "line.h"
#include "circle.h"
#include "polygon.h"
#include "clipping.h"
#include "config.h"

Escena g_escena;
bool   g_mostrarMarcoVentana = false;

// ------------------------------------------------------------
// Escena: agregar objetos
// ------------------------------------------------------------

void Escena::limpiar()
{
    objetos.clear();
}

void Escena::agregarLinea(const std::string& nombre, int x0, int y0, int x1, int y1,
                          Color color)
{
    ObjetoEscena objeto;
    objeto.nombre  = nombre;
    objeto.tipo    = TipoObjeto::Linea;
    objeto.puntos  = { {x0, y0}, {x1, y1} };
    objeto.color   = color;
    objetos.push_back(objeto);
}

void Escena::agregarCirculo(const std::string& nombre, int cx, int cy, int radio,
                            Color color, bool relleno)
{
    ObjetoEscena objeto;
    objeto.nombre  = nombre;
    objeto.tipo    = TipoObjeto::Circulo;
    objeto.puntos  = { {cx, cy} };
    objeto.radio   = radio;
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
    objeto.puntos        = puntos;
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
// Escena: renderizado con recorte
// ------------------------------------------------------------

static void renderizarObjeto(const ObjetoEscena& objeto)
{
    switch (objeto.tipo) {

    case TipoObjeto::Linea: {
        int x0 = objeto.puntos[0].x, y0 = objeto.puntos[0].y;
        int x1 = objeto.puntos[1].x, y1 = objeto.puntos[1].y;
        if (recortarLinea(x0, y0, x1, y1, g_ventana))            // Integrante 2
            dibujarLineaBresenham(x0, y0, x1, y1, objeto.color);
        break;
    }

    case TipoObjeto::Circulo: {
        const int cx = objeto.puntos[0].x;
        const int cy = objeto.puntos[0].y;
        const int radio = objeto.radio;

        // Rechazo rapido: caja del circulo totalmente fuera de la ventana
        if (cx + radio < g_ventana.x_min || cx - radio > g_ventana.x_max ||
            cy + radio < g_ventana.y_min || cy - radio > g_ventana.y_max)
            break;

        // El recorte parcial lo hace ponerPixel (ver establecerVentanaRecorte)
        if (objeto.relleno)
            rellenarCircunferenciaPuntoMedio(cx, cy, radio, objeto.color);   // Integrante 3
        else
            dibujarCircunferenciaPuntoMedio(cx, cy, radio, objeto.color);
        break;
    }

    case TipoObjeto::Poligono: {
        std::vector<Punto2D> recortado = recortarPoligono(objeto.puntos, g_ventana);   // Integrante 5
        if (recortado.size() >= 3)
            rellenarPoligonoScanline(recortado, objeto.color);                         // Integrante 4

        if (objeto.conContorno) {
            const size_t n = objeto.puntos.size();
            for (size_t i = 0; i < n; i++) {
                int x0 = objeto.puntos[i].x,           y0 = objeto.puntos[i].y;
                int x1 = objeto.puntos[(i + 1) % n].x, y1 = objeto.puntos[(i + 1) % n].y;
                if (recortarLinea(x0, y0, x1, y1, g_ventana))
                    dibujarLineaBresenham(x0, y0, x1, y1, objeto.colorContorno);
            }
        }
        break;
    }
    }
}

void Escena::renderizar() const
{
    for (const ObjetoEscena& objeto : objetos)
        renderizarObjeto(objeto);
}

// ------------------------------------------------------------
// Construccion y renderizado de la escena completa
// ------------------------------------------------------------

void construirEscena()
{
    g_escena.limpiar();

    // Orden de dibujo: fondo -> frente
    dibujarCieloYSenales();    // cielo, ciudad lejana, sol, nubes
    dibujarEdificios();        // edificios
    dibujarCarretera();        // pasto, pista, vereda (suelo)
    dibujarCasaYArboles();     // sobre el pasto
    dibujarVehiculos();        // autos sobre la pista
}

void renderizarEscena()
{
    const Ventana lienzoCompleto = { 0, 0, ANCHO_REJILLA - 1, ALTO_REJILLA - 1 };

    // 1. Todo el lienzo en gris oscuro (zona fuera de la ventana de visualizacion)
    establecerVentanaRecorte(lienzoCompleto);
    limpiarBuffer(colorRGB(40, 40, 40));

    // 2. Cielo solo dentro de la ventana de visualizacion
    establecerVentanaRecorte(g_ventana);
    const Color colorCielo = colorRGB(135, 206, 235);
    for (int y = g_ventana.y_min; y <= g_ventana.y_max; y++)
        for (int x = g_ventana.x_min; x <= g_ventana.x_max; x++)
            ponerPixel(x, y, colorCielo);

    // 3. Objetos (recortados contra g_ventana)
    g_escena.renderizar();

    // 4. Marco de la ventana (debug, tecla V), dibujado con Bresenham
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
