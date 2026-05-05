#include<stdio.h>
#include "config.h"
#include "processo.h"

int main(int argc, char *argv[]){
    if(argc < 4){
        printf("USO: %s dados/plan.txt dados/control.txt [ALGORITMO]\n", argv[0]);
        return 1;
    }

    inicializarTabelaProcessos();

    printf("Simulador de Gestão de Processos\n");
    printf("Plano: %s\n", argv[1]);
    printf("Controlo: %s\n", argv[2]);
    printf("Algoritmo: %s\n", argv[3]);
    printf("Tamanho da memória: %d\n", TAM_MEMORIA);
    printf("Máximo de processos: %d\n", MAX_PROCESSOS);
    printf("Quantum: %d\n", QUANTUM);

    // Teste de criar um processo

    int indice = criarProcesso("teste.prg", 0, 0, 4, 1, 0, 4, 10);

    if(indice == -1){
        printf("Erro ao criar processo de teste\n");
        return 1;
    }

    imprimirTabelaProcessos();

    return 0;
}