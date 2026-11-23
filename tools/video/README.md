# Video del dibujado de la escena

Genera un MP4 que muestra cómo se construye la escena ficha por ficha, usando el
código real del proyecto. No modifica nada de `src/`.

## Qué es real y qué es stub
- Real: `scene/scene.cpp`, `scene/objects/*.cpp`, `algorithms/*.cpp` (se llama a `construirEscena()` y a `renderizarEscena()`).
- Stub: `framebuffer_stub.cpp` reemplaza a `core/framebuffer.cpp` (misma firma y misma lógica de recorte, buffer en memoria, sin GLFW/OpenGL). Además puede registrar los píxeles que recibe `ponerPixel`, en su orden real.
- El fotograma *k* se obtiene dejando en `g_escena.objetos` solo las fichas 0..k y llamando a la `renderizarEscena()` real.

## Regenerar
Requisitos (WSL/Linux): `g++` (C++17) y `ffmpeg`. Fuente con acentos: DejaVu Sans (`fonts-dejavu-core`).

```bash
tools/video/build.sh
# opcional: FFMPEG=/ruta/ffmpeg FONT=/ruta/fuente.ttf tools/video/build.sh
```
Si WSL no tiene `ffmpeg`, el script usa el `ffmpeg.exe` de Windows (WinGet) cuando lo encuentra.

## Salida (`tools/video/out/`, ignorada por git)
- `simulador_urbano_fichas.mp4`: 1 ficha = 1 fotograma, con resalte parpadeante y pausa con rótulo al final de cada capítulo.
- `simulador_urbano_pixeles.mp4`: versión que anima la rasterización píxel a píxel (orden real de `ponerPixel`).
- `frames/`: PNG de cada ficha (`ficha_NNNN.png` con resalte, `ficha_NNNN_sin.png` sin resalte) y `capitulo_NN.png`. `frames_px/`: PNG del video de píxeles.
- `resumen.txt` (fichas por grupo), `verificacion.txt`, `fichas.tsv` (nombre, tipo, función, nº de píxeles), `timeline*.txt` (duraciones para ffmpeg).

## Formato
800×688: escena de 800×600 (rejilla 320×240 ×2.5, vecino más cercano, sin suavizado) más una barra inferior de 88 px con nombre, tipo, algoritmo, función `agregar…`, capítulo y contador "ficha k / total". 30 fps, H.264, `yuv420p`, `-preset veryfast`.

La función `agregar…` de cada ficha se deduce del nombre de la ficha, porque esas funciones están en un namespace anónimo. El script comprueba que los grupos así deducidos coinciden con los conteos de las funciones públicas de `objects.h`.

## Verificación
`build.sh` compila `verify_real_fb.cpp`, que incluye el `framebuffer.cpp` real (con un `<GLFW/glfw3.h>` falso que solo declara las funciones GL) y compara:
1. El buffer float 320×240 de `renderizarEscena()` real contra el del último fotograma (comparación bit a bit).
2. La zona de escena 800×600 del último PNG contra ese buffer real escalado 2.5×.
