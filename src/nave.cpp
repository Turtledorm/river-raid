#include <cmath>      /* tan, abs */
#include <algorithm>  /* min */
#include <GL/freeglut.h>

#include "nave.hpp"
#include "tiro.hpp"
#include "item.hpp"
#include "cenario.hpp"
#include "teclado.hpp"
#include "modelo.hpp"
#include "grafico.hpp"
#include "cores.hpp"

/*------------------------------------------------------------------*/

/* Modelo da nave */
static Modelo modelo;

/* Booleano que define invencibilidade perpétua */
static bool godMode;

/*-------------*
 |   N A V E   |
 *-------------*----------------------------------------------------*/

/* Provisório? */
Nave Nave::nave(false);

Nave &Nave::get()
{
    return nave;
}

/*------------------------------------------------------------------*/

Nave::Nave(bool _godMode) : Unidade(0.0)
{
    /* Carrega modelo da nave */
    leVertices("nave.vert", modelo);
    carregaTextura("silver.ppm", false, modelo);

    raio     = 20.0;
    altura   = 40.0;
    cooldown = 8;

    /* Define variável do módulo */
    godMode = _godMode;

    /* Começa em z == 0.0 com 3 vidas */
    recria(0.0, 3);

    /* Guarda nave no ponteiro do módulo */
    nave = *this;
}

/*
 *  Recebe a posição no eixo Oz da nave e um número de vidas.
 *  Reinicializa os atributos variáveis da nave.
 */
void Nave::recria(int z, int nVidas)
{
    /* Coordenadas iniciais */
    x = 0.0;
    y = Y_MAX/2;
    this->z = z;

    /* Aponta para o centro */
    angHoriz = 0.0;
    angVert  = 0.0;

    vidas           = nVidas;
    hpMax = hp      = 100;
    espera          = 0;
    invencibilidade = INVENCIVEL_VIDA;
    escudo          = 0;
}

/*------------------------------------------------------------------*/

void Nave::move()
{
    /* Obtém vetores componentes */
    vx = vz * tan(angHoriz);
    vy = vz * tan(angVert);

    /* Atualiza posição por vetores */
    x += vx;
    y += vy;
    z += vz;

    /* Impede que nave ultrapasse os limites do cenário */
    if      (x >  X_MAX - raio  ) x =  X_MAX - raio;
    else if (x < -X_MAX + raio  ) x = -X_MAX + raio;
    if      (y >  Y_MAX - altura) y =  Y_MAX - altura;
    else if (y <      0 + altura) y =      0 + altura;
}

void Nave::atualizaHorizontal(int sentido)
{
    atualizaDirecao(angHoriz, sentido);
}

void Nave::atualizaVertical(int sentido)
{
    atualizaDirecao(angVert, sentido);
}

/*
 *  Recebe um ângulo de inclinação da nave e atualiza seu valor
 *  em d graus, respeitando o limite de ANG_MAX. Caso chegue a 0°, direção é mantida.
 */
void Nave::atualizaDirecao(double &ang, int sentido)
{
    /* Taxas de alteração de ângulo por timestep */
    static constexpr double ANG_MANUAL = ANG_MAX/20;  /* pelo usuário */
    static constexpr double ANG_AUTO   = ANG_MAX/60;  /* automática */

    /* Atualiza ângulo, evitando ultrapassar o limite */
    ang += ANG_MANUAL * sentido;
    if (abs(ang) > ANG_MAX) ang = ANG_MAX * sentido;

    /* Tende a voltar para o centro */
    if      (ang > 0.0) ang -= ANG_AUTO;
    else if (ang < 0.0) ang += ANG_AUTO;
}

/*------------------------------------------------------------------*/

void Nave::atualizaInvencibilidade()
{
    if (invencibilidade > 0) invencibilidade--;
}

/*------------------------------------------------------------------*/

void Nave::dispara()
{
    bool amigo = true;

    /* Módulo do vetor de velocidade */
    double modulo = norma(vx, vy, vz);

    /* Componentes da velocidade da bala são proporcionais à nave */
    double k = Projetil::VEL_PADRAO/modulo;
    double vx = k * this->vx;
    double vy = k * this->vy;
    double vz = k * this->vz;

    /* Cria projétil e o insere na lista */
    Projetil bullet(*this, vx, vy, vz, amigo);
    Projetil::lista.insere(bullet);

    /* Reinicia contagem até próximo tiro */
    espera = cooldown;
}

/*------------------------------------------------------------------*/

void Nave::danifica(int dano)
{
    static constexpr int INVENCIVEL_DANO = 30;

    /* Se invencível neste instante, não toma dano */
    if (invencibilidade > 0 or godMode) return;

    if (escudo > 0) {
        hp     -=     dano/3;
        escudo -= 2 * dano/3;
    }
    else {
        hp -= dano;
    }
    /* Nave fica invencível por um tempo pós-dano */
    invencibilidade = INVENCIVEL_DANO;

    /* Verifica se nave perdeu vida */
    if (hp <= 0) {
        hp = 0;
        if (--vidas >= 0) recria(z, vidas);
    }
}

/*------------------------------------------------------------------*/

void Nave::ativaItem(Item &item)
{
    switch (item.getTipo()) {
    case TipoItem::HP:
        hp = std::min(hp + getHPMax()/4, getHPMax());
        break;
    case TipoItem::VIDA:
        vidas++;
        break;
    case TipoItem::ESCUDO:
        escudo = 2 * getHPMax();
        break;
    }
}

/*------------------------------------------------------------------*/
 
void Nave::desenha()
{
    GLdouble naveCor = 255 - 190.0/INVENCIVEL_VIDA * invencibilidade;

    glDisable(GL_TEXTURE_2D);

    /* Se ativo, desenha escudo ao redor da nave */
    if (escudo > 0) {
        static double rotacao = 0;
        rotacao += M_PI/6;
        naveCor = 255;
        glPushMatrix();
        glTranslated(x, y, z);
        glRotated(rotacao, 1.0, 1.0, 0.0);
        setColorAlpha(DARK_BLUE, 255 * escudo/(2.0 * getHPMax()));
        glutWireSphere(1.75 * raio, SLICES, STACKS);
        glPopMatrix();
    }
    glPushMatrix();

    /* Posiciona nave rotacionada de acordo com ângulos de inclinação */
    glTranslated(x, y, z);
    glRotated( angHoriz * 180.0/M_PI, 0.0, 1.0, 0.0);
    glRotated(-angVert  * 180.0/M_PI, 1.0, 0.0, 0.0);

    setColorAlpha(3 * naveCor, 3 * naveCor, 0, 3 * naveCor);
    if (estaEmPrimeiraPessoa()) {
        /* No modo 1º pessoa, tela fica vermelha após dano. */
        GLfloat k = 1.5 * invencibilidade/INVENCIVEL_VIDA;
        GLfloat luzTela[3] = {1.0f, 1.0f - k, 1.0f - k};
        glLightfv(LUZ_AMBIENTE, GL_AMBIENT, luzTela);

        /* Exibe uma mira na tela */
        glutWireCone(0.25, 2, 4, 0); 
    }
    else {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, modelo.texturaId);
        glScaled(2 * raio, altura, 2 * raio);

        /* Desenha os vértices do arquivo */
        glVertexPointer(3, GL_DOUBLE, 0, modelo.coords);
        glDrawArrays(GL_TRIANGLES, 0, modelo.numVertices);
    }
    glPopMatrix();
}

/*------------------------------------------------------------------*/

int Nave::getAngHoriz()        { return angHoriz;        }
int Nave::getAngVert()         { return angVert;         }
int Nave::getVidas()           { return vidas;           }
int Nave::getInvencibilidade() { return invencibilidade; }
int Nave::getScore()           { return score;           }

void Nave::aumentaScore(int aumento) { score += aumento; }

/*------------------------------------------------------------------*/

void liberaNave()
{
    liberaTextura(modelo);
    liberaVertices(modelo);
}
