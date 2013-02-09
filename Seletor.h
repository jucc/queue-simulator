/* 
   Definição da classe Seletor
	
   Implementada para facilitar o ato de alternar entre os três cenários propostos.

   Faz a interface das chegadas de pacotes com as filas 
   e depois das primeiras filas (pq e wfq) com a TX-Ring.  

   É também responsável por acionar o fluxo informando se seu pacote for perdido (não 
   houver espaço suficiente no buffer da fila selecionada), para que o fluxo atualize 
   sua estrutura de contagem   
*/   


class Seletor {

	protected : 
		Pacote *pacoteAtual;
		int modoSimulacao;
		GlobalWfq *wfq;
		TxRing *tx;		
		PQ *pq;			
		Lista *listaEventos;
		Servidor *servidor;
	
	public :
		Seletor();
		Seletor(GlobalWfq *wfq, PQ *pq, TxRing *tx, int cenarioSimulado);
		Pacote *getPacoteAtual();
		void setPacoteAtual(Pacote *pacote);
		void setWfq(GlobalWfq *wfq);
		void setPq(PQ *pq);	
		void setTx(TxRing *tx);
		void setListaEventos(Lista *lista);
		void setServidor(Servidor *servidor);
		void setCenario(int cenario);
		// interface entre chegadas e filas
		void encaminhaPacote(Pacote *pacote);			
		void enfileiraPacote();
		void adicionaWfq();		
		// interface entre filas e TX		
		void repoeSlotTx();
};

