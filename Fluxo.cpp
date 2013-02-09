/* 
   Implementação dos métodos da classe Fluxo e de suas derivadas
   Vide Fluxo.h
*/

//TODO: Por que nao consigo declarar a funcao geraPacote como virtual e extender nas derivadas??

#include <cstdlib>
#include <cstdio>
#include <math.h>
#include "Pacote.h"
#include "Evento.h"
#include "Lista.h"
#include "Fluxo.h"


using namespace std;
extern int verbose; // declaracao para variavel global


// -----------------------------------------------------------------------------
// CLASSE BASE FLUXO 
//------------------------------------------------------------------------------

Fluxo::Fluxo() { 
}
int Fluxo::getId() {
	return id;
}
int Fluxo::getTipo() {
	return id;
}
unsigned long Fluxo::getPacotesGerados() {
	return pacotesGerados;
}
unsigned long Fluxo::getPacotesPerdidos() {
	return pacotesPerdidos;
}
unsigned long Fluxo::getPacotesConcluidos() {
	return pacotesConcluidos;
}
double Fluxo::getTempoGasto() {
	return somaTempoGasto;
}
double Fluxo::getJitter() {
	return somaJitter;
}

void Fluxo::setLista(Lista *lista) {
	this->listaEventos = lista;
}
void Fluxo::setRodada(unsigned rodada) {
	this->rodada = rodada;
}

void Fluxo::perdaDePacote() {
	pacotesPerdidos++;
	flagJitter = true;
	if (verbose >= 2)
		printf ("%.03lf\tFluxo\tPerda de pacote do fluxo %u\n", listaEventos->tempoCorrente(), id);
}

void Fluxo::concluiPacote() {
	pacotesConcluidos++;        
}

void Fluxo::acumulaTempoGasto(double tempo) {
	somaTempoGasto += tempo;  	      
}

void Fluxo::acumulaJitter(double tempo) {
	// printf ("%lf\tFluxo\tFluxo %u jitter %lf\n", listaEventos->tempoCorrente(), id, tempo);
	somaJitter += tempo;        
}

void Fluxo::zeraEstatisticas() {
	pacotesGerados = 0;
	pacotesPerdidos = 0;
	pacotesConcluidos = 0;
	somaTempoGasto = 0;
	somaJitter = 0;
}

Fluxo::Fluxo(int id, int tipo) {                             
	this->id = id;   
	this->ultimo = NULL;   
	this->ultimaSaida = 0;
	this->zeraEstatisticas();
}

/* Essa função gera um pacote, 
               gera um evento para a chegada do pacote,
               insere o evento no futuro da lista de eventos 
*/

Pacote *Fluxo::geraPacote() {
 
	Pacote *novoPacote;
	Evento *novoEvento;
	double chegada;	

	// determina o tempo de chegada do pacote
	if (tipo == FLUXO_VOZ)
		chegada = proximaChegadaVoz();  
	else
                chegada = proximaChegadaDados();        
	
	// cria o pacote
	novoPacote = (Pacote *)malloc(sizeof(Pacote));    
	novoPacote->setChegada(chegada);
	novoPacote->setSaida(-1);
	novoPacote->setInicioServico(-1);
	novoPacote->setFluxo(this);
	if (tipo == FLUXO_VOZ) {
		novoPacote->setTipoPacote(PCT_VOZ);  
		novoPacote->setTamanho(TAM_PCT_VOZ);      
	}
	else
	{
		novoPacote->setTipoPacote(PCT_DADOS);
		novoPacote->setTamanho(TAM_PCT_DADOS);
	}
	novoPacote->setRodada(rodada); 
	novoPacote->flagJitter = flagJitter;	// o ultimo foi descartado
	ultimo = novoPacote;  
	flagJitter = false;			// volta ao status normal

	if (verbose >= 4) 
		printf ("%lf\tFluxo\tFluxo de Voz %d gerando evento de chegada em %lf\n", listaEventos->tempoCorrente(), getId(), chegada);
              
	// gera o evento de chegada do pacote
	novoEvento = (Evento *)malloc(sizeof(Evento));   
	novoEvento->setTempo(chegada);
	novoEvento->setPacote(novoPacote);
	novoEvento->setProximo(NULL);
	novoEvento->setTipoEvento(EV_CHEGADA);
       
	// insere o evento na lista de eventos
	listaEventos->Insere(novoEvento);
                            
	return novoPacote; 
}

double Fluxo::proximaChegadaVoz() {
 
	double t;
	// se for o primeiro pacote do fluxo, o tempo deve ser aleatório entre 0 e 30ms      
	if (ultimo == NULL) {
		t = (double)(rand()%30000)/1000;
		pacotesGerados = 0;
	}
	// caso contrário, basta adicionar 30ms ao último
	else {                                        
		t = ultimo->getChegada() + 30.0 ;
		pacotesGerados++;
	}             
	return t;
}

double Fluxo::proximaChegadaDados() {
      
	double t, u0, x0;
        
	// primeira chegada do fluxo em t = 0 não perde generalização
	if (ultimo == NULL) {
		t= 0.0;
	}
	// próximas chegadas chegam depois de uma amostra da exponencial
	else  {     
		t = ultimo->getChegada();		
		u0 = 1;       
		do {
			u0 = (double)(rand() / (double)RAND_MAX); 		
		} while (u0 == 1);
		x0 = ( log(1 - u0) / LAMBDA ) * (-1); 
		// printf ("%lf\n", x0);              
		t += x0;

		//para gerar estatísticas  
		pacotesGerados++;        
       }   
                
       return t;          
}
