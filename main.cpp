#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "Pacote.h"
#include "Evento.h"
#include "Lista.h"
#include "Fluxo.h"
#include "Servidor.h"
#include "Filas.h"
#include "Seletor.h"


//TODO: Semente do número aleatório para gerar espaço entre pacotes de dados precisa ser gerada de novo ou pode usar a mesma que usou para espacar o inicio das chamadas de voz?


using namespace std;
int verbose; // declaracao para variavel global


int main(int argc, char *argv[])
{

//-----------------------------------------------------------------------------
// RECEBE PARAMETROS
//-----------------------------------------------------------------------------
	
	// primeiro cria alguns valores default, caso nao sejam chamados

    	int numeroChamadas = 5;
    	int numFluxosTrafegoFundo = 16;
  	unsigned tamanhoRodada = 50000;	
	int cenario = 3;
	unsigned bufferVoz = 10000;
	verbose = 0;    	// 0 = so as estatisticas finais 
				// 1 = estatisticas por rodada
				// 2 = estatisticas + avisos da main (cada evento gera um)
				// 3 = estatisticas + avisos da main + avisos de insercao em filas
				// 4 = estatisticas + avisos da main + avisos de insercao + aviso de geracao de eventos e insercao na lista


	// depois parseia a linha de comando	

	int parm;
	while ((parm = getopt (argc, argv, "n:f:r:t:m:b:v:h")) != -1) {
        	switch (parm)
        	{
           		case 'n': numeroChamadas = atoi(optarg);
			break;	
			case 'f': numFluxosTrafegoFundo = atoi(optarg);
			break;
			case 't': tamanhoRodada = atoi(optarg);
			break;
			case 'm': cenario = atoi(optarg);
			break;
			case 'b': bufferVoz = atoi(optarg);
			break;
			case 'v': verbose = atoi(optarg);
			break;
			case 'h': printf
		("Modo de uso: \n\
		Simulador \n\
		[-n numero de chamadas de voz]\n\
		[-f numero de fluxos do trafego de fundo]\n\
		[-t tamanho da rodada em eventos concluidos por chamada]\n\
		[-m modo de simulacao (1 - 3)]\n\
		[-b tamanho do buffer para os pacotes de voz (em bytes)]\n\
		[-v verbose (0  = somente resumo, 1 = resumo por chamada, 2 = exibe eventos, 3 = exibe eventos detalhados, 4 e 5 = debug)]\n");
			exit(1);
			break;
			default : printf("Opcao invalida\nUse Simulador -h para ver as opcoes\n");
			exit(1); 
		}
	}

	// e confere se estao todos no limite certo
	
	if (numeroChamadas < 1) numeroChamadas = 1;
	if (numFluxosTrafegoFundo < 0) numFluxosTrafegoFundo = 0;
	if ((cenario < 1) || (cenario > 3)) cenario = 3;	
	if (tamanhoRodada < 10) tamanhoRodada = 10;


//-----------------------------------------------------------------------------
// INICIALIZACAO DE VARIAVEIS
//-----------------------------------------------------------------------------

// variáveis principais  

	Lista listaEventos = Lista(); 
	Fluxo *fluxos;
	Fluxo **refFluxos;
	Rodada *rodadas;
	Servidor servidor;
	GlobalWfq wfq;
	PQ pq;
	TxRing tx;
	Seletor seletor;
	Balde balde;


// variáveis auxiliares        

	int i = 0;
	unsigned long semente = time(NULL);
	int numFluxos = numeroChamadas + numFluxosTrafegoFundo;
	int *prioridades = (int *) malloc(numFluxos * sizeof(int));
        unsigned *buffers = (unsigned *) malloc(numFluxos * sizeof(unsigned));
	unsigned rodadaAtual = 0; 			// a rodada 0 é a fase transiente!
	IC ic;

	rodadas = (Rodada *) malloc(100 * sizeof(Rodada));
	
// inicializa estruturas de dados	

	seletor.setWfq(&wfq);
	seletor.setPq(&pq);
	seletor.setTx(&tx);
	seletor.setListaEventos(&listaEventos);
	seletor.setServidor(&servidor);
	seletor.setCenario(cenario);
	servidor.setLista(&listaEventos);
	pq.setBalde(&balde);
	pq.setMaxBuffer(bufferVoz);
		

// inicializa wfq de acordo com cenario

	for (i = 0; i < numeroChamadas ; i++) {
		if (cenario == 2)
			prioridades[i] = 5;
		else
			prioridades[i] = 0;
	}
	for (i = numeroChamadas; i < numeroChamadas + numFluxosTrafegoFundo; i++) {
		prioridades[i] = 0;
	}		

        for (i = 0; i < numeroChamadas ; i++) {
                buffers[i] = bufferVoz;
        }
        for (i = numeroChamadas; i < numeroChamadas + numFluxosTrafegoFundo; i++) {
                buffers[i] = 10000;
        }

	
	wfq = GlobalWfq(numFluxos, prioridades, buffers);	
	
	
//-----------------------------------------------------------------------------
// INICIA SIMULACAO
//----------------------------------------------------------------------------
   

// inicia todas as chamadas de voz 

	srand(semente);

	fluxos = (Fluxo *)malloc((numeroChamadas + numFluxosTrafegoFundo) * sizeof(Fluxo));

	refFluxos = (Fluxo **)malloc((numeroChamadas + numFluxosTrafegoFundo) * sizeof(Fluxo *));

	for (i = 0; i < numeroChamadas ; i++)
	{	        
		fluxos[i] = Fluxo(i, FLUXO_VOZ);
		fluxos[i].setLista(&listaEventos);
		fluxos[i].setRodada(0);
		fluxos[i].geraPacote();
		refFluxos[i] = &fluxos[i];
	}

// inicia todos os fluxos do tráfego de fundo 
	
	for  (i = numeroChamadas; i < numeroChamadas + numFluxosTrafegoFundo; i++)      
	{
		fluxos[i] = Fluxo(i, FLUXO_DADOS);      
		fluxos[i].setLista(&listaEventos);
		fluxos[i].setRodada(0);
		fluxos[i].geraPacote();
		refFluxos[i] = &fluxos[i];
	}    

       	rodadas[0] = Rodada(0, 0.0, tamanhoRodada, numeroChamadas, numFluxosTrafegoFundo, refFluxos);

//-----------------------------------------------------------------------------
// MAIN LOOP         
//-----------------------------------------------------------------------------

	servidor.setOcioso(true);	
	bool fim = false;

	do {
		Evento evento;	
		double tempo;
                               
	// 1 - incrementa o tempo até o próximo evento ------------------------

		evento = listaEventos.Dequeue();
		tempo = evento.getTempo();
		listaEventos.transcorreTempoPara(tempo);        

	// 2 - Atualiza a chegada de tokens
		
		if (cenario == 1)
		{
			balde.adicionaTokens( 100.0 * (tempo - balde.getUltimoRefil()));			
			balde.setUltimoRefil(tempo);
		}		

	// 3 - verifica o tipo de evento e toma as acoes necessarias ----------
		       
		// 3a - se for uma chegada ------------------------------------

		if (evento.getTipoEvento() == EV_CHEGADA) 
		{	
			if (verbose >= 2) 
				printf("%.03lf\tMain\tChegada do fluxo %d\n", tempo, evento.getPacote()->getFluxo()->getId());

			// gera a proxima chegada desse fluxo e coloca na lista	
		
			int idFluxo = evento.getPacote()->getFluxo()->getId();
			fluxos[idFluxo].geraPacote();
					
			// decide para onde enviar o pacote

			seletor.encaminhaPacote(evento.getPacote());
		}		    

		
		// 3b - se for uma saida --------------------------------------

		else if (evento.getTipoEvento() == EV_SAIDA)         
		{
			if (verbose >= 2) 
				printf ("%.03lf\tMain\tPartida do fluxo %d\n", tempo, evento.getPacote()->getFluxo()->getId());
			
			evento.getPacote()->setSaida(tempo);		
					
			// se a cor do pacote é a cor da rodada atual, acrescenta nas estatísticas
			if (evento.getPacote()->getRodada() == rodadaAtual)
				evento.getPacote()->atualizaEstatisticas();
			else
				if (verbose >= 5) printf ("%.03lf\tMain\tEvento do fluxo %u gerado na rodada %u descartado por cor\n", tempo,  evento.getPacote()->getFluxo()->getId(), rodadaAtual);
	
			evento.getPacote()->getFluxo()->ultimaSaida = tempo;
			
			// verifica se acabou a rodada ou o transiente (rodada 0)		
							
			if (rodadas[rodadaAtual].acabou(wfq.pacotesVoz() + pq.pacotesNaFila, verbose, tempo)) 
			{			
				if (rodadaAtual >= 30)
				{
					ic.calcula(rodadas, rodadaAtual);
					if ((ic.tamanhoPerda < 0.1 * ic.centroPerda) || (ic.tamanhoPerda == 0)) 
						fim = true;
				}

				rodadaAtual++;				

				if (!fim)
				{					
					srand(semente * rodadaAtual * (rodadaAtual + 1));
					rodadas[rodadaAtual] = Rodada(rodadaAtual, tempo, tamanhoRodada, numeroChamadas, numFluxosTrafegoFundo, refFluxos);
				}
			}
			
			// libera o pacote

			free(evento.getPacote());
			
			// se tiver mais pacotes prontos na TX, manda para o serviço
			
			if (!(tx.vazia())) 	
			{
				servidor.servico(tx.getProximo());

				// repoe o slot vago da TX-Ring
				seletor.repoeSlotTx();					
			}
			// senão, fica ocioso
			else
			{	
				servidor.setOcioso(true);
				if (verbose >= 2)
					printf ("%.03lf\tMain\tServidor entrando em modo ocioso até a próxima chegada\n", tempo);
			}
		}		
	} while (!fim);	
	
	
	ic.exibe();	
}

	

