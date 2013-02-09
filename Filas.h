/*
  Definição das classes referentes às filas de espera:
            WFQ, Subfila WFQ, PQ, TX-Ring
*/



// A classe fila provê os métodos básicos de uma fila FIFO genérica

class Fila 
{
	protected:
		Pacote *head;
		Pacote *tail;		
	public:
		void Enqueue(Pacote *pacote);
		Pacote *dequeue();
		bool vazia();
		void start();
		int pacotesNaFila;	
		Pacote *primeiro();		
};


// A classe TxRing representa a FIFO TX-Ring com capacidade para 2 pacotes

class TxRing : public Fila
{
	public:
		TxRing();

		bool podeReceber();
		void adicionaPacote(Pacote *pacote);
		Pacote *getProximo();
};


// A classe SubWfq representa cada uma das subfilas da WFQ, e provê métodos para
// tratar os valores de SN de seus pacotes

class SubWfq : public Fila 
{ 

	protected:
		int prioridade;	   
		unsigned long long ciclo; 		//vai ser atualizado pela wfq
		unsigned ocupacaoBuffer;
		unsigned maxBuffer;
	
	public: 		  
		void setPrioridade(int prioridade);
		void setOcupacaoBuffer(unsigned SN);	
		void setCiclo(unsigned long long ciclo);
		void setMaxBuffer(unsigned buffer);

		bool podeReceber(Pacote *);		
		void adicionaPacote(Pacote *pacote);		
		Pacote *getProximo();
};


// A classe GlobalWfq implementa a WFQ com um vetor de subfilas
// Provê métodos para gerir os ciclos e selecionar a próxima fila a enviar pacote

class GlobalWfq  
{
	protected: 		   
		unsigned long long ciclo;
		SubWfq **filas;
		int numFilas;
	public:
		GlobalWfq();
		GlobalWfq(int numFilas, int *prioridades, unsigned *buffers);
		void setCiclo(unsigned long long ciclo);   
		unsigned pacotes();
		unsigned pacotesVoz();

		unsigned ultimoServido;
		bool vazia();
		bool podeReceber(Pacote *);	
		void adicionaPacote(Pacote *);
		Pacote *getProximo();		
};


// Balde de tokens para verificar se os pacotes podem entrar na PQ

#define MAX_TOKENS 6000


class Balde {
	protected :
		double tokensRestantes;			// double para resolver o problema da apoximacao
		double ultimoRefil;
	public :
		Balde();
		unsigned getTokensRestantes(); 		// na interface com a PQ, usa o floor para ser unsigned
		void adicionaTokens(double tokens);
		void removeTokens(unsigned tokens);				
		double getUltimoRefil();
		void setUltimoRefil(double t);		
};


// A classe PQ herda da FIFO genérica e acrescenta funcionalidades do balde de tokens

class PQ : public Fila 
{     
	protected:
		unsigned ocupacaoBuffer; 
		Balde *balde;
		unsigned maxBuffer;
	public:
		PQ();
		void setOcupacaoBuffer(unsigned ocp);
		void setMaxBuffer(unsigned buffer);
		void setBalde(Balde *balde);

		bool podeReceber(Pacote *pacote);
		void adicionaPacote(Pacote *pacote);
		Pacote *getProximo();
};

    
     
