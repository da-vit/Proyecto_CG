// ============================================================
// text.cpp - Texto simple hecho SOLO con lineas de Bresenham
//
// Cada letra vive en una cuadricula de 3 x 5 celdas
// (x de 0 a 2, y de 0 a 4, con Y hacia ARRIBA) y se describe
// como una lista de segmentos. Cada segmento se guarda en la
// escena como una Linea, asi que se recorta con Cohen-Sutherland
// y se dibuja con Bresenham, igual que cualquier otra linea.
//
// Ejemplo, la letra T:          ###   <- segmento (0,4)-(2,4)
//                                #
//                                #    <- segmento (1,4)-(1,0)
//                                #
//                                #
// Se usa en la senal de PARE y en los letreros de los edificios.
// ============================================================
#include "scene.h"

namespace {

struct Segmento { int x0, y0, x1, y1; };

const int ANCHO_LETRA = 3;
const int ALTO_LETRA  = 5;
const int ESPACIO     = 1;   // celdas libres entre letra y letra

std::vector<Segmento> segmentosLetra(char letra)
{
    switch (letra) {
        case 'A': return { {0,0,0,3}, {0,3,1,4}, {1,4,2,3}, {2,3,2,0}, {0,2,2,2} };
        case 'B': return { {0,0,0,4}, {0,4,1,4}, {2,3,2,3}, {0,2,1,2}, {2,1,2,1}, {0,0,1,0} };
        case 'C': return { {2,4,0,4}, {0,4,0,0}, {0,0,2,0} };
        case 'D': return { {0,0,0,4}, {0,4,1,4}, {2,3,2,1}, {1,0,0,0} };
        case 'E': return { {0,0,0,4}, {0,4,2,4}, {0,2,1,2}, {0,0,2,0} };
        case 'F': return { {0,0,0,4}, {0,4,2,4}, {0,2,1,2} };
        case 'G': return { {2,4,0,4}, {0,4,0,0}, {0,0,2,0}, {2,0,2,2}, {2,2,1,2} };
        case 'H': return { {0,0,0,4}, {2,0,2,4}, {0,2,2,2} };
        case 'I': return { {0,4,2,4}, {1,4,1,0}, {0,0,2,0} };
        case 'L': return { {0,4,0,0}, {0,0,2,0} };
        case 'M': return { {0,0,0,4}, {2,0,2,4}, {0,4,1,3}, {1,3,2,4} };
        case 'N': return { {0,0,0,4}, {2,0,2,4}, {0,4,2,0} };
        case 'O': return { {0,0,0,4}, {0,4,2,4}, {2,4,2,0}, {2,0,0,0} };
        case 'P': return { {0,0,0,4}, {0,4,2,4}, {2,4,2,2}, {2,2,0,2} };
        case 'R': return { {0,0,0,4}, {0,4,2,4}, {2,4,2,2}, {2,2,0,2}, {1,2,2,0} };
        case 'S': return { {2,4,0,4}, {0,4,0,2}, {0,2,2,2}, {2,2,2,0}, {2,0,0,0} };
        case 'T': return { {0,4,2,4}, {1,4,1,0} };
        case 'U': return { {0,4,0,0}, {0,0,2,0}, {2,0,2,4} };
    }
    return {};   // espacio o letra no definida: no se dibuja nada
}

} // namespace

int Escena::anchoTexto(const std::string& texto, int escala)
{
    if (texto.empty()) return 0;
    const int n = static_cast<int>(texto.size());
    return (n * (ANCHO_LETRA + ESPACIO) - ESPACIO) * escala;
}

void Escena::agregarTexto(const std::string& nombre, const std::string& texto,
                          int x, int y, Color color, int escala)
{
    const int paso = (ANCHO_LETRA + ESPACIO) * escala;   // avance de una letra a la siguiente

    for (size_t i = 0; i < texto.size(); i++) {
        const int xLetra = x + static_cast<int>(i) * paso;
        for (const Segmento& s : segmentosLetra(texto[i]))
            agregarLinea(nombre,
                         xLetra + s.x0 * escala, y + s.y0 * escala,
                         xLetra + s.x1 * escala, y + s.y1 * escala, color);
    }
}
