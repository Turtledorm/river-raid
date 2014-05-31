#include <stdio.h>  /* printf, puts */
#include <ctype.h>  /* toupper */
#include <stdbool.h>
#include <GL/glut.h>
#include "Grafico.h"
#include "Teclado.h"
#include "Cores.h"
#include "Nave.h"
#include "Defesa.h"
#include "Tiro.h"
#include "Cenario.h"

/*-------------------------*
 |   D E F I N I Ç Õ E S   |
 *-------------------------*/

/* Tempo de espera até criar um inimigo */
#define TEMPO_INIMIGOS 10

static void hud();
static void ground();

/*-------------------*
 |   F U N Ç Õ E S   |
 *-------------------*/

void inicializaGraficos()
{
    
}

/*------------------------------------------------------------------*/

void display()
{
    /* Tempo de espera até gerar um inimigo */
    static int cont = TEMPO_INIMIGOS;

    /* Reconhecimento do teclado */
    keyOperations();
    keySpecialOperations();

    /* Limpa buffer de cores e carrega matriz identidade */
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    /* Coloca câmera atrás da nave.
       (ponto de visão, ponto de fuga, vertical da câmera) */
    gluLookAt(0.0, Y_MAX/2, nave.base.z - DIST_CAMERA,
              0.0, Y_MAX/2, nave.base.z + Z_MAX,
              0.0, 1.0, 0.0);

    /* Desenha elementos */
    hud();
    /* ground(); */

    /* Atualizações visuais e não-visuais */
    atualizaCenario();
    imprimeElementos(); 
    if (cont-- == 0) {
        geraInimigo();
        cont = TEMPO_INIMIGOS;
    }
    if (nave.vidas <= 0) {
        liberaCenario();
        exit(EXIT_SUCCESS);
    }

    glutSwapBuffers();
}

/*------------------------------------------------------------------*/

void reshape(GLint width, GLint height)
{ 
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();
    glScaled(-1.0, 1.0, 1.0); /* espelha o eixo x */
    gluPerspective(60, (GLdouble) width/height, 1.0, Z_MAX);
    glMatrixMode(GL_MODELVIEW);
}

/*------------------------------------------------------------------*
 *
 *  Faz a representação visual dos atributos da nave pertinentes à
 *  jogabilidade: vidas restantes, hp e score.
 *
 */
static void hud()
{
    const double RAIO = 5.0;

    int i, tam;
    char score[16];

    glPushMatrix();
    glTranslated(-GLUT_WINDOW_WIDTH, GLUT_WINDOW_HEIGHT, nave.base.z);

    /* Desenha vidas restantes da nave */
    for (i = 0; i < nave.vidas; i++) {
        glPointSize(50.0);  
        glBegin(GL_TRIANGLE_FAN);
            glColor(CYAN);
            glVertex3d( 0.0  + 3*RAIO*i,   0.0, 0.0);
            glColor(WHITE);
            glVertex3d( 0.0  + 3*RAIO*i,  RAIO, 0.0);
            glColor(WHITE);
            glVertex3d( RAIO + 3*RAIO*i,   0.0, 0.0);
            glColor(CYAN);
            glVertex3d( 0.0  + 3*RAIO*i, -RAIO, 0.0);
            glColor(CYAN);
            glVertex3d(-RAIO + 3*RAIO*i,   0.0, 0.0);
            glColor(WHITE);
            glVertex3d( 0.0  + 3*RAIO*i,  RAIO, 0.0);
        glEnd();
    }

    /* Desenha a caixa da lifebar da nave */
    glColor(DARK_BLUE); /*glColor3ub(0, 0, 60); */
    glBegin(GL_QUADS); {
        glVertex3d(-1.0, -2*RAIO + 1, 0.0);
        glVertex3d(NAVE_HPMAX + 1, -2*RAIO + 1, 0.0);
        glVertex3d(NAVE_HPMAX + 1, -2*RAIO - 2, 0.0);
        glVertex3d(-1.0, -2*RAIO - 2, 0.0);
    } glEnd();

    /* Desenha a lifebar, que varia de verde a vermelho, dependendo do hp da nave */
    glColor(1 - 255*nave.base.hp/NAVE_HPMAX, 255*nave.base.hp/NAVE_HPMAX, 0);
    glBegin(GL_QUADS); {
        glVertex3d(0.0, -2*RAIO, 0.0);
        glVertex3d(NAVE_HPMAX*nave.base.hp/100.0, -2*RAIO, 0.0);
        glVertex3d(NAVE_HPMAX*nave.base.hp/100.0, -2*RAIO - 1, 0.0);
        glVertex3d(0.0, -2*RAIO - 1, 0.0);
    } glEnd();

    /* Imprime score */
    tam = sprintf(score, "Score: %d", nave.score);
    glColor(WHITE);
    glRasterPos3d(0.0, -2*RAIO - 10, 0.0);
    for (i = 0; i < tam; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, score[i]);
    }

    glPopMatrix();
}

/*------------------------------------------------------------------*
 *
 *  Desenha a representação visual do chão, ou seja, o plano que
 *  representa o menor valor que y pode assumir no jogo.
 *
 */
static void ground() {
    glPushMatrix();
    glTranslated(0.0, -Y_MAX/2, nave.base.z - DIST_CAMERA);

    /* Desenha o chão com alguns tons diferentes de azul */
    glBegin(GL_QUADS); {
        glColor(LIGHT_BLUE);
        glVertex3d(2 * -X_MAX, 0.0, 0.0);
        glColor(POWDER_BLUE);
        glVertex3d(2 * X_MAX, 0.0, 0.0);
        glColor(SKY_BLUE);
        glVertex3d(2 * X_MAX, 0.0, Z_MAX + DIST_CAMERA);
        glColor(LIGHT_SKY_BLUE);
        glVertex3d(2 * -X_MAX, 0.0, Z_MAX + DIST_CAMERA);
    } glEnd();

    glPopMatrix();
}
