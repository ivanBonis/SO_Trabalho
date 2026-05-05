#ifndef RELATORIO_H
#define RELATORIO_H

#include "fila.h"

// Imprime tempo atual, running, prontos, bloqueados e terminados
void imprimirRelatorioSistema(int tempo_atual, int indice_running, PNodoFila fila_prontos, PNodoFila fila_bloqueados,PNodoFila fila_terminados);

// Imprime uma fila de processos usando os indices guardados nela
void imprimirFilaProcessos(const char *titulo, PNodoFila fila);

// Imprime estatisticas do final da simulacao
void imprimirEstatisticasFinais(int tempo_atual);

// turnaround = tempo_fim - tempo_chegada (apenas do processo mas depois podemos incluir de todos apos a simulacao?)
int calcularTurnaroundProcesso(int indice_processo);

// espera inicial = tempo_inicio - tempo_chegada
int calcularTempoEsperaInicial(int indice_processo);

// Retorna 1 se falhou deadline 0 caso contrário
int processoFalhouDeadline(int indice_processo);

// Pensar em mais estatisticas

#endif