/*
  Definição da classe Pacote
  O Pacote é o item sobre o qual trabalha o simulador
  Fluxos geram pacotes e usam os setPropriedade para iniciá-lo
  Os pacotes têm estruturas próprias para encadeamento (*proximo) nas filas wfq, pq ou tx;
  Todo pacote guarda em si informações como seu tipo, tamanho, id de seu fluxo gerador, 
  e os tempos relevantes, que podem ser usados depois para gerar estatísticas sobre o uso do sistema
  Quando o pacote for perdido, terá os tempos de serviço e saída iguais a -1  
*/

#define PCT_VOZ 0 
#define PCT_DADOS 1 

#define TAM_PCT_VOZ 300
#define TAM_PCT_DADOS 1250

// forward declaration da classe fluxo para resolver dependência circular
class Fluxo;

class Pacote {

	private:
		double chegada;
		double inicioServico;
		double saida;
		int tipoPacote;
		Fluxo *fluxo;          
		unsigned tamanho;     
		unsigned rodada;				// representa a "cor" do pacote
		unsigned /*very very*/ long long sn; 	// para uso da wfq
		Pacote *proximo;             

	public:             
		Pacote();                                     
           
		double getChegada();
		void setChegada(double chegada);
		double getInicioServico();
		void setInicioServico(double isrv);
		double getSaida();
		void setSaida(double saida);
		int getTipoPacote();             
		void setTipoPacote(int tipo);           
		Fluxo *getFluxo();             
		void setFluxo(Fluxo *fluxo);
		unsigned getTamanho();             
		void setTamanho(unsigned tamanho);
		unsigned getRodada();             
		void setRodada(unsigned rodada);
		unsigned long long getSn();             
		void setSn(unsigned long long sn);
             
		Pacote *getProximo();
		void setProximo(Pacote *pacote);
		bool antesDe(Pacote *pacote);
	
		void atualizaEstatisticas();
		bool flagJitter;
} ;
