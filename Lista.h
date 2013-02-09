/*
    Definição da classe lista

    A classe Lista define uma lista de eventos encadeada ordenada pelo tempo dos eventos
    Será usada para criar a lista única de eventos, de maneira a controlar a referência do tempo 
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

