#pragma once
// ============================================================
// animacion.h - Estado de la animacion (segunda entrega)
// David Araoz: bucle de animacion y control por teclado
//
// Animacion DEPENDIENTE DEL TIEMPO (diapositiva 2.10): cada objeto
// tiene una velocidad, y en cada frame se avanza segun el tiempo
// transcurrido (dt, en segundos). Asi la velocidad no depende de
// que tan rapida sea la computadora.
//
// Cada transformacion se puede activar/desactivar con su tecla:
//   1 Traslacion | 2 Rotacion | 3 Escala | 4 Reflexion
//   5 Distorsion | 6 Composicion
// ============================================================

enum Transformacion {
    TRASLACION = 1, ROTACION, ESCALA, REFLEXION, DISTORSION, COMPOSICION
};

struct EstadoAnimacion {
    bool pausa = false;                      // ESPACIO
    bool activa[7] = { false, true, true, true, true, true, true };   // [1..6]
    bool imprimirMatrices = false;           // tecla M: se imprimen en el siguiente frame

    // Distancia recorrida por cada vehiculo (traslacion)
    double distanciaAuto = 0, distanciaCamion = 0, distanciaReflejado = 0;
    // Angulo de las ruedas en grados (rotacion)
    double anguloRuedasAuto = 0, anguloRuedasCamion = 0, anguloRuedasReflejado = 0;
    // Relojes propios de cada efecto (avanzan solo si el efecto esta activo)
    double relojNubes = 0, relojEscala = 0, relojDistorsion = 0, relojComposicion = 0;
};

extern EstadoAnimacion g_animacion;

void        actualizarAnimacion(double dt);        // avanza la animacion dt segundos
void        alternarTransformacion(int numero);    // teclas 1..6
const char* nombreTransformacion(int numero);
void        mostrarEstadoAnimacion();              // imprime que esta activo

// ------------------------------------------------------------
// Valores actuales que usan los objetos para armar sus matrices
// ------------------------------------------------------------
double xAuto();                  // traslacion: posicion x del auto rojo
double xCamion();                // traslacion: posicion x del camion
double xAutoReflejado();         // traslacion del auto reflejado (va a la izquierda)
double desplazamientoNube(int i);// traslacion de la nube i (va y vuelve, sin salir)
double escalaArbol();            // escala: factor del arbol que crece
double deformacionEdificio();    // distorsion: factor shx del edificio
double anguloSenal();            // composicion: angulo de la senal PARE
double escalaSenal();            // composicion: escala de la senal PARE
