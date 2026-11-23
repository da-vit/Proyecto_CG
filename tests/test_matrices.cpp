// ============================================================
// test_matrices.cpp - Pruebas de las matrices SIN ventana grafica
//
// Se comprueba cada transformacion con un punto de valor conocido
// (calculado a mano) antes de usarla en la escena.
//
// Compilar y ejecutar:
//   cmake --build build --target test_matrices && ./build/test_matrices
// o directamente:
//   g++ -std=c++17 -Isrc/core tests/test_matrices.cpp src/core/matriz.cpp -o test_matrices
// ============================================================
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "matriz.h"

static int fallos = 0;

static void comprobar(const char* nombre, PuntoF obtenido, PuntoF esperado)
{
    const double tol = 1e-9;
    const bool ok = std::fabs(obtenido.x - esperado.x) < tol &&
                    std::fabs(obtenido.y - esperado.y) < tol;
    std::printf("%-48s (%9.4f, %9.4f)  esperado (%9.4f, %9.4f)  %s\n", nombre,
                obtenido.x, obtenido.y, esperado.x, esperado.y, ok ? "OK" : "FALLA");
    if (!ok) fallos++;
}

int main()
{
    std::printf("=== Pruebas de matrices homogeneas 2D ===\n\n");

    // 1. Caso resuelto del docente (Laboratorio 07, 3.5):
    //    (2,1) --S(2,1)--> (4,1) --R(90)--> (-1,4) --T(3,4)--> (2,8)
    const Matriz3x3 TRS = componer({ traslacion(3, 4), rotacion(90), escalamiento(2, 1) });
    comprobar("Docente: T(3,4)*R(90)*S(2,1) * (2,1)", transformar(TRS, {2, 1}), {2, 8});

    // 2. La matriz compuesta da lo mismo que aplicar una por una
    PuntoF p = {2, 1};
    p = transformar(escalamiento(2, 1), p);
    p = transformar(rotacion(90), p);
    p = transformar(traslacion(3, 4), p);
    comprobar("Paso a paso: S, luego R, luego T", p, {2, 8});

    // 3. Identidad: no cambia nada
    comprobar("Identidad * (-7,5)", transformar(identidad(), {-7, 5}), {-7, 5});

    // 4. Traslacion del auto: T(30, 14) * (0,0)
    comprobar("Traslacion T(30,14) * (0,0)", transformar(traslacion(30, 14), {0, 0}), {30, 14});

    // 5. Rotacion de 90 grados respecto al centro de la rueda C = (102,23)
    //    (106,23) - C = (4,0) -> R(90) -> (0,4) -> + C = (102,27)
    const Matriz3x3 Rc = componer({ traslacion(102, 23), rotacion(90), traslacion(-102, -23) });
    comprobar("Rotacion 90 resp. a (102,23) de (106,23)", transformar(Rc, {106, 23}), {102, 27});
    comprobar("  el centro no se mueve (punto fijo)", transformar(Rc, {102, 23}), {102, 23});

    // 6. Escala 1.5 respecto a la base del arbol B = (110,72)
    //    (110,104) - B = (0,32) -> S(1.5) -> (0,48) -> + B = (110,120)
    const Matriz3x3 Sb = componer({ traslacion(110, 72), escalamiento(1.5, 1.5), traslacion(-110, -72) });
    comprobar("Escala 1.5 resp. a (110,72) de (110,104)", transformar(Sb, {110, 104}), {110, 120});

    // 7. Reflexion respecto a la recta vertical x = 160:  x' = 2*160 - x
    const Matriz3x3 Rk = componer({ traslacion(160, 0), reflexionEjeY(), traslacion(-160, 0) });
    comprobar("Reflexion resp. a x = 160 de (222,72)", transformar(Rk, {222, 72}), {98, 72});
    comprobar("Reflexion eje X de (3,5)", transformar(reflexionEjeX(), {3, 5}), {3, -5});

    // 8. Deformacion en X con shx = 0.3 respecto a la base y = 96
    //    x' = x + 0.3*(y - 96) = 30 + 0.3*78 = 53.4
    const Matriz3x3 Sh = componer({ traslacion(0, 96), deformacionX(0.3), traslacion(0, -96) });
    comprobar("Deformacion X 0.3 resp. a y = 96 de (30,174)", transformar(Sh, {30, 174}), {53.4, 174});
    comprobar("  un punto de la base no se mueve", transformar(Sh, {30, 96}), {30, 96});

    // 9. Deformacion en Y (diapositiva 2.2): y2 = shy*x1 + y1
    comprobar("Deformacion Y 2 de (3,1)", transformar(deformacionY(2), {3, 1}), {3, 7});

    // 10. El orden si importa: T*R != R*T  (Laboratorio 07, actividad 2)
    const PuntoF a = transformar(componer({ traslacion(70, 20), rotacion(90) }), {30, 0});
    const PuntoF b = transformar(componer({ rotacion(90), traslacion(70, 20) }), {30, 0});
    comprobar("T(70,20)*R(90) * (30,0)", a, {70, 50});
    comprobar("R(90)*T(70,20) * (30,0)", b, {-20, 100});

    std::printf("\n%s (%d fallos)\n", fallos == 0 ? "TODAS LAS PRUEBAS CORRECTAS" : "HAY FALLOS", fallos);
    return fallos == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
