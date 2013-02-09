CPP  = g++	
CC   = gcc	
OBJ  = Pacote.o Evento.o Lista.o Fluxo.o Rodada.o Filas.o Tx.o Wfq.o Pq.o Servidor.o Seletor.o main.o 
LIBS =  -lm
BIN  = Simulador
RM = rm -f 


all: $(BIN)
	
clean:
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(OBJ) -o Simulador $(LIBS) -Wall  -DHAVE_PTHREAD 

Pacote.o: Pacote.cpp
	$(CPP) -c Pacote.cpp -o Pacote.o -Wall -g -DHAVE_PTHREAD

Evento.o: Evento.cpp
	$(CPP) -c Evento.cpp -o Evento.o -Wall -g -DHAVE_PTHREAD

Lista.o: Lista.cpp
	$(CPP) -c Lista.cpp -o Lista.o -Wall -g -DHAVE_PTHREAD

Filas.o: Filas.cpp
	$(CPP) -c Filas.cpp -o Filas.o -Wall -g -DHAVE_PTHREAD

Fluxo.o: Fluxo.cpp
	$(CPP) -c Fluxo.cpp -o Fluxo.o -Wall -g -DHAVE_PTHREAD

Rodada.o: Rodada.cpp
	$(CPP) -c Rodada.cpp -o Rodada.o -Wall -g -DHAVE_PTHREAD

Tx.o: Tx.cpp
	$(CPP) -c Tx.cpp -o Tx.o -Wall -g -DHAVE_PTHREAD

Wfq.o: Wfq.cpp
	$(CPP) -c Wfq.cpp -o Wfq.o -Wall -g -DHAVE_PTHREAD

Pq.o: Pq.cpp
	$(CPP) -c Pq.cpp -o Pq.o -Wall -g -DHAVE_PTHREAD

Seletor.o: Seletor.cpp
	$(CPP) -c Seletor.cpp -o Seletor.o -Wall -g -DHAVE_PTHREAD

Servidor.o: Servidor.cpp
	$(CPP) -c Servidor.cpp -o Servidor.o -Wall -g -DHAVE_PTHREAD
	
main.o: main.cpp
	$(CPP) -c main.cpp -o main.o -Wall -g -DHAVE_PTHREAD
