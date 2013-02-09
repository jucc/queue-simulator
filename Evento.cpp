/* 
   Implementação dos métodos da classe Evento
   Vide Evento.h
*/

#include <cstdlib>
#include <iostream>
#include "Pacote.h"
#include "Evento.h"
#include "Lista.h"


using namespace std;

extern int verbose; // declaracao para variavel global

Evento::Evento() { }

double Evento::getTempo() {
     return this->tempo;
}
void Evento::setTempo(double tempo) {
     this->tempo = tempo;
}
int Evento::getTipoEvento() {
     return this->tipoEvento;
}
void Evento::setTipoEvento(int tipoEvento) {
     this->tipoEvento = tipoEvento;
}
Pacote *Evento::getPacote() {
     return this->pacote;  
}
void Evento::setPacote(Pacote *pacote) {
     this->pacote = pacote;
}
Evento *Evento::getProximo() {
     return this->proximo;  
}
void Evento::setProximo(Evento *evento) {
     this->proximo = evento;
}   

// determina se um evento vem depois de outro na lista de eventos

bool Evento::depoisDe(Evento *outro) {
	
	if (outro == NULL)
		return false;

	if (this->getTempo() > outro->getTempo())
		return true;
	
	if (this->getTempo() < outro->getTempo())
		return false;
	
	// se nao é maior nem menor, é igual (empate)
	// com empate no tempo, a prioridade vem do tipo de evento

	if (this->getTipoEvento() == EV_SAIDA)
		return false;			// eventos de saida vem 1o
	else 	
		return true; 			// chegada vem por ultimo
						// se os dois forem chegadas, tanto faz
	
}
