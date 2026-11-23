// Cabecera falsa SOLO para verify_real_fb.cpp: declara las funciones GL que usa
// mostrarBuffer() de src/core/framebuffer.cpp, sin hacer nada.
#pragma once
typedef int GLint;
typedef unsigned int GLenum;
#define GL_VIEWPORT 0x0BA2
#define GL_RGB      0x1907
#define GL_FLOAT    0x1406
inline void glGetIntegerv(GLenum, GLint* p) { p[0] = 0; p[1] = 0; p[2] = 800; p[3] = 600; }
inline void glRasterPos2f(float, float) {}
inline void glPixelZoom(float, float) {}
inline void glDrawPixels(int, int, GLenum, GLenum, const void*) {}
