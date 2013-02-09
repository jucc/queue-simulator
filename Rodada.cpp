/* 
   Implementação da classe estatística
   Criada como classe apenas para simplificar a main
*/


#include <cstdlib>
#include <cstdio>
#include <math.h>
#include "Pacote.h"
#include "Evento.h"
#include "Lista.h"
#include "Fluxo.h"
#include "Filas.h"
#include "Servidor.h"


Rodada::Rodada()
{
	this->tamanhoRodada = 0;
} 

Rodada::Rodada(unsigned id, double inicio, unsigned tamanhoRodada, unsigned numeroChamadas, unsigned numFluxosTrafegoFundo, Fluxo **fluxos)
{
	this->id = id;
	this->inicio = inicio;
	this->tamanhoRodada = tamanhoRodada;
	this->numeroChamadas = numeroChamadas;
	this->numFluxosTrafegoFundo = numFluxosTrafegoFundo;
	this->fluxos = fluxos;

	this->pacotesGerados = 0;
	this->pacotesPerdidos = 0; 
	this->pacotesConcluidos = 0;
	this->tempoTotal = 0;	
	this->tempoTotal = 0;	
	this->mediaTempoUltimoConjunto = 0;
	this->pctsFilaUltimoConjunto = 0;

	unsigned i;		
	for(i = 0; i < numeroChamadas + numFluxosTrafegoFundo; i++)
	{
		fluxos[i]->zeraEstatisticas();
		fluxos[i]->setRodada(id);		
	}
} 


bool Rodada::acabouTransiente(unsigned pacotesVozNasFilas)
{	
	unsigned i = 0;

	// se alguma chamada não tiver concluido o mínimo de pacotes para completar um conjunto, retorna falso

	for (i = 0; i < numeroChamadas; i++)	
		if (fluxos[i]->getPacotesConcluidos() < TAM_CONJUNTO_TRANSIENTE)
			return false;

	// se não retornou falso, o conjunto está completo. Calcula a média do tempo gasto pelas chamadas nesse cojnunto e zera as estatísticas das chamadas 
			
	double totTempoConjunto = 0;
	double mediaTempoConjuntoAtual = 0;	
	unsigned long pacotesConcluidosConjunto = 0;	
	unsigned pctsFilaConjuntoAtual = 0;

	for (i = 0; i < numeroChamadas; i++)		
	{
		totTempoConjunto += fluxos[i]->getTempoGasto(); 
		pacotesConcluidosConjunto += fluxos[i]->getPacotesConcluidos();		
		fluxos[i]->zeraEstatisticas();
	}

	mediaTempoConjuntoAtual = totTempoConjunto / pacotesConcluidosConjunto;			
	pctsFilaConjuntoAtual = pacotesVozNasFilas;

	// se a diferença é maior que 5%, continua o transiente e inicia outro conjunto de medidas
	bool mediaTempoInstavel = fabs(mediaTempoConjuntoAtual - mediaTempoUltimoConjunto) >  0.1 * mediaTempoUltimoConjunto;
	if ( mediaTempoInstavel) 
	{
		mediaTempoUltimoConjunto = mediaTempoConjuntoAtual;
		pctsFilaUltimoConjunto = pctsFilaConjuntoAtual;		
		return false;
	}		
	else		
	// se o sistema está saturado, então aumenta a certeza do equilíbrio com a medida de pacotes nas filas
/*	{		
		unsigned taxaEntrada = (numeroChamadas * 80000) + (numFluxosTrafegoFundo * 100000);
		unsigned taxaSaida = LARGURA_BANDA;

		if (taxaEntrada > taxaSaida)
		{	
			if (mediaPacotesInstavel)
			{
				mediaTempoUltimoConjunto = mediaTempoConjuntoAtual;
				pctsFilaUltimoConjunto = pctsFilaConjuntoAtual;		
				return false;
			}	
			else
				return true;
		}
		else return true;			
	} */
		return true;
}


bool Rodada::acabou(unsigned pacotesVoz, int verbose, double tempo)
{
	unsigned i;

	if (id == 0) 	{	

		if (acabouTransiente(pacotesVoz)) {
	
			printf	("%.03lf\tTransiente\tFim da fase transiente\n\n", tempo);
			return true;
		}
		else
			return false;
	}

	for (i = 0; i < numeroChamadas; i++)
		if (fluxos[i]->getPacotesConcluidos() < tamanhoRodada)
			return false;
	
	this->fim = tempo;
	printf ("%lf\tRodada %u\t Fim da Rodada\tDuracao: %.03lf\n", tempo, id, fim-inicio);
	this->encerra(verbose);
	return true;	
	
}	


void Rodada::encerra(int verbose)
{
	unsigned i;

	estatisticas(verbose);
	estatisticasFluxosDados(verbose);

	for (i = 0; i < numeroChamadas + numFluxosTrafegoFundo; i++)
		fluxos[i]->zeraEstatisticas();
}


void Rodada::estatisticas (int verbose) {

	unsigned i;
	
	for (i = 0; i < numeroChamadas; i++)  {

		unsigned gerados = fluxos[i]->getPacotesGerados();        
		unsigned perdidos = fluxos[i]->getPacotesPerdidos();
		unsigned concluidos = fluxos[i]->getPacotesConcluidos();
		double somaTempos = fluxos[i]->getTempoGasto();
		double somaJitter = fluxos[i]->getJitter();
	
		double perdaMedia = (double) ((double)perdidos / (double)gerados) * 100.0;
		double tempoMedio = (double) (somaTempos / (double)concluidos);
		double jitterMedio = (double) (somaJitter / (double)concluidos);
		pacotesGerados += gerados;
		pacotesPerdidos += perdidos;
		pacotesConcluidos += concluidos;
		tempoTotal += somaTempos;
		jitterTotal += somaJitter;

		if (verbose >= 1) 
			printf("Chamada %d - Pacotes gerados: %u | Pacotes perdidos: %u | Perda Media %.02lf%% | Tempo Medio Gasto: %.03lf | Jitter médio %.03lf\n ", i, gerados, perdidos, perdaMedia, tempoMedio, jitterMedio);
	}
		
	if (verbose >= 0) {

		printf ("Perda media das chamadas na rodada %u: %.02lf%%\n", id, (double)((double) pacotesPerdidos /  (double)pacotesGerados) * 100.0);
		printf ("Tempo medio gasto pelos pacotes de voz na rodada %u: %.03lf\n", id, (double) ( tempoTotal / (double)pacotesConcluidos ) );
		printf ("Jitter medio dos pacotes de voz na rodada %u: %.03lf\n", id, (double) ( jitterTotal / (double)pacotesConcluidos ) );
	}

}


void Rodada::estatisticasFluxosDados (int verbose) {
	
	long numeroChegadas = 0;		
	unsigned long totalPerdidos = 0;
	unsigned i;
	double tempoRodada = fim - inicio;
		
	if (numFluxosTrafegoFundo != 0 )
	{
		for (i = numeroChamadas; i < numeroChamadas + numFluxosTrafegoFundo; i++)  {       

			unsigned gerados = fluxos[i]->getPacotesGerados();        
			unsigned perdidos = fluxos[i]->getPacotesPerdidos();
			double tempoEntreChegadas = tempoRodada / gerados;
	
			double perdaMedia = (double) ( (double) perdidos / (double) gerados ) * 100.0;
	
			numeroChegadas += gerados;			
			totalPerdidos += perdidos;

			if (verbose >= 4)
				printf("Conexao de dados %d - Pacotes gerados: %u | Pacotes perdidos: %u | Perda Media %.02lf%% | Media de tempo entre chegadas de dados: %.03lf \n", i, gerados, perdidos, perdaMedia, tempoEntreChegadas); 
		}          

		if (verbose >= 0)
			printf ("Total de pacotes de dados: %lu - Media de tempo entre chegadas: %.03lf - Perda Media %.02lf%% \n\n", numeroChegadas, tempoRodada / numeroChegadas, (double) ( (double) totalPerdidos / (double) numeroChegadas ) * 100.0 );       	
	}
}


void IC::calcula (Rodada *rodadas, unsigned numRodadas)
{
	double somaAmostras = 0;
	double mediaAmostras = 0;
	double somaVariancias = 0;
	double varianciaAmostras = 0;
	unsigned i;	

	// calcula IC da perda
	
	for (i = 1; i <= numRodadas; i++) 		
	{
		double perdaMedia = 100.0 * (double) ((double)rodadas[i].pacotesPerdidos / (double)rodadas[i].pacotesGerados); 
		somaAmostras += perdaMedia;
	}
	
	mediaAmostras = somaAmostras / numRodadas;
	
	for (i = 1; i <= numRodadas; i++) {
		
		double amostra = 100.0 * (double) ((double)rodadas[i].pacotesPerdidos / (double)rodadas[i].pacotesGerados); 
		double sigma =  amostra - mediaAmostras;
		double variancia = sigma * sigma;
		somaVariancias += variancia;
	}				
	
	varianciaAmostras = somaVariancias / (numRodadas - 1);

	centroPerda = mediaAmostras;
	tamanhoPerda = 2 * sqrt(varianciaAmostras) * ASSINTOTICO_90 / sqrt(numRodadas);	


	// calcula IC do tempo gasto pelos pacotes
	somaAmostras = 0;
	mediaAmostras = 0;
	somaVariancias = 0;
	varianciaAmostras = 0;
		
	for (i = 1; i <= numRodadas; i++) 		
		somaAmostras += rodadas[i].tempoTotal / rodadas[i].pacotesConcluidos;
	
	
	mediaAmostras = somaAmostras / numRodadas;
	
	for (i = 1; i <= numRodadas; i++) {		
		double sigma =  mediaAmostras - rodadas[i].tempoTotal / rodadas[i].pacotesConcluidos;
		double variancia = sigma * sigma;
		somaVariancias += variancia;
	}				
	
	varianciaAmostras = somaVariancias / (numRodadas - 1);

	centroTempo = mediaAmostras;
	tamanhoTempo = 2 * sqrt(varianciaAmostras) * ASSINTOTICO_90 / sqrt(numRodadas);	


	// calcula IC do jitter

	somaAmostras = 0;
	mediaAmostras = 0;
	somaVariancias = 0;
	varianciaAmostras = 0;
		
	for (i = 1; i <= numRodadas; i++) 	
	{		
		somaAmostras += rodadas[i].jitterTotal / rodadas[i].pacotesConcluidos;		
	}
	
	mediaAmostras = somaAmostras / numRodadas;
	
	for (i = 1; i <= numRodadas; i++) {		
		double sigma =  mediaAmostras - rodadas[i].jitterTotal / rodadas[i].pacotesConcluidos;
		double variancia = sigma * sigma;
		somaVariancias += variancia;
	}				
	
	varianciaAmostras = somaVariancias / (numRodadas - 1);

	centroJitter = mediaAmostras;
	tamanhoJitter = 2 * sqrt(varianciaAmostras) * ASSINTOTICO_90 / sqrt(numRodadas);	
}


void IC::exibe()
{
	printf ("\nFinal da Simulacao\n");
	printf("Intervalo de confiança 90%%: %.04lf%% - %.04lf%% (centro: %.04lf, tamanho: %.04lf)\n", centroPerda - tamanhoPerda / 2, centroPerda + tamanhoPerda / 2, centroPerda, tamanhoPerda);
	printf("Tempo gasto pelos pacotes: %.04lf - %.04lf (centro: %.04lf, tamanho: %.04lf)\n", centroTempo - tamanhoTempo / 2, centroTempo + tamanhoTempo / 2, centroTempo, tamanhoTempo);
	printf("Jitter: %.04lf - %.04lf (centro: %.04lf, tamanho: %.04lf)\n\n", centroJitter - tamanhoJitter / 2, centroJitter + tamanhoJitter / 2, centroJitter, tamanhoJitter);
	printf("Tabela, %.04lf, %.04lf, %.04lf, %.04lf, %.04lf, %.04lf, %.04lf, %.04lf, %.04lf\n\n", centroPerda - tamanhoPerda / 2, centroPerda + tamanhoPerda / 2, centroPerda, centroTempo - tamanhoTempo / 2, centroTempo + tamanhoTempo / 2, centroTempo, centroJitter - tamanhoJitter / 2, centroJitter + tamanhoJitter / 2, centroJitter);
}

