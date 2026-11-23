// ============================================================
// main.cpp - Punto de entrada del programa
// David Araoz: ventana, teclado y bucle de animacion
//
// En cada vuelta del bucle (un FRAME), como indica la diapositiva 2.10:
//   1. Se calcula el tiempo transcurrido dt (animacion dependiente del tiempo)
//   2. Se actualiza la animacion (posiciones, angulos, escalas...)
//   3. Se vuelve a armar la escena con las matrices nuevas
//   4. Se rasteriza (transformar -> recortar -> rasterizar) y se muestra
//
// Controles:
//   1..6    : activar / desactivar cada transformacion
//             1 Traslacion  2 Rotacion  3 Escala
//             4 Reflexion   5 Distorsion  6 Composicion
//   ESPACIO : pausar / reanudar la animacion
//   M       : imprimir en consola las matrices actuales
//   Flechas : mover la ventana de visualizacion (recorte)
//   Z / X   : achicar / agrandar la ventana de visualizacion
//   R       : restaurar la ventana inicial
//   V       : mostrar / ocultar el marco rojo de la ventana
//   ESC     : salir
// ============================================================

#include <GLFW/glfw3.h>
#include <iostream>
#include "config.h"
#include "framebuffer.h"
#include "clipping.h"
#include "scene.h"
#include "animacion.h"

// Si se cambia la ventana, en el siguiente frame se imprime el resumen del recorte
static bool g_reportarRecorte = true;

void callbackCambioTamano(GLFWwindow*, int ancho, int alto)
{
    glViewport(0, 0, ancho, alto);
}

void callbackTeclado(GLFWwindow* ventana, int tecla, int, int accion, int)
{
    if (accion != GLFW_PRESS && accion != GLFW_REPEAT) return;

    const int PASO = 4;   // celdas que se mueve/cambia la ventana por pulsacion

    // Teclas 1..6: transformaciones
    if (tecla >= GLFW_KEY_1 && tecla <= GLFW_KEY_6 && accion == GLFW_PRESS) {
        alternarTransformacion(tecla - GLFW_KEY_1 + 1);
        return;
    }

    switch (tecla) {
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(ventana, true); break;
        case GLFW_KEY_SPACE:
            if (accion == GLFW_PRESS) {
                g_animacion.pausa = !g_animacion.pausa;
                mostrarEstadoAnimacion();
            }
            break;
        case GLFW_KEY_M:      g_animacion.imprimirMatrices = true;       break;
        case GLFW_KEY_LEFT:   moverVentana(-PASO, 0);  g_reportarRecorte = true; break;
        case GLFW_KEY_RIGHT:  moverVentana(+PASO, 0);  g_reportarRecorte = true; break;
        case GLFW_KEY_DOWN:   moverVentana(0, -PASO);  g_reportarRecorte = true; break;
        case GLFW_KEY_UP:     moverVentana(0, +PASO);  g_reportarRecorte = true; break;
        case GLFW_KEY_Z:      cambiarTamanoVentana(-PASO); g_reportarRecorte = true; break;
        case GLFW_KEY_X:      cambiarTamanoVentana(+PASO); g_reportarRecorte = true; break;
        case GLFW_KEY_R:      restaurarVentana();      g_reportarRecorte = true; break;
        case GLFW_KEY_V:      g_mostrarMarcoVentana = !g_mostrarMarcoVentana;   break;
        default: break;
    }
}

int main()
{
    if (!glfwInit()) {
        std::cerr << "ERROR: No se pudo inicializar GLFW.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* ventana = glfwCreateWindow(
        ANCHO_VENTANA, ALTO_VENTANA, "Simulador Urbano 2D - Entrega 2: escena animada (UNSAAC)",
        nullptr, nullptr);

    if (!ventana) {
        std::cerr << "ERROR: No se pudo crear la ventana.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(ventana);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(ventana, callbackCambioTamano);
    glfwSetKeyCallback(ventana, callbackTeclado);
    glViewport(0, 0, ANCHO_VENTANA, ALTO_VENTANA);

    std::cout << "=== Simulador Urbano 2D - Entrega 2: escena animada ===\n"
              << "1 Traslacion | 2 Rotacion | 3 Escala | 4 Reflexion | 5 Distorsion | 6 Composicion\n"
              << "ESPACIO: pausa | M: imprimir matrices\n"
              << "Flechas: mover ventana | Z/X: achicar/agrandar | R: restaurar | V: marco | ESC: salir\n\n";
    mostrarEstadoAnimacion();

    double tiempoAnterior = glfwGetTime();

    while (!glfwWindowShouldClose(ventana)) {
        // 1. Tiempo transcurrido desde el frame anterior (en segundos)
        const double ahora = glfwGetTime();
        double dt = ahora - tiempoAnterior;
        tiempoAnterior = ahora;
        if (dt > 0.1) dt = 0.1;   // si la ventana se congela (p. ej. al moverla), no salta

        // 2. Actualizar la animacion
        actualizarAnimacion(dt);

        // 3 y 4. Armar la escena con las matrices nuevas y rasterizarla
        construirEscena();
        renderizarEscena(g_reportarRecorte);
        g_reportarRecorte = false;

        glClear(GL_COLOR_BUFFER_BIT);
        mostrarBuffer();
        glfwSwapBuffers(ventana);
        glfwPollEvents();
    }

    glfwDestroyWindow(ventana);
    glfwTerminate();
    return 0;
}
