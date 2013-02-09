/* 
   Implementação dos métodos da classe Servidor
   Vide Fluxo.h
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

Servidor::Servidor() { }

bool Servidor::getOcioso() { 
	return ocioso; 
}
void Servidor::setOcioso(bool ocioso) { 
	this->ocioso = ocioso; 
}
void Servidor::setLista(Lista *lista) { 
	listaEventos = lista; 
}

void Servidor::servico(Pacote *pacote) { 

	double tempoInicio = listaEventos->tempoCorrente();
	double duracaoServico;

	// atualiza seu estado
	setOcioso(false);

	// atualiza o pacote informando o inico de serviço
	pacote->setInicioServico(tempoInicio);

	// determina o tempo do final do serviço
	// multiplica por 1000 para obter milissegundos no lugar de segundos

	duracaoServico = (double)(pacote->getTamanho() * 8 / (double)LARGURA_BANDA) * 1000.0;
	
	if (verbose >=4)
		printf ("%lf\tServidor\tServindo fluxo %d (ate %lf)\n", tempoInicio, pacote->getFluxo()->getId(), tempoInicio + duracaoServico);

	// gera evento de final de serviço

	Evento *novoEvento;
	novoEvento = (Evento *)malloc(sizeof(Evento));   
	novoEvento->setTempo(tempoInicio + duracaoServico);
	novoEvento->setPacote(pacote);
	novoEvento->setProximo(NULL);
	novoEvento->setTipoEvento(EV_SAIDA);
      	
	// insere o evento na lista de eventos

	listaEventos->Insere(novoEvento);
}


