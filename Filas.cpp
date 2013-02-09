/*
  Implementção dos métodos das classes referentes à fila básica e à TX-Ring
  Vide Filas.h
*/

#include <cstdlib>
#include <cstdio>
#include <string>
#include <string.h>
#include <time.h>
#include "Pacote.h"
#include "Evento.h"
#include "Lista.h"
#include "Fluxo.h"
#include "Servidor.h"
#include "Filas.h"


using namespace std;
extern int verbose; // declaracao para variavel global


//------------------------------------------------------------------------------
// CLASSE BASE FILA
//------------------------------------------------------------------------------

void Fila::start () {
	this->head = NULL;
	this->tail = NULL;
}

void Fila::Enqueue(Pacote *pacote) {

	if (head == NULL) 
		head = pacote;
	else 
		tail->setProximo(pacote);

	tail = pacote;      
	pacote->setProximo(NULL);
}
      
Pacote *Fila::dequeue() {

	if (head == NULL) {
		printf ("Erro - Tentando remover elemento de fila vazia\n"); 
		exit(1); 
	}
	else {
		Pacote *pacote;
		pacote = head;

		if (pacote == tail) {	// ultimo	
			tail = NULL;
			head = NULL;
		}
		else
		{
			head = head->getProximo();
		}
		return pacote;
	}
}

bool Fila::vazia() {

	if (head == NULL)
		return true;
	else
		return false;
}

Pacote *Fila::primeiro(){
	return head;
}
