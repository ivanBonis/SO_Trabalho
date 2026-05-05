#ifndef MEMORIA_H
#define MEMORIA_H

#include "config.h"
#include "instrucao.h"

typedef struct {
    char nome[MAX_NOME_PROGRAMA];
    int inicio;
    int comprimento;
    int carregado;
    int processos_a_usar;
} InfoPrograma;

extern Instrucao memoria[TAM_MEMORIA];
extern InfoPrograma programas_carregados[MAX_PROGRAMAS];
extern int total_programas_carregados;

// Inicializa memoria[] com instruções vazias e limpa a tabela de programas
void inicializarMemoria(void);

// Carrega programa para memory[] se ainda não estiver carregado
// Se já estiver carregado devolve inicio/comprimento existentes
// Retorna 0 em sucesso -1 em erro
int carregarProgramaMemoria(const char *nome_programa, int *inicio, int *comprimento);

// Procura programa na tabela de programas carregados
// Retorna indice na tabela ou -1 se não existir
int procurarProgramaCarregado(const char *nome_programa);

// Obtem inicio e comprimento de um programa carregado
int obterInfoPrograma(const char *nome_programa, int *inicio, int *comprimento);

// Procura espaco livre consecutivo para comprimento (numero de instruções)
int procurarEspacoLivre(int comprimento);

// Escreve instruções na memória a partir de inicio.
int alocarProgramaMemoria(Instrucao instrucoes[], int comprimento, int inicio);

// Liberta memória ocupada por um programa.
int libertarProgramaMemoria(const char *nome_programa);

// Incrementa contador de processos (processos_a_usar) que usam o programa.
int incrementarUsoPrograma(const char *nome_programa);

// Decrementa contador de uso (processos_a_usar)
// se chegar a 0 pode libertar programa
int decrementarUsoPrograma(const char *nome_programa);

// Retorna instrucao guardada num endereco da memoria
Instrucao obterInstrucaoMemoria(int endereco);

// Coloca uma instrução "dummy" numa posição da memoria
void limparInstrucaoMemoria(int endereco);

// Imprime posições ocupadas da memória
void imprimirMemoria(void);

// Imprime tabela de programas carregados
void imprimirProgramasCarregados(void);

#endif