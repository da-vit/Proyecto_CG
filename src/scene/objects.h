#pragma once
// ============================================================
// objects.h - Funciones que agregan los objetos a la escena
//
// Cada funcion esta en su propio archivo de scene/objects/ y
// agrega piezas (lineas, circulos, poligonos) a g_escena.
// Se llaman en este orden desde construirEscena() (scene.cpp):
// primero lo del fondo, al final lo del frente.
//
// Segunda entrega: entre corchetes, la transformacion que tiene
// cada archivo y quien la hizo.
// ============================================================

void dibujarCielo();          // sky.cpp         - David Araoz:    cielo, ciudad lejana, sol y nubes   [traslacion de nubes - David]
void dibujarEdificios();      // buildings.cpp   - Coral Cuadros:  edificios y letreros [distorsion - Coral]
void dibujarCarretera();      // road.cpp        - Keidy Cochama:  pasto, pista, cruce peatonal
void dibujarCasaYArboles();   // house_trees.cpp - Samuel Sanchez: casa, arboles y arbustos [escala - Samuel]
void dibujarSenales();        // signs.cpp       - David Araoz:    semaforo, PARE y postes [composicion - Keidy]
void dibujarVehiculos();      // vehicles.cpp    - Lurancy Chipa:  auto y camion [traslacion y rotacion - Lurancy,
                              //                                  reflexion - Samuel]
