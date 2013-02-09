/*
  Implementção dos métodos das classes referentes à WFQ e suas subfilas 
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
// WFQ SUBFILA
//------------------------------------------------------------------------------

void SubWfq::setCiclo(unsigned long long ciclo) {
	this->ciclo = ciclo;
} 
void SubWfq::setPrioridade(int prioridade) {
	this->prioridade = prioridade;
}   

void SubWfq::setOcupacaoBuffer(unsigned ocupacaoBuffer) {
	this->ocupacaoBuffer = ocupacaoBuffer;
}   

void SubWfq::setMaxBuffer(unsigned buffer) {
        this->maxBuffer = buffer;
}   


bool SubWfq::podeReceber(Pacote *pacote) {

	if (ocupacaoBuffer + pacote->getTamanho() > maxBuffer)
		return false;
	else
		return true;      
}  

Pacote *SubWfq::getProximo() {

	Pacote *pacote;

	if (vazia()) {
		printf ("Erro - Tentando remover pacote de fila vazia!\n");
		exit(1);
	}

	// retira da fila
	pacote = dequeue();

	// atualiza buffer
	ocupacaoBuffer -= pacote->getTamanho();
	pacotesNaFila--;

	return pacote;	
}
 
void SubWfq::adicionaPacote(Pacote *pacote) {
         
	// gera o SN do Pacote 

	unsigned long long novoSN ;
	unsigned peso = 32768 / (this->prioridade + 1);
	
	if (vazia()) 
	{ 				
		novoSN = ciclo + peso * pacote->getTamanho();
	}
	else 
	{
		novoSN = tail->getSn() + peso * pacote->getTamanho();
		
	}

	pacote->setSn(novoSN);

	// coloca na fila
	Enqueue(pacote);	

	// atualiza a ocupacao do buffer
	ocupacaoBuffer += pacote->getTamanho();	
	pacotesNaFila++;

	if (verbose >= 3)	
		printf ("WFQ\tfluxo %u adicionando na WFQ (%u/%u) no ciclo %llu com SN = %llu\n", pacote->getFluxo()->getId(), ocupacaoBuffer, maxBuffer, ciclo, novoSN);
}

//------------------------------------------------------------------------------
// WFQ 
//------------------------------------------------------------------------------

GlobalWfq::GlobalWfq(){ }

GlobalWfq::GlobalWfq(int numFilas, int *prioridades, unsigned *buffers) {

	int i;
	this->ciclo = 0;
	this->numFilas = numFilas;
	this->filas = (SubWfq **) malloc(numFilas * sizeof(SubWfq *));
	for (i = 0; i < numFilas; i++) {
	        printf ("Criando fila\n");         
		SubWfq *fila;
		fila = (SubWfq *) malloc(sizeof(SubWfq));
		fila->setPrioridade(prioridades[i]);
		fila->setCiclo(0);
		fila->setOcupacaoBuffer(0);
		fila->setMaxBuffer(buffers[i]);
		fila->start();
		printf ("Adicionando fila\n");         
		this->filas[i] = fila;		
	}
	this->ultimoServido = 0;	
}

void GlobalWfq::setCiclo(unsigned long long ciclo) {
	int i;

	this->ciclo = ciclo;
	for (i = 0; i < numFilas; i++) {
		filas[i]->setCiclo(ciclo);
	}
}     

bool GlobalWfq::vazia() {
	int i;
	bool teste = true;

	for (i = 0; i < numFilas; i++) {
		if (!(filas[i]->vazia()))
			teste = false;
	}
	return teste;
}

bool GlobalWfq::podeReceber(Pacote *pacote)
{
	int i = pacote->getFluxo()->getId();
	if (filas[i]->podeReceber(pacote))
		return true;
	else
		return false;
}

void GlobalWfq::adicionaPacote(Pacote *pacote)
{
	int i = pacote->getFluxo()->getId();
	filas[i]->adicionaPacote(pacote);	
}

Pacote *GlobalWfq::getProximo() {

	int i;
	SubWfq *proxima;
	Pacote *selecionado = NULL;

	
	for (i = 0; i < numFilas; i++) {
		if (!(filas[i]->vazia())) {
			Pacote *sni = filas[i]->primeiro();
			// e ve se e o minimo de todas
			if ( (selecionado == NULL) || ( sni->antesDe(selecionado)) )
			{				
				selecionado = sni;		
				proxima = filas[i];
			}	
		}
	}	
	
	// atualiza wfq

	proxima->getProximo();

	if (verbose >= 5 && ciclo != selecionado->getSn())  printf ("WFQ\tAtualizando Ciclo para %llu (fluxo %d)\n", ciclo, selecionado->getFluxo()->getId());

	setCiclo(selecionado->getSn());	
		
	return selecionado;
}

unsigned GlobalWfq::pacotes() {

	unsigned resultado = 0;
	int i;

	for (i = 0; i < numFilas; i++) 
		resultado += filas[i]->pacotesNaFila;
	
	return resultado;
}

unsigned GlobalWfq::pacotesVoz() {

	unsigned resultado = 0;
	int i;

	for (i = 0; i < numFilas; i++) 
		if ( !(filas[i]->vazia()) &&  (filas[i]->primeiro()->getTipoPacote() == PCT_VOZ) ) 					
			resultado += filas[i]->pacotesNaFila;		
	
	return resultado;
}
