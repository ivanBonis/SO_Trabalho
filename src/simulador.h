#ifndef SIMULADOR_H
#define SIMULADOR_H

#include "fila.h"
#include "processo.h"
#include "escalonador.h"

typedef struct {
    int tempo_atual;
    int indice_running;

    PNodoFila fila_prontos;
    PNodoFila fila_bloqueados;
    PNodoFila fila_terminados;

    LinhaPlano plano[MAX_PROCESSOS];
    int total_entradas_plano;

    char comandos[MAX_COMANDOS];
    int total_comandos;

    AlgEscalonamento algoritmo;
} Simulador;

// Inicializa tempo, filas, memória e tabela de processos.
void inicializarSimulador(Simulador *sim);

// Lê plan.txt e control.txt.
int carregarDadosSimulador(
    Simulador *sim,
    const char *ficheiro_plano,
    const char *ficheiro_controlo
);

// Cria processos cujo tempo_chegada <= tempo_atual.
int criarProcessosDoPlano(Simulador *sim);

// Executa todos os comandos de control.txt.
int executarSimulador(Simulador *sim);

// Executa um comando individual: E, I, D, R ou T.
int executarComando(Simulador *sim, char comando);

// Comando E.
int comandoExecutar(Simulador *sim);

// Comando I.
int comandoInterromper(Simulador *sim);

// Comando D.
int comandoDesbloquear(Simulador *sim);

// Comando R.
void comandoRelatorio(Simulador *sim);

// Comando T.
void comandoTerminar(Simulador *sim);

// Cria instâncias periódicas quando aplicável.
int atualizarProcessosPeriodicos(Simulador *sim);

// Liberta filas e recursos no fim.
void destruirSimulador(Simulador *sim);

#endif