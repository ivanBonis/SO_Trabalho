#include<string.h>
#include<stdio.h>
#include "config.h"
#include "processo.h"

PCB tabela_processos[MAX_PROCESSOS];
int total_processos = 0;
int prox_pid = 1; // o processo gestor tem pid 0

void inicializarTabelaProcessos(void){
    for(int i=0; i < MAX_PROCESSOS; i++){
        tabela_processos[i].pid = -1;
        tabela_processos[i].ppid = -1;
        tabela_processos[i].nome_programa[0] = '\0';

        tabela_processos[i].inicio = -1;
        tabela_processos[i].comprimento = 0;
        tabela_processos[i].pc = 0;

        tabela_processos[i].var = 0;

        tabela_processos[i].prioridade = -1;
        tabela_processos[i].tempo_chegada = 0;
        tabela_processos[i].tempo_inicio = -1;
        tabela_processos[i].tempo_fim = -1;
        tabela_processos[i].tempo_cpu = 0;
        tabela_processos[i].periodo = 0;
        tabela_processos[i].deadline = 0;
        tabela_processos[i].deadline_abs = 0;

        tabela_processos[i].estado = NEW;
    }

    total_processos = 0;
    prox_pid = 1;
}

// o deadline absoluto representa o instante de tempo exato onde o processo tem de terminar 
int calcularDeadlineAbs(int tempo_chegada, int deadline){
    if(deadline > 0)
        return tempo_chegada + deadline;

    return 0;
}

int criarProcesso(const char *nome_programa, int ppid, int inicio, int comprimento, int prioridade, int tempo_chegada, int periodo, int deadline){
    if(total_processos >= MAX_PROCESSOS)
        return -1;

    if(nome_programa == NULL || nome_programa[0] == '\0')
        return -1;

    if(inicio < 0 || comprimento <= 0)
        return -1;

    PCB *processo = &tabela_processos[total_processos];
    int indice = total_processos;

    processo->pid = prox_pid;
    processo->ppid = ppid;
    strncpy(processo->nome_programa, nome_programa, MAX_NOME_PROGRAMA-1);
    processo->nome_programa[MAX_NOME_PROGRAMA-1] = '\0';

    processo->inicio = inicio;
    processo->comprimento = comprimento;
    processo->pc = 0;
    processo->var = 0;

    processo->prioridade = prioridade;
    processo->tempo_chegada = tempo_chegada;
    processo->tempo_inicio = -1;
    processo->tempo_fim = -1;
    processo->tempo_cpu = 0;

    processo->periodo = periodo;
    processo->deadline = deadline;
    processo->deadline_abs = calcularDeadlineAbs(tempo_chegada, deadline);

    processo->estado = READY;

    total_processos++;
    prox_pid++;

    return indice; // utilizar este indice para a fila de processos (corresponde a onde na tabela esta o processo em vez de guardar o PCB todo)
}

const char *estadoParaString(EstadoProcesso estado){
    switch(estado){
        case NEW:
            return "NEW";
        case READY:
            return "READY";
        case RUNNING:
            return "RUNNING";
        case BLOCKED:
            return "BLOCKED";
        case TERMINATED:
            return "TERMINATED";
        default:
            return "DESCONHECIDO";
    }
}

void imprimirProcesso(int indice){
    if(indice < 0 || indice >= total_processos)
        return;

    PCB *processo = &tabela_processos[indice];

    printf("\n=== Processo indice %d ===\n", indice);
    printf("PID: %d\n", processo->pid);
    printf("PPID: %d\n", processo->ppid);
    printf("Nome programa: %s\n", processo->nome_programa);
    printf("Estado: %s\n", estadoParaString(processo->estado));
    printf("Início na memória: %d\n", processo->inicio);
    printf("Comprimento: %d\n", processo->comprimento);
    printf("PC: %d\n", processo->pc);
    printf("Variavel: %d\n", processo->var);
    printf("Prioridade: %d\n", processo->prioridade);
    printf("Tempo chegada: %d\n", processo->tempo_chegada);
    printf("Tempo Início: %d\n", processo->tempo_inicio);
    printf("Tempo fim: %d\n", processo->tempo_fim);
    printf("Tempo CPU: %d\n", processo->tempo_cpu);
    printf("Período: %d\n", processo->periodo);
    printf("Deadline: %d\n", processo->deadline);
    printf("Deadline absoluto: %d\n", processo->deadline_abs);

    return;
}

void imprimirTabelaProcessos(void){
    printf("\n=== Tabela de Processos ===\n");
    for(int i=0; i < total_processos; i++){
        imprimirProcesso(i);
    }
    return;
}

PCB *obterProcessoPorIndice(int indice){
    if(indice < 0 || indice >= total_processos)
        return NULL;

    return &tabela_processos[indice];
}