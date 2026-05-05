#ifndef INSTRUCAO_H
#define INSTRUCAO_H

#include "config.h"


typedef struct{
    char op;
    int arg;
    char nome_ficheiro[MAX_NOME_FICHEIRO];
} Instrucao;

#endif