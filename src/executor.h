#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "fila.h"

// Executa um processo durante no máximo QUANTUM instruções
// Para antes se terminar ou bloquear
int executarQuantum(int indice_processo, PNodoFila *fila_prontos, PNodoFila *fila_bloqueados, PNodoFila *fila_terminados, int *tempo_atual);

// Executa uma instrução do processo
int executarInstrucao(int indice_processo, PNodoFila *fila_prontos, PNodoFila *fila_bloqueados, PNodoFila *fila_terminados, int *tempo_atual);

// M n: var = n
int executarM(int indice_processo, int valor);

// A n: var += n
int executarA(int indice_processo, int valor);

// S n: var -= n
int executarS(int indice_processo, int valor);

// B: bloqueia processo
int executarB(int indice_processo, PNodoFila *fila_bloqueados);

// T: termina processo
int executarT(int indice_processo, PNodoFila *fila_terminados, int tempo_atual);

// C n: cria filho e coloca em prontos
int executarC(int indice_processo, int salto, PNodoFila *fila_prontos, int tempo_atual);

// L ficheiro: troca programa do processo
int executarL(int indice_processo, const char *nome_programa);

// Comando I: interrompe processo e envia para bloqueados
int interromperProcesso(int indice_processo, PNodoFila *fila_bloqueados);

#endif