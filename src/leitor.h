#ifndef LEITOR_H
#define LEITOR_H

#include "config.h"
#include "processo.h"
#include "instrucao.h"

// Lê um ficheiro .prg e converte cada linha numa Instrucao
// Retorna o número de instruções lidas ou -1 em erro
int lerPrograma(const char *nome_ficheiro, Instrucao instrucoes[], int max_instrucoes);

// Lê o ficheiro plan.txt
// Cada linha pode ter o formato:
// programa tempo_chegada;
// programa tempo_chegada prioridade;
// programa tempo_chegada prioridade periodo deadline
// Retorna o número de entradas lidas, ou -1 em erro
int lerPlano(const char *nome_ficheiro, LinhaPlano entradas[], int max_entradas);

// Lê o ficheiro control.txt
// Comandos válidos: E, I, D, R, T (ler pagina 4 enunciado do trabalho)
// Retorna o número de comandos lidos ou -1 em erro
int lerControlo(const char *nome_ficheiro, char comandos[], int max_comandos);

// Verifica se uma instrução .prg é válida
// Instruções válidas: M, A, S, B, T, C, L
int instrucaoValida(char op);

// Verifica se um comando do control.txt é válido
// Comandos válidos: E, I, D, R, T
int comandoValido(char comando);

// Valida se o nome do ficheiro respeita o limite do tamanho definido pela constante MAX_NOME_PROGRAMA
int validarNomeFicheiro(const char *nome);

// Normaliza o nome do programa
// Exemplo "filho1" passa para "filho1.prg"
int normalizarNomePrograma(const char *entrada, char saida[], int tamanho_saida);

#endif