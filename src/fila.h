#ifndef FILA_H
#define FILA_H

struct NodoFila {
    int elemento; // guarda o índice do processo na tabela de PCBs
    struct NodoFila *prox;
};

typedef struct NodoFila *PNodoFila;

/*
 * Cria um novo nodo da fila
 * Recebe o valor a guardar no nodo
 * Retorna o ponteiro para o nodo criado ou NULL em erro
 */
PNodoFila criarNodoFila(int valor);

/*
 * Liberta um nodo da fila
 * Retorna NULL para evitar ponteiros pendurados
 */
PNodoFila libertarNodoFila(PNodoFila nodo);

/*
 * Cria uma fila vazia
 * Nesta implementação uma fila vazia é representada por NULL.
 */
PNodoFila criarFila(void);

/*
 * Verifica se a fila está vazia
 * Retorna 1 se estiver vazia 0 caso contrário
 */
int filaVazia(PNodoFila fila);

/*
 * Junta um elemento ao fim da fila
 * Retorna o início da fila atualizado
 */
PNodoFila juntar(int valor, PNodoFila fila);

/*
 * Remove o primeiro elemento da fila
 * Retorna o início da fila atualizado
 */
PNodoFila remover(PNodoFila fila);

/*
 * Devolve o elemento da frente da fila
 * Se a fila estiver vazia retorna -1
 */
int frente(PNodoFila fila);

/*
 * Devolve o número de elementos da fila
 */
int tamanhoFila(PNodoFila fila);

/*
 * Devolve o elemento numa posição da fila
 * posicao = 0 corresponde ao primeiro elemento
 * Retorna -1 se a posição for inválida
 */
int obterElementoFila(PNodoFila fila, int posicao);

/*
 * Remove o elemento numa posição lógica da fila
 * Retorna o início da fila atualizado
 */
PNodoFila removerPosicaoFila(PNodoFila fila, int posicao);

/*
 * Liberta todos os nós da fila
 * Retorna NULL
 */
PNodoFila libertarFila(PNodoFila fila);

/*
 * Imprime os elementos da fila
 */
void imprimirFila(PNodoFila fila);

#endif