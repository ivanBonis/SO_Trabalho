#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "fila.h"

typedef enum {
    ALG_FCFS,
    ALG_PRIORITY,
    ALG_SJF,
    ALG_EDF,
    ALG_RM
} AlgEscalonamento;

// Converte string para algoritmo usar strcmp para determinar o algoritmo a usar
// O nome sera passado para a funcao, esse nome e extraido do comando de execucao (agrv[3]) do simulador atraves do main.c 
// Exemplo: "fcfs" -> ALG_FCFS.
int interpretarAlgoritmo(const char *nome, AlgEscalonamento *alg);

// Escolhe e remove o proximo processo da fila de prontos
int escolherProcesso(PNodoFila *fila_prontos, AlgEscalonamento alg);

// FCFS: remove o primeiro da fila
int escolherFCFS(PNodoFila *fila_prontos);

// Priority: escolhe menor prioridade numerica
int escolherPriority(PNodoFila *fila_prontos);

// SJF: escolhe menor tempo restante estimado
int escolherSJF(PNodoFila *fila_prontos);

// EDF: escolhe menor deadline absoluto
int escolherEDF(PNodoFila *fila_prontos);

// RM: escolhe menor periodo
int escolherRM(PNodoFila *fila_prontos);

// tempo_restante = comprimento - pc
int calcularTempoRestanteProcesso(int indice_processo);

#endif