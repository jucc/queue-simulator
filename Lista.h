/*
    Defini��o da classe lista

    A classe Lista define uma lista de eventos encadeada ordenada pelo tempo dos eventos
    Ser� usada para criar a lista �nica de eventos, de maneira a controlar a refer�ncia do tempo 
*/

class Lista {
	protected:
		Evento *head;
		Evento *tail;
		double tempo;		// tempo atual - vem do ultimo evento dequeued

	public:
		Lista();
		double tempoCorrente();				//getTempo
		void transcorreTempoPara(double tempo);		//setTempo
		void Insere(Evento *evento);			//encadeado
		Evento Dequeue();
		Evento *primeiro();
		Evento *ultimo();
};

