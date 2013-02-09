/* 
   Definição da classe Servidor 
   recebe um pacote, calcula o tempo de servico, gera o tempo de saida
*/   

#define LARGURA_BANDA 2000000 //em BITS por SEGUNDO!!

class Servidor {
	protected:
		bool ocioso;
		Lista *listaEventos;
	public:
		Servidor();   
		void servico(Pacote *pacote); 
		bool getOcioso();
		void setOcioso(bool ocioso);
		void setLista(Lista *lista);
};
