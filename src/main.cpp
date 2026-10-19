#include <GLFW/glfw3.h>
#include <iostream>
#include "config.h"
#include "framebuffer.h"
#include "scene.h"

void callbackCambioTamano(GLFWwindow*, int ancho, int alto)
{
    glViewport(0, 0, ancho, alto);
}

void callbackTeclado(GLFWwindow* ventana, int tecla, int, int accion, int)
{
    if (accion != GLFW_PRESS) return;

    if (tecla == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(ventana, true);

    // V: mostrar/ocultar el marco de la ventana de visualizacion
    if (tecla == GLFW_KEY_V) {
        g_mostrarMarcoVentana = !g_mostrarMarcoVentana;
        renderizarEscena();
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
        ANCHO_VENTANA, ALTO_VENTANA, "Simulador Urbano 2D - Entrega 1",
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

    std::cout << "ESC: salir | V: mostrar/ocultar ventana de visualizacion\n";

    // La escena es estatica: se arma y se rasteriza una sola vez
    construirEscena();
    renderizarEscena();

    while (!glfwWindowShouldClose(ventana)) {
        glClear(GL_COLOR_BUFFER_BIT);
        mostrarBuffer();
        glfwSwapBuffers(ventana);
        glfwPollEvents();
    }

    glfwDestroyWindow(ventana);
    glfwTerminate();
    return 0;
}
