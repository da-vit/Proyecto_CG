#pragma once
// ============================================================
// matriz.h - Transformaciones geometricas 2D con MATRICES HOMOGENEAS
// David Araoz: modulo base de matrices
//
// Convencion (la del docente, Laboratorio 07):
//   - Los puntos son VECTORES COLUMNA en coordenadas homogeneas:
//         p = | x |        p' = M * p
//             | y |
//             | 1 |
//   - Angulo positivo = giro ANTIHORARIO (x a la derecha, y hacia arriba).
//   - En una composicion, la matriz de la DERECHA se aplica PRIMERO:
//         M = T * R * S   ->  primero escala, luego rota, al final traslada.
// ============================================================
#include <initializer_list>
#include <string>

struct PuntoF { double x, y; };            // punto con decimales

struct Matriz3x3 { double m[3][3]; };      // m[fila][columna]

// ---------- Matrices basicas ----------
Matriz3x3 identidad();
Matriz3x3 traslacion(double tx, double ty);           // T(tx, ty)
Matriz3x3 rotacion(double grados);                    // R(theta), respecto al origen
Matriz3x3 escalamiento(double sx, double sy);         // S(sx, sy), respecto al origen
Matriz3x3 reflexionEjeX();                            // S(1, -1): (x, y) -> (x, -y)
Matriz3x3 reflexionEjeY();                            // S(-1, 1): (x, y) -> (-x, y)
Matriz3x3 deformacionX(double shx);                   // x' = x + shx*y ,  y' = y
Matriz3x3 deformacionY(double shy);                   // x' = x ,  y' = shy*x + y

// ---------- Operaciones ----------
Matriz3x3 multiplicar(const Matriz3x3& A, const Matriz3x3& B);   // A * B

// componer({A, B, C}) = A * B * C   (se escribe igual que en la formula;
// recordar que C es la que actua primero sobre el punto)
Matriz3x3 componer(std::initializer_list<Matriz3x3> matrices);

PuntoF transformar(const Matriz3x3& M, PuntoF p);     // p' = M * p

// Muestra la matriz en consola (evidencia para el informe)
void imprimirMatriz(const std::string& titulo, const Matriz3x3& M);
