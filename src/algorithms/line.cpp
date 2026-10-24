#include "line.h"
#include "framebuffer.h"
#include <cstdlib>   
//Algoritmo de Bresenham Keidy Sharmely Cochama Mamani
//este algortimo no usa decimales ni divisiones
void dibujarLineaBresenham(int x0, int y0, int x1, int y1, Color color)
{
    int dx = std::abs(x1 - x0);           //distancia horizontal (siempre positiva)
    int dy = std::abs(y1 - y0);           //distancia vertical   (siempre positiva)
    int pasoX = (x1 >= x0) ? 1 : -1;      //hacia donde avanza x: derecha (+1) o izquierda (-1)
    int pasoY = (y1 >= y0) ? 1 : -1;      //hacia donde avanza y: arriba (+1) o abajo (-1)
    int x = x0;
    int y = y0;
    if (dx >= dy) {
        //CASOS 
        //CASO 1: pendiente |m| <= 1 (linea mas acotada 
        //x avanza SIEMPRE y avanza solo a veces
        int p = 2 * dy - dx;              //parametro de decision inicial
        for (int i = 0; i <= dx; i++) {
            ponerPixel(x, y, color);
            x += pasoX;
            if (p < 0) {
                p += 2 * dy;              //y se queda igual
            }
            else {
                y += pasoY;               //y tambien avanza
                p += 2 * dy - 2 * dx;
            }
        }
    }
    else {
        //CASO 2: pendiente |m| > 1 (linea mas parada)
        //se intercambian los papeles: y avanza SIEMPRE ,x solo a veces
        int p = 2 * dx - dy;
        for (int i = 0; i <= dy; i++) {
            ponerPixel(x, y, color);
            y += pasoY;
            if (p < 0) {
                p += 2 * dx;
            }
            else {
                x += pasoX;
                p += 2 * dx - 2 * dy;
            }
        }
    }
}
