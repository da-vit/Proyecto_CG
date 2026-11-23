#!/usr/bin/env bash
# Compila el generador headless, genera los fotogramas, verifica y codifica los MP4.
# Uso: tools/video/build.sh            (desde cualquier carpeta)
# Variables opcionales: FFMPEG=<ruta a ffmpeg>  FONT=<ruta a un .ttf con acentos>
set -euo pipefail

AQUI="$(cd "$(dirname "$0")" && pwd)"
RAIZ="$(cd "$AQUI/../.." && pwd)"
OUT="$AQUI/out"
FONT="${FONT:-/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf}"
FFMPEG="${FFMPEG:-$(command -v ffmpeg || ls /mnt/c/Users/*/AppData/Local/Microsoft/WinGet/Packages/Gyan.FFmpeg*/ffmpeg*/bin/ffmpeg.exe 2>/dev/null | head -1)}"

INC=(-I"$RAIZ/src" -I"$RAIZ/src/core" -I"$RAIZ/src/algorithms" -I"$RAIZ/src/scene" -I"$AQUI" -I"$AQUI/third_party")
FUENTES_REALES=("$RAIZ"/src/scene/scene.cpp "$RAIZ"/src/scene/objects/*.cpp "$RAIZ"/src/algorithms/*.cpp)

mkdir -p "$OUT"
rm -rf "$OUT/frames" "$OUT/frames_px"

echo "== Compilando gen (codigo real + framebuffer_stub.cpp) =="
g++ -std=c++17 -O2 "${INC[@]}" "$AQUI/gen.cpp" "$AQUI/framebuffer_stub.cpp" "${FUENTES_REALES[@]}" -o "$OUT/gen"

echo "== Generando fotogramas =="
"$OUT/gen" "$OUT" "$FONT" | tee "$OUT/resumen.txt"

echo "== Compilando verificacion (framebuffer.cpp REAL) =="
g++ -std=c++17 -O2 -I"$AQUI/fake_include" "${INC[@]}" "$AQUI/verify_real_fb.cpp" "${FUENTES_REALES[@]}" -o "$OUT/verify_real_fb"
"$OUT/verify_real_fb" "$OUT/final_acumulado.f32" "$OUT/frames/ficha_0321_sin.png" | tee "$OUT/verificacion.txt"

if [ -z "$FFMPEG" ]; then echo "ffmpeg no encontrado: omito la codificacion (defina FFMPEG)"; exit 0; fi
echo "== Codificando MP4 con $FFMPEG =="
cd "$OUT"
for par in "timeline.txt:simulador_urbano_fichas.mp4" "timeline_px.txt:simulador_urbano_pixeles.mp4"; do
    "$FFMPEG" -y -loglevel error -f concat -safe 0 -i "${par%%:*}" \
        -vf "fps=30,format=yuv420p" -c:v libx264 -preset veryfast -crf 18 -movflags +faststart "${par##*:}"
    echo "  -> ${par##*:}"
done
