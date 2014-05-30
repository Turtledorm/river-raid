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

/* Tempo de espera para criar um inimigo */
#define TEMPO_INIMIGOS 10

/* Dimensões da janela */
#define JANELA_LARGURA 2 * 5 * X_MAX
#define JANELA_ALTURA      5 * Y_MAX

static void hud();
static void ground();

static void imprimeElementos();

/*-------------------*
 |   F U N Ç Õ E S   |
 *-------------------*/

void inicializaGraficos()
{
    /* Inicializa flags */    
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE);

    /* Desenha e posiciona janela de jogo */
    glutInitWindowSize(JANELA_LARGURA, JANELA_ALTURA);
    glutInitWindowPosition(
        (glutGet(GLUT_SCREEN_WIDTH)  - JANELA_LARGURA)/2,
        (glutGet(GLUT_SCREEN_HEIGHT) - JANELA_ALTURA)/2);
    glutCreateWindow("River Raid");    

    /* ---- Funções de renderização ---- */

    glutDisplayFunc(display);
    glutIdleFunc(display);

    glutReshapeFunc(reshape);

    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyUp);

    glutSpecialFunc(keySpecialPressed);
    glutSpecialUpFunc(keySpecialUp);

    /* ---- Funções de renderização ---- */

    /* Loop principal que cuidará do resto do jogo */
    glutMainLoop();
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
            Cor(CYAN);
            glVertex3d( 0.0  + 3*RAIO*i,   0.0, 0.0);
            Cor(WHITE);
            glVertex3d( 0.0  + 3*RAIO*i,  RAIO, 0.0);
            Cor(WHITE);
            glVertex3d( RAIO + 3*RAIO*i,   0.0, 0.0);
            Cor(CYAN);
            glVertex3d( 0.0  + 3*RAIO*i, -RAIO, 0.0);
            Cor(CYAN);
            glVertex3d(-RAIO + 3*RAIO*i,   0.0, 0.0);
            Cor(WHITE);
            glVertex3d( 0.0  + 3*RAIO*i,  RAIO, 0.0);
        glEnd();
    }

    /* Desenha a caixa da lifebar da nave */
    Cor(DARKBLUE); /*glColor3ub(0, 0, 60); */
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
    Cor(WHITE);
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
        glColor(LIGHTBLUE);
        glVertex3d(2 * -X_MAX, 0.0, 0.0);
        glColor(POWDERBLUE);
        glVertex3d(2 * X_MAX, 0.0, 0.0);
        glColor(SKYBLUE);
        glVertex3d(2 * X_MAX, 0.0, Z_MAX + DIST_CAMERA);
        glColor(LIGHTSKYBLUE);
        glVertex3d(2 * -X_MAX, 0.0, Z_MAX + DIST_CAMERA);
    } glEnd();

    glPopMatrix();
}

/*------------------------------------------------------------------*
 *
 *  Mostra informação a respeito de todos os elementos do jogo neste
 *  timestep. Para efeitos de clareza, todas as componentes Z, exceto
 *  a da nave, são relativas à nave em si (e não absolutas).
 *
 */
static void imprimeElementos()
{
    Celula *p;

    /* Limpa a tela do terminal/prompt */
    #ifdef __linux__
            system("clear");
    #elif _WIN32
            system("cls");
    #endif

    puts("{Nave}");
    printf("PONTUAÇÂO: %d\n", nave.score);
    printf("VIDAS: %d\n", nave.vidas);
    printf("Energia: %-3d/%d\n", nave.base.hp, NAVE_HPMAX);
    printf("Posição: (%.0f, %.0f, %.0f)\n", 
        nave.base.x, nave.base.y, nave.base.z);
    printf("Ângulos: (%.0f°, %.0f°)\n",
        (180/PI) * nave.angHoriz, (180/PI) * nave.angVert);
    
    puts("\n{Inimigos}");
    puts("   ( x, y, z)       Recarga    Precisão    Energia ");
    puts("----------------    -------    --------   ---------");
    for (p = inimigos; p->prox != NULL; p = p->prox) {
        Inimigo *foe = p->prox->item;
        printf(" (%3g, %3g, %3g)      %2d/%2d       %3.0f%%       %2d/%2d\n",
            foe->base.x, foe->base.y, (foe->base.z - nave.base.z),
            foe->base.espera, foe->base.cooldown, 100 * foe->precisao,
            foe->base.hp, FOE_HPMAX);
    }
    puts("\n{Projéteis}");
    puts("   ( x, y, z)          [ vx, vy, vz]        Amigo? ");
    puts("----------------    --------------------   --------");
    for (p = projeteis; p->prox != NULL; p = p->prox) {
        Projetil *bullet = p->prox->item;
        printf(" (%3.0f, %3.0f, %3.0f)      [%4.1f, %4.1f, %4.1f]       %s\n",
            bullet->x, bullet->y, (bullet->z - nave.base.z),
            bullet->vx, bullet->vy, bullet->vz,
            (bullet->amigo) ? "sim" : "não");
    }
}
