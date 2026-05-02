COMPILER = cc
FLAGS = -c -Wall
LIBS = -lm

OBS = src/main.o src/fila.o src/leitor.o src/memoria.o src/escalonador.o src/executor.o src/relatorio.o src/processo.o

all: projeto

src/main.o: src/main.c
	$(COMPILER) $(FLAGS) src/main.c -o src/main.o

src/fila.o: src/fila.c
	$(COMPILER) $(FLAGS) src/fila.c -o src/fila.o

src/leitor.o: src/leitor.c
	$(COMPILER) $(FLAGS) src/leitor.c -o src/leitor.o

src/memoria.o: src/memoria.c
	$(COMPILER) $(FLAGS) src/memoria.c -o src/memoria.o

src/escalonador.o: src/escalonador.c
	$(COMPILER) $(FLAGS) src/escalonador.c -o src/escalonador.o

src/executor.o: src/executor.c
	$(COMPILER) $(FLAGS) src/executor.c -o src/executor.o

src/relatorio.o: src/relatorio.c
	$(COMPILER) $(FLAGS) src/relatorio.c -o src/relatorio.o

src/processo.o: src/processo.c
	$(COMPILER) $(FLAGS) src/processo.c -o src/processo.o

projeto: $(OBS)
	$(COMPILER) -o projeto $(OBS) $(LIBS)

clean:
	rm -f src/*.o projeto

run: projeto
	./projeto dados/plan.txt dados/control.txt fcfs