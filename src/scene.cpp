#include "scene.h"
#include "objects.h"
#include "framebuffer.h"
#include "types.h"

// TODO (Integrante 1): agregar SceneObject / Scene para guardar los
// objetos en estructuras de datos (requisito del enunciado).

void renderScene()
{
    clearBuffer(rgb(135, 206, 235));   // cielo

    // Orden de dibujo: fondo -> frente
    drawSkyAndSigns();
    drawBuildings();
    drawHouseAndTrees();
    drawRoad();
    drawVehicles();
}
