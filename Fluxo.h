/* 
   Definição das classes Fluxo, Rodada e IC
   Os fluxos são responsáveis por gerar pacotes e receber informações sobre os pacotes perdidos e tempo médio de seus pacotes para gerar estatísticas
   Nessas classes ficam as estruturas que contabilizam a geração e perda de pacotes
*/   

#define LAMBDA 0.01
#define FLUXO_VOZ 0
#define FLUXO_DADOS 1

class Fluxo {
	protected:
		int id;
		int tipo;
		unsigned rodada;

		unsigned long pacotesGerados;
		unsigned long pacotesPerdidos;
		unsigned long pacotesConcluidos;
		double somaTempoGasto;			// soma do tempo gasto por todos os pcts
		double somaJitter;
	
		Lista *listaEventos;
		Pacote *ultimo; 

	public:
		Fluxo();   
		Fluxo(int id, int tipo);

		int getId();
		int getTipo();
		double ultimaSaida;			// para calcular jitter
		void setLista(Lista *lista);
		void setRodada(unsigned rodada);

		double proximaChegadaDados();
		double proximaChegadaVoz();
		Pacote *geraPacote(); 

		void perdaDePacote();			// acrescenta nos pacotes perdidos
		void concluiPacote();			// acrescenta nos pacotes concluidos
		unsigned long getPacotesGerados();
		unsigned long getPacotesPerdidos();
		unsigned long getPacotesConcluidos();
		double getTempoGasto();
		double getJitter();
		void acumulaTempoGasto(double tempo);
		void acumulaJitter(double tempo);
		void zeraEstatisticas();	
	
		bool flagJitter;	 
};




#define TAM_CONJUNTO_TRANSIENTE 100

class Rodada {

	public:	
		unsigned id;
		unsigned tamanhoRodada;
		unsigned numeroChamadas;
		unsigned numFluxosTrafegoFundo;
		double inicio;
		double fim;
		Fluxo **fluxos;

		unsigned long pacotesGerados;
		unsigned long pacotesPerdidos;
		unsigned long pacotesConcluidos;
		double tempoTotal;	
		double jitterTotal;	

		double mediaTempoUltimoConjunto;		// para a rodada transiente
		unsigned pctsFilaUltimoConjunto;		// idem
	
		Rodada();
		Rodada(unsigned id, double inicio, unsigned tamanhoRodada, unsigned numeroChamadas, unsigned numFluxosTrafegoFundo, Fluxo **fluxos);
	
		bool acabouTransiente(unsigned pacotesVoz);
		bool acabou(unsigned pacotesVoz, int verbose, double tempo);
		void encerra(int verbose);
		void estatisticas(int verbose);
		void estatisticasFluxosDados(int verbose);
};


#define ASSINTOTICO_90 1.64

class IC {
	public:
		double centroPerda;
		double tamanhoPerda;
		double centroTempo;
		double tamanhoTempo;
		double centroJitter;
		double tamanhoJitter;

		void calcula(Rodada *rodadas, unsigned numRodadas);
		void exibe();
};
