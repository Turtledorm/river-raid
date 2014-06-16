#include <stdio.h>
#include <string.h>
#include "Textura.h"
#include "Base.h"

/*-------------------------*
 |   D E F I N I Ç Õ E S   |
 *-------------------------*/

/* Arquivos de textura */
#define FUNDO_TEXTURA  "texture/space.ppm"
#define RIO_TEXTURA    "texture/water.ppm"
#define PAREDE_TEXTURA "texture/brick.ppm"

GLuint fundoTextura;
GLuint rioTextura;
GLuint paredeTextura;

static void ignoraComentario(FILE *file);
static void erro(FILE *file, const char *filename);

/*-------------------*
 |   F U N Ç Õ E S   |
 *-------------------*/

void inicializaTexturas()
{
    carregaTextura(FUNDO_TEXTURA,  &fundoTextura);
    carregaTextura(RIO_TEXTURA,    &rioTextura);
    carregaTextura(PAREDE_TEXTURA, &paredeTextura);
}

/*------------------------------------------------------------------*/

void carregaTextura(const char *filename, GLuint *textura)
{
    /* Abre o arquivo */
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("carregaTextura()");
        exit(EXIT_FAILURE);
    }

    /* Faz verificação da chave mágica */
    char aux[3];
    fscanf(file, "%2s", aux);
    ignoraComentario(file);
    if (strcmp(aux, "P6") != 0) erro(file, filename);

    /* Obtém largura e altura */
    GLsizei largura, altura;
    ignoraComentario(file);
    if (fscanf(file, "%d %d", &largura, &altura) != 2) erro(file, filename);

    /* Obtém tamanho do arquivo */
    fseek(file, 0, SEEK_END);
    GLsizei n = ftell(file);
    rewind(file);

    /* Lê dados para uma string */
    GLubyte *dados = mallocSafe(sizeof (GLubyte) * n);
    fread(dados, sizeof (GLubyte), n, file);
    fclose(file);

    /* DOOM: Hell begins here */
    glGenTextures(1, textura);
    glBindTexture(GL_TEXTURE_2D, *textura);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, largura, altura,
                      GL_RGB, GL_UNSIGNED_BYTE, dados);
    free(dados);
    /* Hell ends here */
}

/*------------------------------------------------------------------*/

void liberaTexturas()
{
    glDeleteTextures(1, &fundoTextura);
    glDeleteTextures(1, &rioTextura);
    glDeleteTextures(1, &paredeTextura);
}

/*------------------------------------------------------------------*/

static void ignoraComentario(FILE *file)
{
    while (getc(file) == '\n');
    fseek(file, -1, SEEK_CUR);

    if (getc(file) == '#') {
        while (getc(file) != '\n');
    }
    else fseek(file, -1, SEEK_CUR);
}

/*------------------------------------------------------------------*/

static void erro(FILE *file, const char *filename)
{
    fprintf(stderr, "carregaTextura(): "
        "%s não é um arquivo PPM\n", filename);
    fclose(file);
    exit(EXIT_FAILURE);
}
