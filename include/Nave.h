/*------------------------------------------------------------------*
 |                                                                  |
 |  Nave que será usada primariamente pelo jogador principal.       |
 |                                                                  |
 *------------------------------------------------------------------*/

#pragma once

#include <stdbool.h>

#include "Defesa.h"
#include "Item.h"
#include "Base.h"

/*-------------------------*
 |   D E F I N I Ç Õ E S   |
 *-------------------------*/

/* Atributos iniciais e/ou fixos */
#define VIDAS_INI     3
#define NAVE_VEL      4
#define NAVE_HPMAX  100
#define NAVE_COOL     6
#define NAVE_RAIO    40
#define NAVE_ALTURA  20

/* Ângulo de inclinação máxima */
#define ANG_MAX PI/6

/* Taxas de alteração da direção a cada timestep */
#define ANG_MANUAL ANG_MAX/20  /* por comando do usuário */
#define ANG_AUTO   ANG_MAX/60  /* automático */

/* Nº de timesteps de invencibilidade */
#define INVENCIVEL_DANO 30  /* ao ser atingida */
#define INVENCIVEL_VIDA 90  /* ao perder uma vida */

/* Energia perdida caso ocorra colisão com inimigos */
#define DANO_COLISAO 20

/* Arquivo e número de vértices referentes ao modelo da nave */
#define NAVE_MODELO_VERTICES "Nave.vert"
#define NAVE_NUM_VERTICES 696

/*
 *  Representa a nave do jogador.
 */
typedef struct nave Nave;
struct nave
{
    Corpo base;

    /* Número de chances do jogador */
    int vidas;

    /* Componentes da velocidade da nave */
    double vx, vy, vz;

    /* Inclinações em relação ao eixo Oz */
    double angHoriz, angVert;

    /* Pontuação do jogador */
    unsigned int score;

    /* Enquanto > 0, nave é imune a tiros e colisões */
    unsigned int invencibilidade;

    /* Serve como uma 2ª barra de vida para a nave, até ser destruída */
    unsigned int escudo;

    /* Define invencibilidade */
    bool godMode;
};

extern Nave nave;

/*-------------------------*
 |   P R O T Ó T I P O S   |
 *-------------------------*/

/*
 *  Carrega modelo gráfico da nave para a memória.
 */
void carregaModeloNave();

/*
 *  Recebe a posição no eixo Oz da nave e um número de vidas.
 *  Inicializa os atributos da nave.
 *  Se godMode == true, então nave será invencível.
 */
void criaNave(int z, int nVidas, bool godMode);

/*
 *  Atualiza a posição da nave em relação ao timestep anterior.
 *  As componentes horizontal e vertical são proporcionais
 *  à velocidade escalar da nave ao longo do eixo Oz.
 */
void moveNave();

/*
 *  Dispara um projétil na direção em que a nave estiver
 *  apontando e atualiza o tempo de recarga.
 */
void naveDispara();

/*
 *  Recebe uma quantidade de dano, remove a energia em questão
 *  da nave e ativa o período de invencibilidade. Caso (hp <= 0),
 *  jogador perde uma vida.
 */
void danificaNave(int dano);

/*
 *  Verifica se nave colidiu com determinado inimigo,
 *  ou seja, se seus corpos entraram em contato.
 */
bool naveColidiu(Inimigo *foe);
bool naveTocaItem(Item *item);

/*
 *  Desenha a representação visual da nave na tela.
 */
void desenhaNave(); 
