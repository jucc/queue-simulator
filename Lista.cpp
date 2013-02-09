/*
    Implementação da classe lista
    Vide Lista.h
*/

#include <cstdlib>
#include <cstdio>
#include "Pacote.h"
#include "Evento.h"
#include "Lista.h"


using namespace std;
extern int verbose; // declaracao para variavel global


Lista::Lista() {                 
	this->head = NULL;
	this->tail = NULL;   
	this->tempo = -1.0;
}

double Lista::tempoCorrente() {
	return tempo;
}

void Lista::transcorreTempoPara(double tempo) {
	this->tempo = tempo;
}

// devolve o primeiro elemento da lista (menor tempo) e libera seu espaço na memória

Evento Lista::Dequeue() {
	Evento *cp;
	Evento evento;
   
	// cp é o primeiro da Lista
	cp = head;
	if (cp == NULL) {
		printf( "Erro: a Lista está vazia\n");
		exit(-1);
	}   
	// o próximo passa a ser o primeiro da Lista
	head = cp->getProximo();
   
	// copia o conteúdo para um evento e libera o ponteiro   
	evento = *cp;
	free(cp);
	return evento;
}

Evento *Lista::primeiro() {
	return head;
}       

Evento *Lista::ultimo() {
	return tail;
}


// Insere em ordem crescente do tempo do evento
// Empates sao decididos pelo tipo do evento (Saida < Servico < Chegada) 

void Lista::Insere(Evento *novoEvento) {
 
	
	// se a Lista estava vazia

	if (head == NULL) {       
		head = novoEvento;
		tail = novoEvento;
		novoEvento->setProximo(NULL);

		if (verbose >= 5)
			printf ("%lf\tLista\tInserindo primeiro em lista vazia - %lf \n", tempoCorrente(), novoEvento->getTempo());
	}

	// caso contrario, determina posicao

	else {
		Evento *pt1, *pt2;

		pt1 = head;
		pt2 = pt1->getProximo();

		// primeiro da lista ?

		if (pt1->depoisDe(novoEvento)) { 
			
			novoEvento->setProximo(head);
			head = novoEvento;
			if (verbose >= 5)
				printf ("%lf\tLista\tInserindo primeiro - %lf < %lf\n", tempoCorrente(), novoEvento->getTempo(), pt1->getTempo());
		}
		
		// quem vem antes e depois de mim ?
		else { 
			bool encontrouMaior = false;
		
			while ((pt1 != tail) && (encontrouMaior == false))
			{
				if (novoEvento->depoisDe(pt2)) {
					pt1 = pt2;
					pt2 = pt2->getProximo();
				}
				else	
					encontrouMaior = true;			
			}

			// pt1 < novoEvento < pt2
           
			pt1->setProximo(novoEvento);
			novoEvento->setProximo(pt2);	

			// se for o ultimo
			if (pt1 == tail)						
				tail = novoEvento;  
			
			if (verbose >= 5)
				printf ("%lf\tLista\tInserindo evento %lf depois de %lf\n", tempoCorrente(), novoEvento->getTempo(), pt1->getTempo());
			
		}   
	}
}	
