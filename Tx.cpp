/*
  Implementção dos métodos das classes referentes à fila básica e à TX-Ring
  Vide Filas.h
*/

#include <cstdlib>
#include <cstdio>
#include "Pacote.h"
#include "Evento.h"
#include "Lista.h"
#include "Fluxo.h"
#include "Servidor.h"
#include "Filas.h"


using namespace std;
extern int verbose; // declaracao para variavel global

//------------------------------------------------------------------------------
//  TX-RING
//------------------------------------------------------------------------------

TxRing::TxRing() {
	this->head = NULL;
	this->tail = NULL;
}

bool TxRing::podeReceber() {

	// TX Ring fica cheia quando tem dois pacotes

	if ((head != NULL) && (head->getProximo() != NULL)) 
		return false;
	else				
		return true;
}

void TxRing::adicionaPacote(Pacote *pacote) {

	if (!(podeReceber())) {		
		printf ("Tentando inserir na TX cheia\n");
		exit(1);
	} 

	Enqueue(pacote);
	pacotesNaFila++;

	if (verbose >= 3) {
		int i;
		if (pacote == head) 
			i = 1;
		else
			i = 2;
		printf ("TX\tFluxo %d adicionando na TX %d/2\n", pacote->getFluxo()->getId(), i);
		
	}
}

Pacote *TxRing::getProximo() {

	Pacote *proximo;

	if (head == NULL) {
		printf ("Tentando remover da TX vazia!\n");
		exit(1);
	}
 
	proximo = dequeue();
	pacotesNaFila--;

	if (verbose >= 3)
		printf ("TX\tPacote de %d saindo da TX para o servidor\n", proximo->getFluxo()->getId());

	return proximo;
}


