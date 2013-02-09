/* 
   Implementação dos métodos da classe Seletor 
   Vide Seletor.h
*/

#include <cstdlib>
#include <cstdio>
#include "Pacote.h"
#include "Evento.h"
#include "Lista.h"
#include "Fluxo.h"
#include "Servidor.h"
#include "Filas.h"
#include "Seletor.h"

using namespace std;
extern int verbose; // declaracao para variavel global

Seletor::Seletor() { }

Pacote *Seletor::getPacoteAtual() { 
	return pacoteAtual;
}
void Seletor::setPacoteAtual(Pacote *pacote) {
	pacoteAtual = pacote;
} 		
void Seletor::setWfq(GlobalWfq *wfq) {
	this->wfq = wfq;
} 		
void Seletor::setPq(PQ *pq) {
	this->pq = pq;
} 		
void Seletor::setTx(TxRing *tx) {
	this->tx = tx;
} 		
void Seletor::setListaEventos(Lista *lista) {
	listaEventos = lista;
} 	 
void Seletor::setServidor(Servidor *servidor) {
	this->servidor = servidor;
} 	 	
void Seletor::setCenario(int cenario) {
	modoSimulacao = cenario;
} 	



// 1 -  Interface pacote - filas


// verifica se o pacote pode ir direto ao servidor ou à TX ou se vai para as filas
void Seletor::encaminhaPacote(Pacote *pacote) { 

	pacoteAtual = pacote;

	// Se o servidor está ocioso e a TX vazia, manda direto para ele

	if ( servidor->getOcioso() && tx->vazia() )
	{	
		servidor->servico(pacoteAtual);		
	}
	else
	{
		// Se está ocupado, mas a TX tem vaga e não tem ninguém nas filas, deixa na TX

		if ( tx->podeReceber() && pq->vazia() && wfq->vazia() ) 
			tx->adicionaPacote(pacoteAtual);
	
		// Caso contrário (servidor e TX ocupados), direciona para a fila correta

		else 
			enfileiraPacote();
	}			
		
}	

// determina a fila para o pacote
void Seletor::enfileiraPacote() { 

	// tráfegos de dados vão para a wfq, cada um em uma fila numerada

	if (pacoteAtual->getTipoPacote() == PCT_DADOS)			
		adicionaWfq();
				
	// tráfegos de voz

	else 
	{
		// Cenario 1 tenta a PQ antes		
		if (modoSimulacao == 1)
		{
			// se a pq tem tokens e espaço no buffer, insere nela

			if (pq->podeReceber(pacoteAtual)) 
				pq->adicionaPacote(pacoteAtual);
		
			// caso contrário, tenta inserir na wfq
			else	
			{		
				if (verbose >= 3)
					printf ("%lf\tSeletor\tSem espaco na PQ, passando pacote do fluxo %d para WFQ\n", listaEventos->tempoCorrente(), pacoteAtual->getFluxo()->getId());

				adicionaWfq(); 				
			}
		}
		
		// Cenarios 2 e 3 só têm WFQ
		else
			adicionaWfq();	
	}
}
		

void Seletor::adicionaWfq() {	

	// se tem espaço no buffer, adiciona
			
	if (wfq->podeReceber(pacoteAtual)) 
		wfq->adicionaPacote(pacoteAtual);

	// se falta espaço no buffer, perdeu o pacote =(
	else 
	{
		// avisa o fluxo sobre a perda e libera o pacote da memória
		pacoteAtual->getFluxo()->perdaDePacote();	
		free(pacoteAtual);
	}
}


// 2 - Interface PQ/WFQ - TX

void Seletor::repoeSlotTx() { 

	Pacote *proximo = NULL;
	
	// se a PQ não está vazia, retorna o primeiro pacote da PQ
		
	if (!(pq->vazia())) {
		proximo = pq->getProximo();
		if (verbose >= 4) printf("%.03lf\tSeletor\tfluxo %d enviando da PQ para a TX\n", listaEventos->tempoCorrente(), proximo->getFluxo()->getId());
	}		
	// se a PQ esta vazia, serve na wfq a fila com menor SN de todas	
	else 
	{
		if (!(wfq->vazia())) {
			proximo = wfq->getProximo();
			if (verbose >= 4) printf("%.03lf\tSeletor\tfluxo %d enviando da WFQ para a TX\n", listaEventos->tempoCorrente(), proximo->getFluxo()->getId());
		}

		//nao tem pacote em fila nenhuma, aproveita para limpar o ciclo da wfq	
		else 
			wfq->setCiclo(0); 		
		
	}		

	if (proximo != NULL) 
		tx->adicionaPacote(proximo);		
}
