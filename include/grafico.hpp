/*------------------------------------------------------------------*
 |                                                                  |
 |  Funções relacionadas à parte gráfica (OpenGL).                  |
 |  Cuida também do desenho de elementos fixos do cenário.          |
 |                                                                  |
 *------------------------------------------------------------------*/

#pragma once

#include <GL/freeglut.h>

/*------------------------------------------------------------------*/

/* Tipo de luz a ser usada */
#define LUZ_AMBIENTE GL_LIGHT0

/* Dimensões da janela (widescreen por padrão) */
#define JANELA_LARGURA 3 * Cenario::X_MAX
#define JANELA_ALTURA  JANELA_LARGURA/16.0 * 9

/* Número de quadros desenhados por segundo */
#define FPS 60

/* Distância da câmera à nave */
#define DIST_CAMERA 250

/* Constantes para os poliedros */
#define SLICES 16
#define STACKS 10

/*------------------------------------------------------------------*/

/*
 *  Inicializa elementos do jogo e passa o comando do restante
 *  do programa ao OpenGL.
 */
void inicializaJogo(int argc, char *argv[]);
