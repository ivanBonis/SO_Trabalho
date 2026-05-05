#ifndef PROCESSO_H
#define PROCESSO_H

#include "config.h"

typedef enum {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} EstadoProcesso;

typedef struct {
    int pid;
    int ppid;
    char nome_programa[MAX_NOME_PROGRAMA];
    int inicio;
    int comprimento;
    int pc;
    int var;
    int prioridade;
    int tempo_chegada;
    int tempo_inicio;
    int tempo_fim;
    int tempo_cpu;
    int periodo;
    int deadline;
    int deadline_abs;
    EstadoProcesso estado;
} PCB;

typedef struct {
    char nome_programa[MAX_NOME_PROGRAMA];
    int tempo_chegada;
    int prioridade;
    int periodo;
    int deadline;
    int criado;
} LinhaPlano;

extern PCB tabela_processos[MAX_PROCESSOS];
extern int total_processos;
extern int prox_pid;

void inicializarTabelaProcessos(void);

int indiceValido(int indice);

int calcularDeadlineAbs(int tempo_chegada, int deadline);

int criarProcesso(const char *nome_programa, int ppid, int inicio, int comprimento,int prioridade, int tempo_chegada, int periodo, int deadline);

int criarProcessoFilho(int indice_pai, int tempo_atual);

int terminarProcesso(int indice, int tempo_atual);

int incPC(int indice);
int definirPC(int indice, int novo_pc);

int incTempoCPU(int indice);

PCB *obterProcessoPorIndice(int indice);
PCB *obterProcessoPorPID(int pid);

int alterarEstadoProcesso(int indice, EstadoProcesso novo_estado);
int marcarInicioProcesso(int indice, int tempo_atual);

int atualizarProgramaProcesso(int indice, const char *novo_nome, int novo_inicio, int novo_comprimento);

const char *estadoParaString(EstadoProcesso estado);

void imprimirProcesso(int indice);
void imprimirTabelaProcessos(void);

#endif