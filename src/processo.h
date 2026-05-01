#include "config.h"

typedef enum{
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
    EstadoProcesso Estado;
} PCB;

typedef struct{
    char nome_programa[MAX_NOME_PROGRAMA];
    int tempo_chegada;
    int prioridade;
    int periodo;
    int deadline;
    int criado;
} InfoPrograma;