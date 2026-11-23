// ============================================================
// matriz.cpp - Implementacion de las matrices homogeneas 2D
// David Araoz: modulo base de matrices
// ============================================================
#include "matriz.h"
#include <cmath>
#include <cstdio>

Matriz3x3 identidad()
{
    return { { {1, 0, 0},
               {0, 1, 0},
               {0, 0, 1} } };
}

//            | 1  0  tx |
// T(tx,ty) = | 0  1  ty |
//            | 0  0  1  |
Matriz3x3 traslacion(double tx, double ty)
{
    return { { {1, 0, tx},
               {0, 1, ty},
               {0, 0, 1 } } };
}

//          | cos  -sen  0 |
// R(th) =  | sen   cos  0 |     (th se pasa de grados a radianes)
//          |  0     0   1 |
Matriz3x3 rotacion(double grados)
{
    const double pi = 3.14159265358979323846;
    const double a  = grados * pi / 180.0;
    const double c  = std::cos(a);
    const double s  = std::sin(a);
    return { { {c, -s, 0},
               {s,  c, 0},
               {0,  0, 1} } };
}

//            | sx  0   0 |
// S(sx,sy) = | 0   sy  0 |
//            | 0   0   1 |
Matriz3x3 escalamiento(double sx, double sy)
{
    return { { {sx, 0,  0},
               {0,  sy, 0},
               {0,  0,  1} } };
}

// La reflexion es una escala con un factor -1
Matriz3x3 reflexionEjeX() { return escalamiento( 1, -1); }
Matriz3x3 reflexionEjeY() { return escalamiento(-1,  1); }

//          | 1  shx  0 |
// Sh_x =   | 0   1   0 |     x2 = x1 + shx*y1 ,  y2 = y1   (diapositiva 2.2)
//          | 0   0   1 |
Matriz3x3 deformacionX(double shx)
{
    return { { {1, shx, 0},
               {0, 1,   0},
               {0, 0,   1} } };
}

//          |  1   0  0 |
// Sh_y =   | shy  1  0 |     x2 = x1 ,  y2 = shy*x1 + y1   (diapositiva 2.2)
//          |  0   0  1 |
Matriz3x3 deformacionY(double shy)
{
    return { { {1,   0, 0},
               {shy, 1, 0},
               {0,   0, 1} } };
}

// (A*B)[i][j] = suma de A[i][k] * B[k][j]
Matriz3x3 multiplicar(const Matriz3x3& A, const Matriz3x3& B)
{
    Matriz3x3 C{};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            C.m[i][j] = 0;
            for (int k = 0; k < 3; k++)
                C.m[i][j] += A.m[i][k] * B.m[k][j];
        }
    return C;
}

Matriz3x3 componer(std::initializer_list<Matriz3x3> matrices)
{
    Matriz3x3 resultado = identidad();
    for (const Matriz3x3& M : matrices)
        resultado = multiplicar(resultado, M);   // de izquierda a derecha, como se escribe
    return resultado;
}

// | x' |   | m00 m01 m02 |   | x |
// | y' | = | m10 m11 m12 | * | y |
// | 1  |   |  0   0   1  |   | 1 |
PuntoF transformar(const Matriz3x3& M, PuntoF p)
{
    return { M.m[0][0] * p.x + M.m[0][1] * p.y + M.m[0][2],
             M.m[1][0] * p.x + M.m[1][1] * p.y + M.m[1][2] };
}

void imprimirMatriz(const std::string& titulo, const Matriz3x3& M)
{
    std::printf("%s\n", titulo.c_str());
    for (int i = 0; i < 3; i++)
        std::printf("    | %9.4f %9.4f %9.4f |\n", M.m[i][0], M.m[i][1], M.m[i][2]);
}
