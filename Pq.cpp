/*
  Implementção dos métodos das classes referentes a Balde e PQ
  Vide Filas.h
*/


#include <cstdlib>
#include <cstdio>
#include <math.h>
#include "Pacote.h"
#include "Evento.h"
#include "Lista.h"
#include "Fluxo.h"
#include "Servidor.h"
#include "Filas.h"


using namespace std;
extern int verbose; // declaracao para variavel global


//-----------------------------------------------------------------------------
// PQ
//-----------------------------------------------------------------------------

PQ::PQ() { 
	this->head = NULL;
	this->tail = NULL;
	this->ocupacaoBuffer = 0;
	this->pacotesNaFila = 0;
}

void PQ::setBalde(Balde *balde) {
	this->balde = balde;
}

void PQ::setOcupacaoBuffer(unsigned ocupacaoBuffer) {
	this->ocupacaoBuffer = ocupacaoBuffer;
}   

void PQ::setMaxBuffer(unsigned buffer) {
	this->maxBuffer = buffer;
}   

bool PQ::podeReceber(Pacote *pacote) {

	// falta espaco no buffer? 

	if (ocupacaoBuffer + pacote->getTamanho() > maxBuffer) 
		return false; 
	
	else
		// falta tokens no balde? 

		if (balde->getTokensRestantes() < (unsigned)(pacote->getTamanho())) 
			return false;	
		else	
			return true;      
}  

void PQ::adicionaPacote(Pacote *pacote) {

	// retira os tokens do balde
	balde->removeTokens(pacote->getTamanho());	
	if (verbose >= 5)    
		printf ("PQ\tRemovendo %d tokens do balde\n", pacote->getTamanho());

	// coloca na fila
	Enqueue(pacote);	

	// atualiza o estado
	ocupacaoBuffer += pacote->getTamanho();	
	pacotesNaFila++;

	if (verbose >= 3)	
		printf ("PQ\tfluxo %u adicionando na PQ (%u/%u) - balde -> %u\n", pacote->getFluxo()->getId(), ocupacaoBuffer, maxBuffer, balde->getTokensRestantes());
}


Pacote *PQ::getProximo() {

	Pacote *pacote = dequeue();

	ocupacaoBuffer -= pacote->getTamanho();	
	pacotesNaFila--;

	return pacote;
}

//-----------------------------------------------------------------------------
// Balde
//-----------------------------------------------------------------------------


Balde::Balde() 
{
	this->tokensRestantes = MAX_TOKENS ;
	this->ultimoRefil = 0;
}


unsigned Balde::getTokensRestantes() {
	return (unsigned)floor(tokensRestantes);
}

void Balde::adicionaTokens(double tokens) {

	if (verbose >= 5)
		printf ("Adicionando %lf tokens ao balde -> de %lf para ", tokens, this->tokensRestantes);
	
	if (tokensRestantes + tokens > MAX_TOKENS)
		tokensRestantes = MAX_TOKENS;
	else
	{ 
		tokensRestantes += tokens;			
	}

	if (verbose >= 4)
		printf ("%lf\n", tokensRestantes);

}

void Balde::removeTokens(unsigned tokens) {
	
	if (floor(tokensRestantes) - tokens < 0) {
		printf ("Erro : Tentando remover mais tokens do que o balde possui\n");
		exit(1);
	}
	else
		tokensRestantes -= (double)tokens;
}

double Balde::getUltimoRefil() {
	return ultimoRefil;
}

void Balde::setUltimoRefil(double t) {
	ultimoRefil = t;
}
