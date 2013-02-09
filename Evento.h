/*
  Definição da classe Evento
  Os eventos são os tipos basicos que constituem a lista de eventos
  Podem ser gerados pelos fluxos (chegada) ou pelo servidor (saida)
  mas são sempre referentes a um pacote  
*/

#define EV_CHEGADA 0 
#define EV_SAIDA 1


class Evento {

	private:
		double tempo;			// tempo do evento
		int tipoEvento;			// chegada, servico ou saida
		Pacote *pacote;			// de qual pacote	
		Evento *proximo;		// para encadear na lista

	public:
		Evento();
                
		double getTempo();
		void setTempo(double tempo);
		int getTipoEvento();
		void setTipoEvento(int tipoEvento);
		Pacote *getPacote();
		void setPacote(Pacote *pacote);

		bool depoisDe(Evento *);	// para determinar a ordem do evento na lista

		Evento *getProximo();		
		void setProximo(Evento *evento);

};
