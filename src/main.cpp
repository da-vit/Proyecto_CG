#include <GLFW/glfw3.h>
#include <iostream>
#include "config.h"
#include "framebuffer.h"
#include "scene.h"

void framebufferSizeCallback(GLFWwindow*, int w, int h) { glViewport(0, 0, w, h); }

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT, "Simulador Urbano 2D - Entrega 1",
        nullptr, nullptr);

    if (!window) {
        std::cerr << "ERROR: No se pudo crear la ventana.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    renderScene();   // la escena es estatica: se calcula una sola vez

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        presentBuffer();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
