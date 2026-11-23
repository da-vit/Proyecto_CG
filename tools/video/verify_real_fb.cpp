// Verificacion contra el framebuffer REAL: incluye src/core/framebuffer.cpp tal cual
// (con un <GLFW/glfw3.h> falso que solo declara las funciones GL que usa mostrarBuffer),
// ejecuta construirEscena() + renderizarEscena() y compara con lo que genero gen.
//
// Uso: verify_real_fb <final_acumulado.f32> <ultimo_cuadro_sin_resalte.png>
#include "framebuffer.cpp"      // el real; bufferPixeles (static) queda visible en esta TU

#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include "scene.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static int a8(float v) { return (int)std::lround(std::min(1.0f, std::max(0.0f, v)) * 255.0f); }

int main(int argc, char** argv)
{
    if (argc < 3) { std::fprintf(stderr, "uso: verify_real_fb <f32> <png>\n"); return 2; }

    construirEscena();
    renderizarEscena();                         // framebuffer.cpp real

    int fallos = 0;

    // 1. Buffer float real vs buffer float del stub (bit a bit)
    std::ifstream f(argv[1], std::ios::binary);
    std::vector<Color> stub(bufferPixeles.size());
    f.read((char*)stub.data(), stub.size() * sizeof(Color));
    if (!f || f.gcount() != (std::streamsize)(stub.size() * sizeof(Color))) { std::fprintf(stderr, "f32 incompleto\n"); return 2; }
    size_t difF = 0;
    for (size_t i = 0; i < stub.size(); i++)
        if (std::memcmp(&stub[i], &bufferPixeles[i], sizeof(Color)) != 0) difF++;
    std::printf("[1] buffer float 320x240 (real vs stub): %zu celdas distintas de %zu\n", difF, stub.size());
    fallos += difF != 0;

    // 2. Parte de escena (800x600) del PNG vs escalado 2.5x del buffer real
    int w, h, c;
    unsigned char* png = stbi_load(argv[2], &w, &h, &c, 3);
    if (!png) { std::fprintf(stderr, "no se pudo leer %s\n", argv[2]); return 2; }
    size_t difP = 0;
    for (int Y = 0; Y < 600; Y++)
        for (int X = 0; X < 800; X++) {
            int gx = (int)((X + 0.5f) / 2.5f), gy = (int)(((599 - Y) + 0.5f) / 2.5f);
            const Color& col = bufferPixeles[gy * ANCHO_REJILLA + gx];
            const unsigned char* p = &png[(Y * w + X) * 3];
            if (p[0] != a8(col.rojo) || p[1] != a8(col.verde) || p[2] != a8(col.azul)) difP++;
        }
    std::printf("[2] PNG %dx%d, zona de escena 800x600 vs buffer real escalado 2.5x: %zu pixeles distintos de %d\n",
                w, h, difP, 800 * 600);
    fallos += difP != 0;
    stbi_image_free(png);

    std::printf(fallos ? "RESULTADO: DIFERENCIAS\n" : "RESULTADO: IDENTICO\n");
    return fallos ? 1 : 0;
}
