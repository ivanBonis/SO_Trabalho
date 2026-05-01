# Organização do Projeto de SO — Simulador de Processos

## 1. Resumo do trabalho

O trabalho consiste em desenvolver um **simulador de gestão de processos** de um sistema operativo.

O simulador deve representar, de forma simplificada, aquilo que um sistema operativo faz quando gere processos:

- criação de processos;
- execução de instruções;
- bloqueio e desbloqueio de processos;
- terminação de processos;
- interrupção de processos;
- comutação de contexto;
- gestão de filas de processos;
- escalonamento do CPU;
- carregamento de programas para uma memória simulada;
- libertação ou reutilização de memória simulada;
- geração de relatórios sobre o estado do sistema;
- geração de estatísticas finais.

Cada processo executa um programa `.prg`. Esses programas têm instruções simples, como alterar uma variável inteira, somar, subtrair, bloquear, terminar, criar processos filhos e carregar outro programa.

A prioridade inicial é termos uma versão simples a funcionar. Depois acrescentamos as partes mais complexas.

---

## 2. Estrutura das diretorias

A estrutura do projeto será a seguinte:

```txt
simulador-processos/
├── Makefile
├── README.md
├── dados/
│   ├── plan.txt
│   ├── control.txt
│   ├── simples.prg
│   ├── bloqueio.prg
│   ├── progenitor.prg
│   ├── filho1.prg
│   └── filho2.prg
├── documentacao/
│   └── relatorio.pdf
└── src/
    ├── main.c
    ├── configuracao.h
    ├── instrucao.h
    ├── processo.h
    ├── fila.h
    ├── fila.c
    ├── leitor.h
    ├── leitor.c
    ├── memoria.h
    ├── memoria.c
    ├── escalonador.h
    ├── escalonador.c
    ├── executor.h
    ├── executor.c
    ├── relatorio.h
    └── relatorio.c
```

### Diretoria principal

Contém o `Makefile`, o `README.md`, os dados de entrada, a documentação e o código-fonte.

### `dados/`

Contém os ficheiros usados para executar e testar o simulador:

- programas `.prg`;
- `plan.txt`;
- `control.txt`.

Vamos usar os nomes `plan.txt` e `control.txt` porque são os nomes usados no enunciado.

### `documentacao/`

Contém o relatório final do trabalho.

### `src/`

Contém todos os ficheiros `.c` e `.h` do simulador.

---

## 3. Ficheiros de entrada

## 3.1 Programas `.prg`

Cada ficheiro `.prg` representa um programa que pode ser executado por um processo.

Exemplo:

```txt
M 100
A 20
S 5
B
A 10
T
```

Instruções possíveis:

| Instrução | Significado |
|---|---|
| `M n` | Muda o valor da variável do processo para `n` |
| `A n` | Soma `n` à variável do processo |
| `S n` | Subtrai `n` à variável do processo |
| `B` | Bloqueia o processo |
| `T` | Termina o processo |
| `C n` | Cria um processo filho e faz o pai saltar `n` instruções a partir do `C` |
| `L ficheiro` | Substitui o programa associado ao processo atual por outro programa |

Nota importante sobre o `L`: o enunciado diz que o nome do ficheiro tem no máximo 15 caracteres. Podemos usar arrays maiores em C por segurança, mas temos de validar esse limite na leitura.

No `plan.txt`, vamos usar nomes com extensão `.prg`, por exemplo:

```txt
simples.prg 0
```

Na instrução `L`, vamos aceitar nomes com ou sem extensão, porque o enunciado usa exemplos como `L filho1`.

Exemplos válidos:

```txt
L filho1
L filho1.prg
```

Internamente, o leitor pode normalizar o nome para o formato usado nos ficheiros. Por exemplo, se receber `filho1`, pode procurar/carregar `filho1.prg`.

---

## 3.2 `plan.txt`

O ficheiro `plan.txt` indica que programas entram no sistema e em que instante de tempo.

No `plan.txt`, vamos usar nomes com extensão `.prg`.

O formato base do enunciado é:

```txt
programa.prg tempo_chegada
```

Exemplo:

```txt
progenitor.prg 0
progenitor.prg 20
filho1.prg 30
```

Para conseguirmos implementar Priority, EDF e Rate Monotonic, vamos aceitar formatos alargados.

Formato com prioridade:

```txt
programa.prg tempo_chegada prioridade
```

Exemplo:

```txt
progenitor.prg 0 4
filho1.prg 20 2
filho2.prg 30 3
```

Significado:

```txt
progenitor.prg entra no tempo 0 com prioridade 4
filho1.prg entra no tempo 20 com prioridade 2
filho2.prg entra no tempo 30 com prioridade 3
```

Para algoritmos de tempo real, o formato pode ser:

```txt
programa.prg tempo_chegada prioridade periodo deadline
```

Exemplo:

```txt
tarefa1.prg 0 1 5 5
tarefa2.prg 0 2 10 10
```

Significado:

```txt
tarefa1.prg entra no tempo 0, prioridade 1, período 5 e deadline 5
tarefa2.prg entra no tempo 0, prioridade 2, período 10 e deadline 10
```

Regras práticas:

```txt
Se a linha tiver só programa e tempo_chegada:
    prioridade = valor por defeito
    periodo = 0
    deadline = 0

Se a linha tiver programa, tempo_chegada e prioridade:
    periodo = 0
    deadline = 0

Se a linha tiver os cinco campos:
    é uma tarefa periódica ou de tempo real
```

---

## 3.3 `control.txt`

O ficheiro `control.txt` contém os comandos que controlam a simulação.

| Comando | Significado |
|---|---|
| `E` | Executa o simulador durante um quantum de tempo |
| `I` | Interrompe um processo e bloqueia-o |
| `R` | Imprime relatório do estado atual do sistema |
| `D` | Executa o escalonador de longo prazo e desbloqueia processo(s) bloqueado(s) |
| `T` | Termina o simulador e imprime estatísticas finais |

Exemplo:

```txt
R
E
R
I
R
D
E
R
T
```

Regras dos comandos:

```txt
E executa no máximo QUANTUM_TEMPO instruções.
I bloqueia o processo atualmente em execução. Se não houver RUNNING, bloqueia o próximo processo READY escolhido pelo escalonador.
R imprime o relatório atual.
D executa o escalonador de longo prazo. Na nossa implementação base, desbloqueia o primeiro processo da fila de bloqueados e devolve-o à fila de prontos.
T termina sempre com estatísticas finais.
```

---

## 4. Regras globais da simulação

## 4.1 Regra do tempo

O simulador funciona por unidades de tempo.

Cada instrução executada de um programa `.prg` consome 1 unidade de tempo.

As instruções que consomem tempo são:

```txt
M
A
S
B
T
C
L
```

Depois de cada instrução executada:

```txt
tempo_atual aumenta 1
tempo_cpu do processo aumenta 1
```

Os comandos do `control.txt` não consomem tempo diretamente:

```txt
R não consome tempo
D não consome tempo
I não consome tempo
T não consome tempo
```

O tempo de simulação só avança quando uma instrução de processo é executada.

---

## 4.2 Regra do `pc`

O `pc` é **relativo ao início do programa**.

A instrução atual de um processo é:

```c
memory[pcb.inicio + pcb.pc]
```

Quando uma instrução normal é executada:

```txt
pcb.pc aumenta 1
```

Quando o processo troca de programa com `L`:

```txt
pcb.pc = 0
```

Esta regra é importante para evitar confusão entre posição absoluta na memória e posição relativa dentro do programa.

---

## 4.3 Criação de processos a partir do `plan.txt`

Antes de executar cada comando do `control.txt`, o simulador deve verificar o `plan.txt`.

Devem ser criados todos os processos com:

```txt
tempo_chegada <= tempo_atual
```

desde que ainda não tenham sido criados.

Isto evita perder processos caso o tempo avance para além do instante exato de chegada.

---

## 4.4 Valores iniciais recomendados para um PCB

O processo gestor do simulador tem PID 0. Os processos criados diretamente a partir do `plan.txt` têm `ppid = 0`.

Quando um processo é criado:

```txt
pid = novo PID
ppid = 0, se vier do plan.txt
nome_programa = programa carregado
inicio = posição inicial do programa na memória
comprimento = número de instruções do programa
pc = 0
variavel = 0
prioridade = prioridade definida ou valor por defeito
tempo_chegada = tempo atual ou tempo definido no plan.txt
tempo_inicio = -1
tempo_fim = -1
tempo_cpu = 0
periodo = valor definido ou 0
deadline = valor definido ou 0
deadline_absoluto = tempo_chegada + deadline, se existir deadline
estado = READY
```

Quando o processo executa pela primeira vez:

```txt
tempo_inicio = tempo_atual
```

Quando o processo termina:

```txt
tempo_fim = tempo_atual
```

---

## 5. Structs principais

Estas são as estruturas base que vamos usar no projeto. A ideia é todos usarem os mesmos nomes e campos para evitar confusão quando juntarmos os módulos.

---

## 5.1 `Instrucao`

Representa uma instrução de um ficheiro `.prg` já convertida para C.

```c
#define MAX_NOME_FICHEIRO 64
#define LIMITE_NOME_ENUNCIADO 15

typedef struct {
    char op;
    int arg;
    char nome_ficheiro[MAX_NOME_FICHEIRO];
} Instrucao;
```

Usamos `MAX_NOME_FICHEIRO` maior por segurança, mas validamos o limite do enunciado com `LIMITE_NOME_ENUNCIADO`.

Significado dos campos:

```txt
op             operação da instrução: M, A, S, B, T, C ou L
arg            argumento numérico usado em M, A, S e C
nome_ficheiro  nome do ficheiro usado na instrução L
```

Exemplos:

```txt
M 100          -> op = 'M', arg = 100, nome_ficheiro = ""
A 20           -> op = 'A', arg = 20, nome_ficheiro = ""
B              -> op = 'B', arg = 0,   nome_ficheiro = ""
T              -> op = 'T', arg = 0,   nome_ficheiro = ""
C 2            -> op = 'C', arg = 2,   nome_ficheiro = ""
L filho1       -> op = 'L', arg = 0,   nome_ficheiro = "filho1"
L filho1       -> op = 'L', arg = 0,   nome_ficheiro = "filho1"
L filho1.prg   -> op = 'L', arg = 0,   nome_ficheiro = "filho1.prg"
```

---

## 5.2 `EstadoProcesso`

Representa o estado atual de um processo.

```c
typedef enum {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} EstadoProcesso;
```

Significado:

```txt
NEW          processo criado mas ainda não está pronto
READY        processo pronto a executar
RUNNING      processo em execução
BLOCKED      processo bloqueado
TERMINATED   processo terminado
```

Isto evita usarmos números soltos no código, como `0`, `1`, `2`, etc.

---

## 5.3 `PCB`

O PCB significa **Process Control Block**. É a estrutura mais importante do projeto.

Cada processo tem um PCB. O PCB guarda tudo o que o simulador precisa de saber sobre esse processo.

```c
#define MAX_NOME_PROGRAMA 64

typedef struct {
    int pid;
    int ppid;

    char nome_programa[MAX_NOME_PROGRAMA];

    int inicio;
    int comprimento;
    int pc;

    int variavel;

    int prioridade;
    int tempo_chegada;

    int tempo_inicio;
    int tempo_fim;
    int tempo_cpu;

    int periodo;
    int deadline;
    int deadline_absoluto;

    EstadoProcesso estado;
} PCB;
```

Significado dos campos:

```txt
pid                identificador único do processo
ppid               pid do processo pai
nome_programa      nome do programa que o processo está a executar
inicio             posição inicial do programa na memória
comprimento        número de instruções do programa
pc                 Program Counter relativo ao início do programa
variavel           variável inteira do processo
prioridade         prioridade do processo
tempo_chegada      instante em que o processo entra no sistema
tempo_inicio       primeira vez que o processo executa
tempo_fim          instante em que o processo termina
tempo_cpu          tempo total de CPU usado pelo processo
periodo            usado no Rate Monotonic
deadline           deadline relativo usado no EDF
deadline_absoluto  deadline calculado a partir do tempo de chegada ou lançamento
estado             estado atual do processo
```

Exemplo:

```txt
pid = 1
ppid = 0
nome_programa = "simples.prg"
inicio = 0
comprimento = 4
pc = 2
variavel = 15
prioridade = 1
tempo_chegada = 0
tempo_inicio = 0
tempo_fim = -1
tempo_cpu = 2
estado = RUNNING
```

---

## 5.4 `InfoPrograma`

Guarda informação sobre programas que já foram carregados para a memória.

Isto serve para evitar carregar o mesmo programa várias vezes.

```c
typedef struct {
    char nome[MAX_NOME_PROGRAMA];
    int inicio;
    int comprimento;
    int carregado;
    int processos_a_usar;
} InfoPrograma;
```

Significado:

```txt
nome              nome do programa
inicio            posição onde o programa começa na memória
comprimento       número de instruções do programa
carregado         indica se o programa está carregado ou não
processos_a_usar  número de processos que ainda usam este programa
```

O campo `processos_a_usar` é importante porque não podemos libertar um programa da memória só porque um processo terminou. Só podemos libertar quando nenhum processo o estiver a usar.

Esta regra também resolve o caso em que vários processos são criados a partir do mesmo programa. O programa fica carregado uma vez na memória e é reutilizado.

---

## 5.5 `Fila`

Representa uma fila de processos.

Vamos usar esta estrutura para a fila de prontos, bloqueados e terminados.

```c
#define MAX_PROCESSOS 100

typedef struct {
    int elementos[MAX_PROCESSOS];
    int inicio;
    int fim;
    int tamanho;
} Fila;
```

A fila guarda **índices da tabela de PCBs**, não guarda os PCBs completos.

Exemplo:

```txt
fila_prontos = [0, 2, 5]
```

Significa:

```txt
pcb_tabela[0] está pronto
pcb_tabela[2] está pronto
pcb_tabela[5] está pronto
```

---

## 5.6 `EntradaPlano`

Representa uma linha do ficheiro `plan.txt`.

```c
typedef struct {
    char nome_programa[MAX_NOME_PROGRAMA];
    int tempo_chegada;
    int prioridade;
    int periodo;
    int deadline;
    int criado;
} EntradaPlano;
```

O campo `criado` serve para saber se a entrada do plano já originou um processo.

Exemplo básico:

```txt
simples.prg 0
```

Fica guardado como:

```txt
nome_programa = "simples.prg"
tempo_chegada = 0
prioridade = valor por defeito
periodo = 0
deadline = 0
criado = 0
```

Exemplo com tempo real:

```txt
tarefa1.prg 0 1 5 5
```

Fica guardado como:

```txt
nome_programa = "tarefa1.prg"
tempo_chegada = 0
prioridade = 1
periodo = 5
deadline = 5
criado = 0
```

---

## 5.7 Resumo das structs

```txt
Instrucao       representa uma linha de um programa .prg
EstadoProcesso  representa o estado de um processo
PCB             guarda toda a informação de um processo
InfoPrograma    guarda informação sobre programas carregados na memória
Fila            guarda índices de processos
EntradaPlano    representa uma linha do plan.txt
```

Estas estruturas devem ser a base do projeto. Se alguém precisar de adicionar campos, deve avisar o grupo antes, porque isso pode afetar vários ficheiros.

---

## 6. Organização dos ficheiros de código

## 6.1 `configuracao.h`

Contém constantes globais do projeto.

Exemplo:

```c
#ifndef CONFIGURACAO_H
#define CONFIGURACAO_H

#define TAMANHO_MEMORIA 1000
#define MAX_PROCESSOS 100
#define MAX_PROGRAMAS 100
#define MAX_INSTRUCOES_PROGRAMA 100
#define MAX_COMANDOS 1000

#define MAX_NOME_FICHEIRO 64
#define MAX_NOME_PROGRAMA 64
#define LIMITE_NOME_ENUNCIADO 15

#define QUANTUM_TEMPO 3
#define PRIORIDADE_DEFAULT 5

#define SEM_PAI 0
#define TEMPO_NAO_DEFINIDO -1

#endif
```

---

## 6.2 `instrucao.h`

Define o tipo de instrução que será guardado na memória simulada.

---

## 6.3 `processo.h`

Define as estruturas relacionadas com processos:

- estados dos processos;
- PCB;
- informação sobre programas carregados;
- entrada do plano.

---

## 6.4 `fila.h` e `fila.c`

Implementam a estrutura de fila usada pelo simulador.

Filas principais:

```txt
fila_prontos      processos prontos a executar
fila_bloqueados   processos bloqueados
fila_terminados   processos terminados
```

Funções previstas:

```c
void criarFila(Fila *f);
int filaVazia(Fila *f);
int filaCheia(Fila *f);
int filaInserir(Fila *f, int valor);
int filaRemover(Fila *f);
int filaObter(Fila *f, int posicao);
int filaRemoverPosicao(Fila *f, int posicao);
```

---

## 6.5 `leitor.h` e `leitor.c`

Responsáveis por ler ficheiros.

Devem ler:

- programas `.prg`;
- `plan.txt`;
- `control.txt`.

Funções previstas:

```c
int lerPrograma(const char *nome_ficheiro, Instrucao instrucoes[], int max_instrucoes);
int lerPlano(const char *nome_ficheiro, EntradaPlano entradas[], int max_entradas);
int lerControlo(const char *nome_ficheiro, char comandos[], int max_comandos);
```

Também devem validar erros simples:

```txt
instrução desconhecida
argumento em falta
nome de ficheiro demasiado grande
ficheiro inexistente
linha mal formatada
comando de controlo inválido
```

---

## 6.6 `memoria.h` e `memoria.c`

Responsáveis pela memória simulada.

A memória é um array onde ficam guardadas as instruções dos programas:

```c
Instrucao memory[TAMANHO_MEMORIA];
```

Na versão inicial, podemos carregar os programas sequencialmente.

Na versão final, a memória deve respeitar melhor o enunciado:

- verificar se o programa já está carregado;
- se não estiver carregado, procurar espaço livre;
- marcar posições ocupadas;
- marcar posições livres quando um programa já não estiver a ser usado;
- lidar com falta de espaço;
- opcionalmente compactar ou defragmentar a memória.

Para simplificar, podemos usar uma instrução vazia/dummy para representar espaço livre:

```txt
op = ' '
arg = 0
nome_ficheiro = ""
```

Importante: só podemos libertar o programa da memória quando nenhum processo o estiver a usar.

---

## 6.7 `escalonador.h` e `escalonador.c`

Responsáveis por escolher o próximo processo a executar.

Algoritmos previstos:

| Algoritmo | Critério |
|---|---|
| FCFS | Escolhe o primeiro processo da fila |
| Priority | Escolhe o processo com menor valor de prioridade |
| SJF | Escolhe o processo com menor tempo restante estimado |
| EDF | Escolhe o processo com deadline absoluto mais cedo |
| RM | Escolhe o processo com menor período |

Decisão importante:

```txt
Quanto menor o valor numérico da prioridade, maior é a prioridade do processo.
```

Exemplo:

```txt
prioridade 1 executa antes de prioridade 4
```

O escalonamento deve ser preemptivo.

Na prática:

```txt
Depois de cada quantum, se o processo não terminou nem bloqueou, volta para a fila de prontos.
Depois disso, o escalonador volta a escolher o próximo processo.
```

Para SJF, como o `pc` é relativo ao início do programa, vamos usar:

```txt
tempo_restante = comprimento - pc
```

Esta estimativa não tenta prever tudo o que pode acontecer com `C`, `L` ou `B`. É uma simplificação aceitável, mas deve ser explicada no relatório.

---

## 6.8 `executor.h` e `executor.c`

Contêm o núcleo da execução do simulador.

Aqui são implementadas as instruções:

```txt
M
A
S
B
T
C
L
```

Exemplo:

```txt
M 10 -> variável = 10
A 5  -> variável = 15
S 3  -> variável = 12
T    -> processo termina
```

Resultado:

```txt
variável final = 12
estado = TERMINATED
```

---

## 6.9 `relatorio.h` e `relatorio.c`

Responsáveis por imprimir o estado do sistema.

O relatório deve mostrar:

```txt
TEMPO ATUAL
PROCESSO EM EXECUÇÃO
PROCESSOS PRONTOS
PROCESSOS BLOQUEADOS
PROCESSOS TERMINADOS
```

Para cada processo, deve mostrar:

```txt
pid
ppid
programa
estado
pc
valor da variável
prioridade
tempo de CPU usado
tempo de chegada
tempo de início
tempo de fim
deadline absoluto, se existir
período, se existir
```

As estatísticas finais devem incluir, pelo menos:

```txt
número total de processos criados
número total de processos terminados
tempo total de simulação
tempo de CPU usado por processo
turnaround time por processo
tempo médio de turnaround
```

Fórmulas:

```txt
turnaround = tempo_fim - tempo_chegada
turnaround_medio = soma dos turnaround dos processos terminados / número de processos terminados
```

---

## 6.10 `main.c`

É o ponto de entrada do programa.

Responsabilidades:

- inicializar a memória;
- inicializar as filas;
- ler `plan.txt`;
- ler `control.txt` ou comandos do stdin para debug;
- criar processos quando chega o seu tempo;
- chamar o executor;
- chamar o escalonador;
- chamar o relatório;
- terminar o simulador.

Este ficheiro deve ligar os módulos, não deve concentrar a lógica toda.

---

## 6.11 `Makefile`

Permite compilar e executar o projeto.

Comandos previstos:

```bash
make
make run
make clean
```

Significado:

```txt
make       compila o projeto
make run   executa o simulador
make clean remove os ficheiros gerados pela compilação
```

Exemplo de execução:

```bash
./simulador dados/plan.txt dados/control.txt fcfs
```

Mais tarde:

```bash
./simulador dados/plan.txt dados/control.txt priority
./simulador dados/plan.txt dados/control.txt sjf
./simulador dados/plan.txt dados/control.txt edf
./simulador dados/plan.txt dados/control.txt rm
```

---

## 7. Funcionamento geral do simulador

O simulador funciona por unidades de tempo.

Fluxo geral:

1. O programa começa e inicializa as estruturas principais.
2. Lê o ficheiro `plan.txt`.
3. Lê os comandos do ficheiro `control.txt` ou do stdin.
4. Antes de cada comando, cria processos cujo `tempo_chegada <= tempo_atual`.
5. Carrega o programa `.prg` para a memória, se ainda não estiver carregado.
6. Coloca o processo na fila de prontos.
7. Quando aparece o comando `E`, o escalonador escolhe um processo.
8. O processo executa até ao fim do quantum, até bloquear ou até terminar.
9. Se bloquear, vai para a fila de bloqueados.
10. Se terminar, vai para a fila de terminados.
11. Se o quantum acabar e o processo ainda não terminou, volta para a fila de prontos.
12. O comando `I` interrompe um processo e bloqueia-o.
13. O comando `R` imprime o estado atual do sistema.
14. O comando `D` desbloqueia o primeiro processo bloqueado.
15. O comando `T` termina a simulação e imprime estatísticas finais.

Assumimos que as operações internas do simulador são instantâneas. O tempo de simulação só avança quando uma instrução de processo é executada.

---

## 8. Execução das instruções

## 8.1 `M`, `A`, `S` e `T`

```txt
M 10 -> variável = 10
A 5  -> variável = 15
S 3  -> variável = 12
T    -> processo termina
```

Quando uma instrução normal é executada:

```txt
pc avança
tempo atual aumenta
tempo_cpu do processo aumenta
```

No caso de `T`:

```txt
o processo muda para TERMINATED
tempo_fim é atualizado
o processo é inserido na fila de terminados
```

---

## 8.2 `B`

A instrução `B` bloqueia o processo.

Exemplo:

```txt
M 50
B
A 10
T
```

Resultado esperado:

```txt
processo executa M 50
processo executa B e fica bloqueado
quando D o desbloquear, volta para prontos
continua em A 10
termina com variável final = 60
```

Regra:

```txt
B consome 1 unidade de tempo
B avança o pc
B muda o estado para BLOCKED
B coloca o processo na fila de bloqueados
```

---

## 8.3 `I`

O comando `I` não vem do programa `.prg`; vem do `control.txt`.

Serve para interromper e bloquear um processo.

Regra:

```txt
Se existir processo RUNNING:
    guardar o PC atual no PCB
    mudar estado para BLOCKED
    inserir na fila de bloqueados
    libertar o CPU

Se não existir processo RUNNING:
    escolher o próximo processo READY pelo escalonador
    mudar estado para BLOCKED
    inserir na fila de bloqueados
```

O comando `I` não executa nenhuma instrução do programa e não consome tempo de simulação.

---

## 8.4 `D`

O comando `D` desbloqueia processos.

O enunciado permite que o escalonador de longo prazo desbloqueie um ou mais processos, até de forma aleatória.

Para manter o comportamento simples, previsível e fácil de testar, vamos implementar a versão determinística:

```txt
D desbloqueia o primeiro processo da fila de bloqueados.
```

Regra:

```txt
Se a fila de bloqueados não estiver vazia:
    remover o primeiro processo da fila de bloqueados
    mudar estado para READY
    inserir na fila de prontos

Se a fila de bloqueados estiver vazia:
    não faz nada ou imprime uma mensagem simples
```

---

## 8.5 `C n`

A instrução `C n` cria um processo filho.

Regra importante para evitar confusão:

```txt
Se pc_c for a posição da instrução C:
    filho.pc = pc_c + 1
    pai.pc = pc_c + n
```

Exemplo:

```txt
0: M 100
1: C 2
2: L filho1.prg
3: C 2
4: L filho2.prg
5: T
```

Se o pai executa o primeiro `C 2`:

```txt
filho.pc = 2
pai.pc = 3
```

O filho deve ser uma cópia do pai, mas com:

```txt
novo PID
PPID igual ao PID do pai
PC ajustado para a instrução seguinte ao C
estado READY
tempo_chegada = tempo_atual
tempo_inicio = -1
tempo_fim = -1
tempo_cpu = 0
```

O filho herda:

```txt
programa
início
comprimento
variável
prioridade
período e deadline, se existirem
```

A instrução `C` consome 1 unidade de tempo.

---

## 8.6 `L ficheiro`

A instrução `L ficheiro` substitui o programa associado ao processo atual.

Regra:

```txt
verificar se o novo programa já está carregado na memória
se não estiver, carregar
atualizar nome_programa no PCB
atualizar inicio
atualizar comprimento
colocar pc = 0
atualizar contadores processos_a_usar
```

O processo mantém:

```txt
mesmo PID
mesmo PPID
mesma variável
mesma prioridade
mesmo tempo_cpu acumulado
```

O programa antigo só é libertado da memória se nenhum processo o estiver a usar. Esta decisão é importante porque vários processos podem estar a executar o mesmo programa carregado em memória.

A instrução `L` consome 1 unidade de tempo.

O nome pode vir com ou sem extensão:

```txt
L filho1
L filho1.prg
```

Isto simula a ideia de `fork` seguido de `exec`.

---

## 9. Exemplo simples de execução

### `simples.prg`

```txt
M 10
A 5
S 3
T
```

### `plan.txt`

```txt
simples.prg 0
```

### `control.txt`

```txt
R
E
R
E
R
T
```

### Execução esperada

No tempo 0, o processo é criado.

Antes de executar:

```txt
PID = 1
estado = READY
pc = 0
variável = 0
```

Primeiro `E`, com quantum 3:

```txt
M 10 -> variável = 10
A 5  -> variável = 15
S 3  -> variável = 12
```

Depois do primeiro quantum:

```txt
variável = 12
pc aponta para T
processo volta para a fila de prontos
```

Segundo `E`:

```txt
T -> processo termina
```

Resultado final:

```txt
PID = 1
estado = TERMINATED
variável final = 12
```

---

## 10. Exemplo com criação de processos

### `progenitor.prg`

```txt
M 100
A 19
A 20
S 12
A 1
A 4
C 2
L filho1.prg
C 2
L filho2.prg
T
```

### `filho1.prg`

```txt
M 200
A 19
T
```

### `filho2.prg`

```txt
M 300
S 12
A 5
T
```

Quando o processo encontra:

```txt
C 2
```

acontece isto:

```txt
é criado um processo filho
o filho continua na instrução seguinte
o pai salta 2 instruções a partir do C
```

Resultado esperado:

```txt
PID 1 -> progenitor
PID 2 -> filho que irá carregar filho1.prg
PID 3 -> filho que irá carregar filho2.prg
```

Quando um processo executa:

```txt
L filho1
```

significa que o processo troca o programa atual por `filho1.prg`, assumindo que o leitor acrescenta internamente a extensão se ela não for escrita.

---

## 11. Algoritmos de escalonamento

## 11.1 FCFS

Escolhe o primeiro processo da fila de prontos.

É o primeiro algoritmo a implementar porque é o mais simples.

---

## 11.2 Priority

Escolhe o processo com maior prioridade.

No nosso simulador:

```txt
menor número = maior prioridade
```

Exemplo:

```txt
prioridade 1 executa antes de prioridade 4
```

---

## 11.3 SJF

Escolhe o processo com menor tempo restante estimado.

Como o `pc` é relativo ao início do programa, a estimativa usada é:

```txt
tempo_restante = comprimento - pc
```

Isto é simples e chega para o nosso simulador, mas temos de explicar no relatório que não prevê efeitos futuros de `C`, `L` e `B`.

---

## 11.4 EDF

EDF significa **Earliest Deadline First**.

Escolhe o processo com deadline absoluto mais cedo.

Regra:

```txt
deadline_absoluto = tempo_lancamento + deadline
```

O processo com menor `deadline_absoluto` executa primeiro.

---

## 11.5 Rate Monotonic

Rate Monotonic escolhe o processo com menor período.

Regra:

```txt
menor período = maior prioridade
```

Exemplo:

```txt
periodo 5 executa antes de periodo 10
```

---

## 11.6 Processos periódicos

Para EDF e Rate Monotonic, vamos tratar tarefas periódicas assim:

```txt
cada tarefa tem tempo_chegada, periodo e deadline
quando chega o tempo_chegada, cria-se a primeira instância
em cada novo período, cria-se uma nova instância da mesma tarefa
deadline_absoluto = tempo_lancamento + deadline
```

Exemplo:

```txt
tarefa1.prg 0 1 5 5
```

Significa:

```txt
primeira instância no tempo 0
nova instância nos tempos 5, 10, 15, ...
deadline de cada instância = tempo_lancamento + 5
```

Nas estatísticas finais, devemos indicar se alguma instância falhou a deadline.

---

## 12. Ordem de implementação

```txt
1. Criar repositório Git
2. Criar estrutura de diretorias
3. Criar Makefile
4. Criar configuracao.h
5. Criar instrucao.h
6. Criar processo.h
7. Implementar fila.h e fila.c
8. Implementar leitor.h e leitor.c
9. Criar ficheiros .prg de teste
10. Criar plan.txt
11. Criar control.txt
12. Implementar memoria.h e memoria.c
13. Implementar criação de processos
14. Implementar instruções M, A, S e T
15. Implementar FCFS
16. Implementar relatorio.h e relatorio.c
17. Testar com um programa simples
18. Implementar B e fila de bloqueados
19. Implementar comando I
20. Implementar comando D
21. Implementar C
22. Implementar L
23. Implementar Priority
24. Implementar SJF
25. Implementar EDF
26. Implementar Rate Monotonic
27. Melhorar gestão de memória
28. Melhorar estatísticas finais
29. Fazer testes finais
30. Escrever relatório final
31. Preparar apresentação/defesa
```

Nota:

```txt
Não vale a pena avançar para C, L, EDF ou Rate Monotonic enquanto M, A, S, T, FCFS e relatórios básicos não estiverem a funcionar.
```

---

## 13. Distribuição de tarefas por fases

A ideia é todos mexerem no simulador em si, mas sem estarem todos a alterar os mesmos ficheiros ao mesmo tempo.

A divisão fica organizada por fases, para cada pessoa ter uma parte concreta do código e para ser mais fácil testar tudo por etapas.

---

## Fase 1 — Base do projeto

Responsável: **Eu**

Ficheiros:

```txt
Makefile
src/configuracao.h
src/instrucao.h
src/processo.h
```

Tarefas:

```txt
1. Criar Makefile.
2. Criar constantes globais.
3. Criar struct Instrucao.
4. Criar enum dos estados dos processos.
5. Criar struct PCB.
6. Criar struct InfoPrograma.
7. Criar struct EntradaPlano.
8. Criar headers principais com funções ainda por implementar.
```

Objetivo:

```txt
Ter uma base comum para todos trabalharem.
O projeto deve compilar, mesmo que ainda quase não faça nada.
```

---

## Fase 2 — Filas

Responsável: **Pessoa 2**

Ficheiros:

```txt
src/fila.h
src/fila.c
```

Tarefas:

```txt
1. Definir struct Fila.
2. Implementar inicialização da fila.
3. Implementar verificação de fila vazia.
4. Implementar verificação de fila cheia.
5. Implementar inserir elemento.
6. Implementar remover elemento.
7. Implementar consultar elemento por posição.
8. Implementar remover elemento por posição.
9. Testar a fila isoladamente.
```

Objetivo:

```txt
Ter uma fila funcional para usar em prontos, bloqueados e terminados.
```

---

## Fase 3 — Leitura de ficheiros

Responsável: **Pessoa 3**

Ficheiros:

```txt
src/leitor.h
src/leitor.c
dados/*.prg
dados/plan.txt
dados/control.txt
```

Tarefas:

```txt
1. Implementar leitura de ficheiros .prg.
2. Ler instruções M, A, S, B, T, C e L.
3. Converter cada linha para uma Instrucao.
4. Validar limite de 15 caracteres no nome do ficheiro.
5. Implementar leitura do plan.txt.
6. Aceitar formato básico e formatos alargados.
7. Implementar leitura do control.txt.
8. Ler comandos E, I, R, D e T.
9. Criar ficheiros .prg para testar o simulador.
```

Ficheiros de teste mínimos:

```txt
dados/simples.prg
dados/bloqueio.prg
dados/progenitor.prg
dados/filho1.prg
dados/filho2.prg
dados/plan.txt
dados/control.txt
```

Objetivo:

```txt
Conseguir ler programas, plano e comandos de controlo.
```

---

## Fase 4 — Memória simulada

Responsável: **Pessoa 4**

Ficheiros:

```txt
src/memoria.h
src/memoria.c
```

Tarefas:

```txt
1. Criar memory[1000].
2. Inicializar a memória com instruções vazias.
3. Verificar se um programa já está carregado.
4. Carregar um programa .prg para a memória.
5. Guardar a posição inicial do programa.
6. Guardar o tamanho do programa.
7. Validar se existe espaço livre suficiente.
8. Marcar posições ocupadas.
9. Libertar posições quando um programa já não estiver a ser usado.
10. Criar função para imprimir a memória para debug.
```

Objetivo:

```txt
Guardar programas na memória simulada e gerir espaço livre/ocupado.
```

---

## Fase 5 — Criação de processos

Responsável: **Eu**

Ficheiros:

```txt
src/executor.h
src/executor.c
src/processo.h
```

Tarefas:

```txt
1. Criar tabela de PCBs.
2. Criar contador de processos.
3. Criar geração automática de PID.
4. Implementar criarProcesso.
5. Carregar programa para memória.
6. Preencher o PCB.
7. Definir PC inicial.
8. Definir estado como READY.
9. Meter processo na fila de prontos.
10. Atualizar processos_a_usar no InfoPrograma.
```

Objetivo:

```txt
Conseguir criar um processo a partir de um programa .prg.
```

---

## Fase 6 — Execução básica

Responsável: **Eu**

Ficheiro:

```txt
src/executor.c
```

Tarefas:

```txt
1. Implementar instrução M.
2. Implementar instrução A.
3. Implementar instrução S.
4. Implementar instrução T.
5. Atualizar o PC.
6. Atualizar a variável do processo.
7. Atualizar tempo atual.
8. Atualizar tempo de CPU.
9. Mover processos terminados para a fila de terminados.
```

Teste obrigatório:

```txt
M 10
A 5
S 3
T
```

Resultado esperado:

```txt
variável final = 12
processo terminado
```

---

## Fase 7 — FCFS e quantum

Responsável: **Pessoa 2**

Ficheiros:

```txt
src/escalonador.h
src/escalonador.c
src/executor.c
src/main.c
```

Tarefas:

```txt
1. Implementar FCFS.
2. Escolher o primeiro processo da fila de prontos.
3. Remover esse processo da fila.
4. Executar durante um quantum.
5. Se não terminar nem bloquear, volta para a fila de prontos.
6. Se terminar, vai para a fila de terminados.
```

Nota:

```txt
O escalonador escolhe o processo.
O executor executa o quantum.
O main chama o escalonador e o executor quando aparece o comando E.
```

Objetivo:

```txt
Ter um simulador mínimo funcional.
```

---

## Fase 8 — Relatórios e estatísticas

Responsável: **Pessoa 4**

Ficheiros:

```txt
src/relatorio.h
src/relatorio.c
```

Tarefas:

```txt
1. Imprimir tempo atual.
2. Imprimir processo em execução.
3. Imprimir fila de prontos.
4. Imprimir fila de bloqueados.
5. Imprimir fila de terminados.
6. Imprimir dados principais de cada PCB.
7. Imprimir estatísticas finais básicas.
8. Calcular turnaround time.
```

Objetivo:

```txt
Conseguir perceber o estado do simulador em qualquer momento.
```

---

## Fase 9 — Comandos do `control.txt`

Responsável: **Pessoa 3**

Ficheiros:

```txt
src/leitor.c
src/main.c
dados/control.txt
```

Tarefas:

```txt
1. Ler comando E.
2. Ler comando I.
3. Ler comando R.
4. Ler comando D.
5. Ler comando T.
6. Executar quantum quando aparece E.
7. Chamar a função de interrupção quando aparece I.
8. Imprimir relatório quando aparece R.
9. Chamar a função de desbloqueio quando aparece D.
10. Terminar simulação quando aparece T.
```

Objetivo:

```txt
Controlar a simulação através do ficheiro control.txt.
```

---

## Fase 10 — Bloqueio, interrupção e desbloqueio

Responsável: **Pessoa 2**

Ficheiros:

```txt
src/executor.c
src/fila.c
src/main.c
```

Tarefas:

```txt
1. Implementar instrução B.
2. Quando o processo executa B, muda para BLOCKED.
3. Meter processo na fila de bloqueados.
4. Implementar lógica do comando I.
5. O comando I interrompe um processo e bloqueia-o.
6. Implementar lógica do comando D.
7. O comando D tira o primeiro processo da fila de bloqueados.
8. O processo desbloqueado volta para a fila de prontos.
```

Teste:

```txt
M 50
B
A 10
T
```

Resultado esperado:

```txt
processo bloqueia
com D volta à fila de prontos
termina com variável final = 60
```

---

## Fase 11 — Criação de processos com `C`

Responsável: **Eu**

Ficheiro:

```txt
src/executor.c
```

Tarefas:

```txt
1. Implementar instrução C n.
2. Criar um novo PCB para o filho.
3. Atribuir novo PID ao filho.
4. Definir PPID do filho como PID do pai.
5. O filho continua na instrução seguinte ao C.
6. O pai salta n instruções a partir do C.
7. Meter o filho na fila de prontos.
```

Regra:

```txt
Se pc_c for a posição da instrução C:
    filho.pc = pc_c + 1
    pai.pc = pc_c + n
```

---

## Fase 12 — Carregamento de novo programa com `L`

Responsável: **Pessoa 4**

Ficheiros:

```txt
src/executor.c
src/memoria.c
```

Tarefas:

```txt
1. Implementar instrução L ficheiro.
2. Validar limite de 15 caracteres.
3. Verificar se o programa já está carregado.
4. Se não estiver, carregar para a memória.
5. Atualizar o nome do programa no PCB.
6. Atualizar início do programa no PCB.
7. Atualizar tamanho do programa no PCB.
8. Atualizar PC para o início do novo programa.
9. Atualizar contadores de processos a usar cada programa.
```

Objetivo:

```txt
Simular a ideia de fork seguido de exec.
```

---

## Fase 13 — Escalonadores extra

Responsável: **Pessoa 4**

Ficheiro:

```txt
src/escalonador.c
```

### Priority

```txt
1. Percorrer fila de prontos.
2. Escolher processo com menor valor de prioridade.
3. Remover esse processo da fila.
4. Devolver esse processo para execução.
```

### SJF

```txt
1. Calcular tempo restante estimado.
2. Escolher processo com menor tempo restante.
3. Remover esse processo da fila.
4. Devolver esse processo para execução.
```

Fórmula:

```txt
tempo_restante = comprimento - pc
```

### EDF

```txt
1. Usar deadline absoluto.
2. Escolher processo com deadline absoluto mais cedo.
3. Remover esse processo da fila.
4. Devolver esse processo para execução.
```

### Rate Monotonic

```txt
1. Usar período.
2. Escolher processo com menor período.
3. Remover esse processo da fila.
4. Devolver esse processo para execução.
```

Objetivo:

```txt
Ter todos os algoritmos de escalonamento pedidos no enunciado.
```

---

## Fase 14 — Processos periódicos

Responsável: **Pessoa 3 + Pessoa 4**

Ficheiros:

```txt
src/main.c
src/escalonador.c
src/processo.h
```

Tarefas:

```txt
1. Ler período e deadline do plan.txt.
2. Criar instâncias periódicas nos tempos certos.
3. Calcular deadline absoluto.
4. Guardar deadline absoluto no PCB.
5. Usar deadline absoluto no EDF.
6. Usar período no Rate Monotonic.
7. Registar deadlines cumpridos e falhados.
```

Objetivo:

```txt
Suportar os algoritmos de tempo real pedidos no enunciado.
```

---

## Fase 15 — Testes por módulo

Responsável: **Pessoa 3**

Ficheiros:

```txt
dados/*.prg
dados/plan.txt
dados/control.txt
README.md
```

Tarefas:

```txt
1. Criar teste para programa simples.
2. Criar teste com vários processos.
3. Criar teste para bloqueio.
4. Criar teste para interrupção com I.
5. Criar teste para desbloqueio com D.
6. Criar teste para C.
7. Criar teste para L.
8. Criar teste para C + L.
9. Criar teste para FCFS.
10. Criar teste para Priority.
11. Criar teste para SJF.
12. Criar teste para EDF.
13. Criar teste para Rate Monotonic.
14. Registar os resultados esperados.
```

Objetivo:

```txt
Ter exemplos suficientes para validar o simulador e usar no relatório.
```

---

## Fase 16 — Testes finais

Responsáveis: **Todos**

Tarefas:

```txt
1. Testar programa simples.
2. Testar vários processos.
3. Testar fila de prontos.
4. Testar fila de bloqueados.
5. Testar B, I e D.
6. Testar C.
7. Testar L.
8. Testar C + L.
9. Testar FCFS.
10. Testar Priority.
11. Testar SJF.
12. Testar EDF.
13. Testar Rate Monotonic.
14. Testar estatísticas finais.
15. Corrigir erros encontrados.
```

Objetivo:

```txt
Garantir que o projeto funciona e que todos sabem explicar a sua parte.
```

---

## 14. Regras de Git

Cada elemento deve trabalhar numa branch própria.

Exemplo:

```bash
git checkout -b filas
git checkout -b leitor
git checkout -b memoria
git checkout -b escalonador
```

Commits devem ser pequenos e claros.

Bons exemplos:

```bash
git commit -m "Adicionar estrutura PCB"
git commit -m "Implementar fila circular"
git commit -m "Adicionar leitura de programas prg"
git commit -m "Implementar escalonamento FCFS"
```

Evitar:

```bash
git commit -m "coisas"
git commit -m "update"
git commit -m "final"
```

Regra do grupo:

```txt
Ninguém deve fazer merge para a branch principal sem revisão.
```

Além disso, quem faz commit tem de saber explicar o código que submeteu.

Isto é importante porque na apresentação podem verificar as contribuições de cada elemento.

---

## 15. Versões do projeto

## 15.1 Primeira versão funcional

Prioridade máxima:

```txt
Makefile funcional
leitura de plan.txt
leitura de control.txt
leitura de programas .prg
memória simulada simples
PCB
fila de prontos
execução de M, A, S e T
FCFS
relatório simples
estatísticas finais básicas
```

Esta versão tem de funcionar antes de avançarmos.

---

## 15.2 Segunda versão

Depois da primeira versão estar estável:

```txt
B
I
D
C
L
Priority
SJF
melhor gestão de memória
```

---

## 15.3 Terceira versão

No fim:

```txt
EDF
Rate Monotonic
processos periódicos
estatísticas finais melhoradas
relatório final
apresentação
```