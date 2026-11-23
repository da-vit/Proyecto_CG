#pragma once
// ============================================================
// config.h - Constantes generales del proyecto
//
// Sistema de coordenadas de TODO el proyecto:
//   - Origen (0,0) en la esquina INFERIOR izquierda.
//   - x crece hacia la derecha, y crece hacia ARRIBA.
// ============================================================

// Ventana fisica del sistema operativo (pixeles reales de la pantalla)
const int ANCHO_VENTANA = 800;
const int ALTO_VENTANA  = 600;

// Rejilla logica: el "lienzo" de 320 x 240 celdas donde dibujan todos
// nuestros algoritmos. Luego se amplia para llenar la ventana fisica.
const int ANCHO_REJILLA = 320;
const int ALTO_REJILLA  = 240;

// ------------------------------------------------------------
// Alturas (eje Y) de las franjas de la escena:
//
//   240 +------------------------------+
//       |  cielo, sol, nubes, edificios|
//    96 +-- base de los edificios -----+
//       |  pasto: casa, arboles,       |
//       |  semaforo, senal, postes     |
//    68 +-- borde superior de la pista +
//       |  carretera y vehiculos       |
//     0 +------------------------------+
// ------------------------------------------------------------
const int Y_BASE_EDIFICIOS = 96;
const int Y_CARRETERA_SUP  = 68;
const int Y_CARRETERA_INF  = 0;

// Altura donde se apoyan los objetos del pasto (casa, arboles,
// semaforo, senal de PARE y postes de luz)
const int Y_SUELO = Y_CARRETERA_SUP + 4;   // 72
