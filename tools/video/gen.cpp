// Generador headless de fotogramas: dibuja la escena ficha por ficha usando el
// codigo REAL del proyecto (scene.cpp, objects/*.cpp, algorithms/*.cpp). Solo
// framebuffer.cpp esta sustituido por framebuffer_stub.cpp.
//
// Uso: gen <directorio_salida> <ruta_fuente_ttf>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "clipping.h"
#include "config.h"
#include "framebuffer.h"
#include "framebuffer_stub.h"
#include "objects.h"
#include "scene.h"

namespace fs = std::filesystem;

// ------------------------------------------------------------
// Constantes de salida
// ------------------------------------------------------------
static const int   ANCHO_ESC = 800, ALTO_ESC = 600;   // escena (rejilla x 2.5)
static const int   ALTO_BARRA = 88;
static const int   ANCHO_VID = ANCHO_ESC, ALTO_VID = ALTO_ESC + ALTO_BARRA;
static const float ZOOM = 2.5f;
static const int   FPS = 30;

struct RGB8 { int r, g, b; };

// ------------------------------------------------------------
// Imagen + texto
// ------------------------------------------------------------
struct Img {
    int w = ANCHO_VID, h = ALTO_VID;
    std::vector<unsigned char> px = std::vector<unsigned char>(ANCHO_VID * ALTO_VID * 3, 0);
};

struct Fuente {
    std::vector<unsigned char> datos;
    stbtt_fontinfo info;
};

static void rect(Img& im, int x, int y, int w, int h, RGB8 c, float alfa = 1.0f)
{
    for (int j = std::max(0, y); j < std::min(im.h, y + h); j++)
        for (int i = std::max(0, x); i < std::min(im.w, x + w); i++) {
            unsigned char* p = &im.px[(j * im.w + i) * 3];
            p[0] = (unsigned char)std::lround(p[0] * (1 - alfa) + c.r * alfa);
            p[1] = (unsigned char)std::lround(p[1] * (1 - alfa) + c.g * alfa);
            p[2] = (unsigned char)std::lround(p[2] * (1 - alfa) + c.b * alfa);
        }
}

static std::vector<int> utf8(const std::string& s)
{
    std::vector<int> cps;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = s[i];
        int cp, n;
        if (c < 0x80)       { cp = c;        n = 1; }
        else if (c < 0xE0)  { cp = c & 0x1F; n = 2; }
        else if (c < 0xF0)  { cp = c & 0x0F; n = 3; }
        else                { cp = c & 0x07; n = 4; }
        for (int k = 1; k < n && i + k < s.size(); k++) cp = (cp << 6) | (s[i + k] & 0x3F);
        cps.push_back(cp);
        i += n;
    }
    return cps;
}

static float anchoTexto(Fuente& f, float px, const std::string& s)
{
    float sc = stbtt_ScaleForPixelHeight(&f.info, px), w = 0;
    for (int cp : utf8(s)) {
        int adv, lsb;
        stbtt_GetCodepointHMetrics(&f.info, cp, &adv, &lsb);
        w += adv * sc;
    }
    return w;
}

static void texto(Img& im, Fuente& f, float x, int base, float px, RGB8 c, const std::string& s)
{
    float sc = stbtt_ScaleForPixelHeight(&f.info, px);
    for (int cp : utf8(s)) {
        int adv, lsb, w, h, xo, yo;
        stbtt_GetCodepointHMetrics(&f.info, cp, &adv, &lsb);
        unsigned char* bmp = stbtt_GetCodepointBitmap(&f.info, sc, sc, cp, &w, &h, &xo, &yo);
        for (int j = 0; j < h; j++)
            for (int i = 0; i < w; i++) {
                int X = (int)x + xo + i, Y = base + yo + j;
                if (X < 0 || Y < 0 || X >= im.w || Y >= im.h) continue;
                float a = bmp[j * w + i] / 255.0f;
                unsigned char* p = &im.px[(Y * im.w + X) * 3];
                p[0] = (unsigned char)std::lround(p[0] * (1 - a) + c.r * a);
                p[1] = (unsigned char)std::lround(p[1] * (1 - a) + c.g * a);
                p[2] = (unsigned char)std::lround(p[2] * (1 - a) + c.b * a);
            }
        stbtt_FreeBitmap(bmp, nullptr);
        x += adv * sc;
    }
}

static void guardarPNG(const std::string& ruta, const Img& im)
{
    if (!stbi_write_png(ruta.c_str(), im.w, im.h, 3, im.px.data(), im.w * 3)) {
        std::fprintf(stderr, "ERROR escribiendo %s\n", ruta.c_str());
        std::exit(1);
    }
}

// ------------------------------------------------------------
// Clasificacion de fichas (por nombre) y grupos
// ------------------------------------------------------------
enum Grupo { G_CIELO, G_LEJANA, G_SOL, G_NUBES, G_EDIF, G_CARR, G_CASA, G_SEM, G_PARE, G_VEH, G_N };

struct InfoGrupo {
    const char* rotulo;     // nombre del capitulo
    int cuadros;            // cuadros (a 30 fps) que dura cada ficha en el video principal
    int pxPorCuadro;        // video de pixeles: pixeles revelados por cuadro
    int repeticion;         // video de pixeles: cuadros de video por cuadro de animacion
};

static const InfoGrupo INFO[G_N] = {
    { "Cielo",         5,  60, 1 },
    { "Ciudad lejana", 8,  60, 1 },
    { "Sol",           14,  5, 2 },
    { "Nubes",         7,  40, 1 },
    { "Edificios",     2,  60, 1 },
    { "Carretera",     4,  60, 1 },
    { "Casa y árboles",4,  40, 1 },
    { "Semáforo",      14,  5, 2 },
    { "Señal PARE",    8,   5, 2 },
    { "Vehículos",     4,  40, 1 },
};

static bool empieza(const std::string& s, const char* p) { return s.rfind(p, 0) == 0; }
static bool termina(const std::string& s, const char* suf)
{
    size_t n = std::strlen(suf);
    return s.size() >= n && s.compare(s.size() - n, n, suf) == 0;
}

// Devuelve el grupo y el nombre de la funcion agregar... que genero la ficha
// (las funciones viven en un namespace anonimo; se deduce del nombre de la ficha)
static void clasificar(const std::string& n, Grupo& g, std::string& fn)
{
    if      (empieza(n, "Cielo"))     { g = G_CIELO;  fn = "agregarCielo"; }
    else if (empieza(n, "Lejano"))    { g = G_LEJANA; fn = "agregarCiudadLejana"; }
    else if (empieza(n, "Sol"))       { g = G_SOL;    fn = "agregarSol"; }
    else if (empieza(n, "Nube"))      { g = G_NUBES;  fn = "agregarNube"; }
    else if (empieza(n, "Edificio"))  { g = G_EDIF;
        fn = termina(n, " - ventana") ? "agregarVentanas"
           : termina(n, " - techo")   ? "agregarTechoCristal" : "agregarEdificio"; }
    else if (empieza(n, "Pasto"))     { g = G_CARR;   fn = "agregarPastoGrande"; }
    else if (empieza(n, "Carretera")) { g = G_CARR;   fn = "agregarCarretera"; }
    else if (empieza(n, "Arbusto"))   { g = G_CASA;   fn = "agregarArbusto"; }
    else if (empieza(n, "Arbol"))     { g = G_CASA;   fn = "agregarArbol"; }
    else if (empieza(n, "Casa"))      { g = G_CASA;   fn = "agregarCasa"; }
    else if (empieza(n, "Semaforo"))  { g = G_SEM;    fn = "agregarSemaforo"; }
    else if (empieza(n, "PARE"))      { g = G_PARE;
        fn = termina(n, " - letra") ? "agregarTextoPare" : "agregarSenalPare"; }
    else if (n.find("rueda") != std::string::npos) { g = G_VEH; fn = "dibujarRueda"; }
    else if (empieza(n, "Auto"))      { g = G_VEH;    fn = "dibujarAuto"; }
    else if (empieza(n, "Camion"))    { g = G_VEH;    fn = "dibujarCamion"; }
    else { std::fprintf(stderr, "ERROR: ficha sin clasificar: %s\n", n.c_str()); std::exit(1); }
}

static const char* nombreTipo(TipoObjeto t)
{
    switch (t) {
    case TipoObjeto::Linea:    return "Línea";
    case TipoObjeto::Circulo:  return "Círculo";
    case TipoObjeto::Poligono: return "Polígono";
    }
    return "?";
}

static const char* nombreAlgoritmo(const ObjetoEscena& o)
{
    switch (o.tipo) {
    case TipoObjeto::Linea:    return "Bresenham";
    case TipoObjeto::Circulo:  return o.relleno ? "Punto medio (relleno)" : "Punto medio";
    case TipoObjeto::Poligono: return "Scanline";
    }
    return "?";
}

// ------------------------------------------------------------
// Composicion de fotogramas
// ------------------------------------------------------------
using Buffer = std::vector<Color>;

static int a8(float v) { return (int)std::lround(std::min(1.0f, std::max(0.0f, v)) * 255.0f); }

struct Resalte {
    const std::vector<unsigned char>* mascara = nullptr;   // celdas de la rejilla a teñir
    bool contorno = false;                                  // caja envolvente
    int x0 = 0, y0 = 0, x1 = -1, y1 = -1;                   // caja en celdas de rejilla
};

static int g_colX[ANCHO_ESC], g_filaY[ALTO_ESC];

static void prepararMapas()
{
    // Igual que glDrawPixels con glPixelZoom(2.5): la celda i cubre [2.5i, 2.5(i+1))
    for (int X = 0; X < ANCHO_ESC; X++) g_colX[X] = (int)((X + 0.5f) / ZOOM);
    for (int Y = 0; Y < ALTO_ESC; Y++)  g_filaY[Y] = (int)(((ALTO_ESC - 1 - Y) + 0.5f) / ZOOM);
}

static void dibujarEscena(Img& im, const Buffer& buf, const Resalte& r)
{
    for (int Y = 0; Y < ALTO_ESC; Y++)
        for (int X = 0; X < ANCHO_ESC; X++) {
            int gx = g_colX[X], gy = g_filaY[Y];
            const Color& c = buf[gy * ANCHO_REJILLA + gx];
            int R = a8(c.rojo), G = a8(c.verde), B = a8(c.azul);
            if (r.mascara && (*r.mascara)[gy * ANCHO_REJILLA + gx]) {
                R = (int)std::lround(R * 0.5 + 255 * 0.5);
                G = (int)std::lround(G * 0.5 + 225 * 0.5);
                B = (int)std::lround(B * 0.5 + 0   * 0.5);
            }
            unsigned char* p = &im.px[(Y * im.w + X) * 3];
            p[0] = R; p[1] = G; p[2] = B;
        }

    if (r.contorno && r.x1 >= r.x0) {
        int X0 = (int)std::floor(r.x0 * ZOOM) - 2, X1 = (int)std::ceil((r.x1 + 1) * ZOOM) + 1;
        int Ytop = ALTO_ESC - (int)std::ceil((r.y1 + 1) * ZOOM) - 2;
        int Ybot = ALTO_ESC - (int)std::floor(r.y0 * ZOOM) + 1;
        RGB8 m = { 255, 0, 200 };
        rect(im, X0, Ytop, X1 - X0 + 1, 2, m);                 // arriba
        rect(im, X0, Ybot - 1, X1 - X0 + 1, 2, m);             // abajo
        rect(im, X0, Ytop, 2, Ybot - Ytop + 1, m);             // izquierda
        rect(im, X1 - 1, Ytop, 2, Ybot - Ytop + 1, m);        // derecha
    }
}

struct DatosBarra {
    std::string nombre, tipo, algoritmo, funcion, capitulo;
    int k = 0, total = 0;            // ficha k de total
    long px = -1, pxTotal = 0;       // progreso de pixeles (modo pixeles), -1: no mostrar
};

static void dibujarBarra(Img& im, Fuente& f, const DatosBarra& d)
{
    rect(im, 0, ALTO_ESC, ANCHO_VID, ALTO_BARRA, { 24, 26, 32 });
    // progreso global
    rect(im, 0, ALTO_ESC, ANCHO_VID, 5, { 55, 60, 70 });
    rect(im, 0, ALTO_ESC, ANCHO_VID * d.k / std::max(1, d.total), 5, { 90, 200, 120 });

    texto(im, f, 16, ALTO_ESC + 42, 28, { 255, 255, 255 }, d.nombre);
    std::string cnt = "ficha " + std::to_string(d.k) + " / " + std::to_string(d.total);
    texto(im, f, ANCHO_VID - 16 - anchoTexto(f, 24, cnt), ALTO_ESC + 40, 24, { 150, 220, 170 }, cnt);

    float x = 16;
    std::string chips[4] = { "Tipo: " + d.tipo, d.algoritmo, d.funcion + "()", d.capitulo };
    int n = d.px >= 0 ? 4 : 4;
    for (int i = 0; i < n; i++) {
        texto(im, f, x, ALTO_ESC + 74, 18, i == 2 ? RGB8{ 240, 200, 110 } : RGB8{ 175, 182, 195 }, chips[i]);
        x += anchoTexto(f, 18, chips[i]) + 26;
    }
    if (d.px >= 0) {
        std::string s = "píxeles " + std::to_string(d.px) + " / " + std::to_string(d.pxTotal);
        texto(im, f, ANCHO_VID - 16 - anchoTexto(f, 18, s), ALTO_ESC + 74, 18, { 150, 220, 170 }, s);
    }
}

static void dibujarRotulo(Img& im, Fuente& f, const std::string& s)
{
    float w = anchoTexto(f, 30, s) + 40;
    int x = (int)((ANCHO_ESC - w) / 2);
    rect(im, x, 18, (int)w, 56, { 15, 17, 22 }, 0.82f);
    rect(im, x, 18, (int)w, 3, { 90, 200, 120 });
    texto(im, f, x + 20, 56, 30, { 255, 255, 255 }, s);
}

// ------------------------------------------------------------
// Utilidades
// ------------------------------------------------------------
static bool iguales(const Buffer& a, const Buffer& b)
{
    return a.size() == b.size() &&
           std::memcmp(a.data(), b.data(), a.size() * sizeof(Color)) == 0;
}

struct Tramo { std::string archivo; int cuadros; };

static void escribirTimeline(const std::string& ruta, const std::vector<Tramo>& t)
{
    std::ofstream o(ruta);
    for (const Tramo& x : t)
        o << "file '" << x.archivo << "'\nduration " << (double)x.cuadros / FPS << "\n";
    o << "file '" << t.back().archivo << "'\n";   // el demuxer ignora la duracion del ultimo
}

static std::string num(const char* fmt, int v)
{
    char b[64];
    std::snprintf(b, sizeof b, fmt, v);
    return b;
}

// ============================================================
int main(int argc, char** argv)
{
    if (argc < 3) { std::fprintf(stderr, "uso: gen <salida> <fuente.ttf>\n"); return 1; }
    const fs::path salida = argv[1];

    Fuente fuente;
    {
        std::ifstream in(argv[2], std::ios::binary);
        if (!in) { std::fprintf(stderr, "ERROR: no se pudo abrir la fuente %s\n", argv[2]); return 1; }
        fuente.datos.assign(std::istreambuf_iterator<char>(in), {});
        if (!stbtt_InitFont(&fuente.info, fuente.datos.data(), 0)) { std::fprintf(stderr, "fuente invalida\n"); return 1; }
    }
    fs::create_directories(salida / "frames");
    fs::create_directories(salida / "frames_px");
    prepararMapas();

    // ---- 1. Construir la escena real y resumir ----------------------------
    construirEscena();
    const std::vector<ObjetoEscena> todas = g_escena.objetos;
    const int total = (int)todas.size();

    // Conteo por funcion publica de objects.h (mismo orden que construirEscena)
    struct Pub { const char* nombre; void (*fn)(); size_t n; };
    Pub pubs[] = {
        { "dibujarCieloYSenales", dibujarCieloYSenales, 0 },
        { "dibujarEdificios",     dibujarEdificios,     0 },
        { "dibujarCarretera",     dibujarCarretera,     0 },
        { "dibujarCasaYArboles",  dibujarCasaYArboles,  0 },
        { "dibujarSenales",       dibujarSenales,       0 },
        { "dibujarVehiculos",     dibujarVehiculos,     0 },
    };
    size_t sumaPub = 0;
    for (Pub& p : pubs) {
        g_escena.limpiar();
        p.fn();
        p.n = g_escena.objetos.size();
        sumaPub += p.n;
    }
    g_escena.objetos = todas;

    std::vector<Grupo> grupo(total);
    std::vector<std::string> funcion(total);
    int porGrupo[G_N] = {};
    for (int i = 0; i < total; i++) {
        clasificar(todas[i].nombre, grupo[i], funcion[i]);
        porGrupo[grupo[i]]++;
    }
    // Capitulos = tramos contiguos de un mismo grupo (cada grupo debe aparecer una vez)
    struct Cap { Grupo g; int ini, fin; };   // [ini, fin]
    std::vector<Cap> caps;
    for (int i = 0; i < total; i++) {
        if (caps.empty() || caps.back().g != grupo[i]) caps.push_back({ grupo[i], i, i });
        else caps.back().fin = i;
    }
    if ((int)caps.size() != G_N) { std::fprintf(stderr, "ERROR: los grupos no son contiguos (%zu tramos)\n", caps.size()); return 1; }

    std::printf("=== Resumen de la escena (construirEscena) ===\n");
    std::printf("Total de fichas: %d\n", total);
    for (size_t c = 0; c < caps.size(); c++)
        std::printf("  %2zu. %-15s %3d fichas  (fichas %d-%d)\n", c + 1, INFO[caps[c].g].rotulo,
                    porGrupo[caps[c].g], caps[c].ini + 1, caps[c].fin + 1);
    std::printf("Por funcion publica de objects.h:\n");
    for (Pub& p : pubs) std::printf("  %-22s %3zu\n", p.nombre, p.n);
    std::printf("  suma = %zu  %s\n", sumaPub, sumaPub == (size_t)total ? "(coincide con el total)" : "(NO COINCIDE)");
    if (sumaPub != (size_t)total) return 1;
    // Coherencia: cielo..nubes == dibujarCieloYSenales; semaforo+PARE == dibujarSenales
    if ((size_t)(porGrupo[G_CIELO] + porGrupo[G_LEJANA] + porGrupo[G_SOL] + porGrupo[G_NUBES]) != pubs[0].n ||
        (size_t)(porGrupo[G_SEM] + porGrupo[G_PARE]) != pubs[4].n ||
        (size_t)porGrupo[G_EDIF] != pubs[1].n || (size_t)porGrupo[G_CARR] != pubs[2].n ||
        (size_t)porGrupo[G_CASA] != pubs[3].n || (size_t)porGrupo[G_VEH] != pubs[5].n) {
        std::fprintf(stderr, "ERROR: la clasificacion por nombre no coincide con las funciones publicas\n");
        return 1;
    }
    std::printf("Clasificacion por nombre coincide con los conteos de cada funcion publica.\n\n");

    // ---- 2. Fondo (escena vacia) y fotogramas acumulativos ----------------
    auto renderPrefijo = [&](int n) -> Buffer {
        g_escena.objetos.assign(todas.begin(), todas.begin() + n);
        renderizarEscena();                       // codigo real: fondo + cielo base + fichas 0..n-1
        return stubBuffer();
    };

    std::vector<Buffer> cuadro(total + 1);        // cuadro[k] = fichas 0..k-1 (cuadro[0] = fondo)
    std::vector<std::vector<EventoPixel>> eventos(total);
    cuadro[0] = renderPrefijo(0);
    for (int k = 0; k < total; k++) {
        cuadro[k + 1] = renderPrefijo(k + 1);

        // Pixeles que la ficha k envia a ponerPixel, en orden real
        g_escena.objetos = { todas[k] };
        establecerVentanaRecorte(g_ventana);
        stubRegistrar(&eventos[k]);
        g_escena.renderizar();
        stubRegistrar(nullptr);
        g_escena.objetos = todas;

        // Control: fotograma anterior + eventos reproducidos == fotograma real k+1
        Buffer rep = cuadro[k];
        for (const EventoPixel& e : eventos[k]) rep[e.y * ANCHO_REJILLA + e.x] = e.color;
        if (!iguales(rep, cuadro[k + 1])) {
            std::fprintf(stderr, "ERROR: la reproduccion de pixeles de la ficha %d no reproduce el fotograma\n", k + 1);
            return 1;
        }
    }
    std::printf("Control: para las %d fichas, (cuadro anterior + pixeles de ponerPixel) == cuadro de renderizarEscena.\n", total);

    // Ultimo fotograma: render completo real, sin truncar, vs acumulado
    g_escena.objetos = todas;
    renderizarEscena();
    const Buffer completo = stubBuffer();
    if (!iguales(completo, cuadro[total])) { std::fprintf(stderr, "ERROR: ultimo cuadro != render completo\n"); return 1; }
    {   // volcados para la verificacion contra el framebuffer real
        std::ofstream f(salida / "final_acumulado.f32", std::ios::binary);
        f.write((const char*)completo.data(), completo.size() * sizeof(Color));
        std::ofstream p(salida / "final_acumulado.ppm", std::ios::binary);
        p << "P6\n" << ANCHO_REJILLA << " " << ALTO_REJILLA << "\n255\n";
        for (int y = ALTO_REJILLA - 1; y >= 0; y--)
            for (int x = 0; x < ANCHO_REJILLA; x++) {
                const Color& c = completo[y * ANCHO_REJILLA + x];
                unsigned char b[3] = { (unsigned char)a8(c.rojo), (unsigned char)a8(c.verde), (unsigned char)a8(c.azul) };
                p.write((const char*)b, 3);
            }
    }

    // Metadatos por ficha
    {
        std::ofstream m(salida / "fichas.tsv");
        m << "n\tcapitulo\tfuncion\ttipo\tpixeles\tnombre\n";
        for (int i = 0; i < total; i++)
            m << i + 1 << "\t" << INFO[grupo[i]].rotulo << "\t" << funcion[i] << "\t"
              << nombreTipo(todas[i].tipo) << "\t" << eventos[i].size() << "\t" << todas[i].nombre << "\n";
    }

    // ---- 3. Video principal: 1 ficha = 1 fotograma ------------------------
    std::vector<Tramo> timeline;
    std::vector<int> capDe(total);
    for (size_t c = 0; c < caps.size(); c++)
        for (int i = caps[c].ini; i <= caps[c].fin; i++) capDe[i] = (int)c;

    auto barraFicha = [&](int i, int pxHechos = -1) {
        DatosBarra d;
        d.nombre = todas[i].nombre;
        d.tipo = nombreTipo(todas[i].tipo);
        d.algoritmo = nombreAlgoritmo(todas[i]);
        d.funcion = funcion[i];
        d.capitulo = std::to_string(capDe[i] + 1) + ". " + INFO[grupo[i]].rotulo;
        d.k = i + 1; d.total = total;
        d.px = pxHechos; d.pxTotal = (long)eventos[i].size();
        return d;
    };

    for (int i = 0; i < total; i++) {
        std::vector<unsigned char> mascara(ANCHO_REJILLA * ALTO_REJILLA, 0);
        Resalte r;
        r.x0 = ANCHO_REJILLA; r.y0 = ALTO_REJILLA; r.x1 = -1; r.y1 = -1;
        for (const EventoPixel& e : eventos[i]) {
            mascara[e.y * ANCHO_REJILLA + e.x] = 1;
            r.x0 = std::min(r.x0, e.x); r.x1 = std::max(r.x1, e.x);
            r.y0 = std::min(r.y0, e.y); r.y1 = std::max(r.y1, e.y);
        }
        r.mascara = &mascara; r.contorno = true;

        Img con, sin;
        dibujarEscena(con, cuadro[i + 1], r);
        dibujarBarra(con, fuente, barraFicha(i));
        dibujarEscena(sin, cuadro[i + 1], Resalte{});
        dibujarBarra(sin, fuente, barraFicha(i));
        std::string fcon = num("frames/ficha_%04d.png", i + 1);
        std::string fsin = num("frames/ficha_%04d_sin.png", i + 1);
        guardarPNG((salida / fcon).string(), con);
        guardarPNG((salida / fsin).string(), sin);

        // Parpadeo: bloques de 4 cuadros alternando resaltado / sin resaltado
        int h = INFO[grupo[i]].cuadros;
        if (h <= 4) timeline.push_back({ fcon, h });
        else
            for (int f = 0, on = 1; f < h; f += 4, on ^= 1)
                timeline.push_back({ on ? fcon : fsin, std::min(4, h - f) });

        // Pausa con rotulo al terminar cada grupo
        if (i == caps[capDe[i]].fin) {
            bool ultimo = (i == total - 1);
            std::string txt = std::to_string(capDe[i] + 1) + ". " + INFO[grupo[i]].rotulo +
                              "  ·  " + std::to_string(porGrupo[grupo[i]]) + " fichas";
            if (ultimo) txt = "Escena completa  ·  " + std::to_string(total) + " fichas";
            Img cap = sin;
            dibujarRotulo(cap, fuente, txt);
            std::string fc = num("frames/capitulo_%02d.png", capDe[i] + 1);
            guardarPNG((salida / fc).string(), cap);
            timeline.push_back({ fc, ultimo ? 3 * FPS : FPS });
        }
        if ((i + 1) % 50 == 0 || i + 1 == total) std::printf("principal: %d/%d fichas\n", i + 1, total);
    }
    escribirTimeline((salida / "timeline.txt").string(), timeline);
    {
        long n = 0; for (auto& t : timeline) n += t.cuadros;
        std::printf("Video principal: %ld cuadros a %d fps = %.1f s\n", n, FPS, (double)n / FPS);
    }

    // ---- 4. Video de pixeles: ponerPixel en su orden real -----------------
    std::vector<Tramo> timelinePx;
    int nPng = 0;
    long nCuadrosPx = 0;
    for (int i = 0; i < total; i++) {
        const auto& ev = eventos[i];
        const InfoGrupo& ig = INFO[grupo[i]];
        long n = (long)ev.size();
        int pasos = n == 0 ? 0 : (int)std::min<long>(30, std::max<long>(2, (n + ig.pxPorCuadro - 1) / ig.pxPorCuadro));
        Buffer cur = cuadro[i];
        for (int s = 0; s < pasos; s++) {
            long a = n * s / pasos, b = n * (s + 1) / pasos;
            std::vector<unsigned char> mascara(ANCHO_REJILLA * ALTO_REJILLA, 0);
            for (long j = a; j < b; j++) {
                cur[ev[j].y * ANCHO_REJILLA + ev[j].x] = ev[j].color;
                mascara[ev[j].y * ANCHO_REJILLA + ev[j].x] = 1;
            }
            Resalte r; r.mascara = &mascara;
            Img im;
            dibujarEscena(im, cur, r);
            dibujarBarra(im, fuente, barraFicha(i, b));
            std::string f = num("frames_px/px_%05d.png", ++nPng);
            guardarPNG((salida / f).string(), im);
            timelinePx.push_back({ f, ig.repeticion });
            nCuadrosPx += ig.repeticion;
        }
        if (!iguales(cur, cuadro[i + 1])) { std::fprintf(stderr, "ERROR: video de pixeles diverge en ficha %d\n", i + 1); return 1; }
        // Cuadro de cierre de ficha (sin resaltar), solo si la ficha tuvo pixeles
        if (pasos > 0) {
            Img im;
            dibujarEscena(im, cur, Resalte{});
            dibujarBarra(im, fuente, barraFicha(i, n));
            std::string f = num("frames_px/px_%05d.png", ++nPng);
            guardarPNG((salida / f).string(), im);
            timelinePx.push_back({ f, 1 });
            nCuadrosPx += 1;
        }
        if (i == caps[capDe[i]].fin) {
            bool ultimo = (i == total - 1);
            Img im;
            dibujarEscena(im, cuadro[i + 1], Resalte{});
            dibujarBarra(im, fuente, barraFicha(i, (long)ev.size()));
            std::string txt = std::to_string(capDe[i] + 1) + ". " + INFO[grupo[i]].rotulo;
            if (ultimo) txt = "Escena completa  ·  " + std::to_string(total) + " fichas";
            dibujarRotulo(im, fuente, txt);
            std::string f = num("frames_px/px_%05d.png", ++nPng);
            guardarPNG((salida / f).string(), im);
            timelinePx.push_back({ f, ultimo ? 3 * FPS : FPS });
            nCuadrosPx += ultimo ? 3 * FPS : FPS;
        }
        if ((i + 1) % 50 == 0 || i + 1 == total) std::printf("pixeles: %d/%d fichas\n", i + 1, total);
    }
    escribirTimeline((salida / "timeline_px.txt").string(), timelinePx);
    std::printf("Video de pixeles: %d PNG, %ld cuadros a %d fps = %.1f s\n", nPng, nCuadrosPx, FPS, (double)nCuadrosPx / FPS);
    std::printf("OK\n");
    return 0;
}
