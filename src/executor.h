#include "config.h"
#include "processo.h"

extern PCB tabela_processos[MAX_PROCESSOS];
extern int total_processos;
extern int prox_pid;

void inicializarTabelaProcessos(void);
int calcularDeadlineAbs(int tempo_chegada, int deadline);
int criarProcesso(const char *nome_programa, int ppid, int inicio, int comprimento, int prioridade, int tempo_chegada, int periodo, int deadline);