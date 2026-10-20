#pragma once

// Ventana fisica (pixeles reales de la pantalla)
const int ANCHO_VENTANA = 800;
const int ALTO_VENTANA  = 600;

// Rejilla logica (el "lienzo" donde dibujan todos los algoritmos)
const int ANCHO_REJILLA = 320;
const int ALTO_REJILLA  = 240;

// ------------------------------------------------------------
// Alturas (eje Y) de las franjas de la escena. Y hacia ARRIBA.
//   96: base de los edificios | 87: pasto de atras
//   75: acera de atras        | 48: carretera
//   26: acera de frente       |  0: pasto de frente
// ------------------------------------------------------------

// Alturas (eje Y) de las franjas de la escena. Y hacia ARRIBA.
//   96 a 68: pasto grande (arboles, arbustos, casa)
//   68 a 44: carretera
//   44 a 20: vereda (una sola, como en la referencia)
//   20 a  0: pasto de frente
const int Y_BASE_EDIFICIOS = 96;
const int Y_CARRETERA_SUP  = 68;
const int Y_CARRETERA_INF  = 44;
const int Y_VEREDA_INF     = 20;
