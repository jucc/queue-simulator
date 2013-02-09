/*
  Implementação dos métodos da classe Pacote
  Vide Pacote.h
*/

#include <cstdlib>
#include <iostream>
#include "Pacote.h"
#include "Evento.h"
#include "Lista.h"
#include "Fluxo.h"


using namespace std;

Pacote::Pacote() { }
             
double Pacote::getChegada() {
	return chegada;
}
void Pacote::setChegada(double chegada) {
	this->chegada = chegada;
}
double Pacote::getInicioServico() {
	return inicioServico;
}
void Pacote::setInicioServico(double inicioServico) {
	this->inicioServico = inicioServico;
}
double Pacote::getSaida() {
	return saida;
}
void Pacote::setSaida(double saida) {
	this->saida = saida;
}

int Pacote::getTipoPacote() {
	return tipoPacote;
}
void Pacote::setTipoPacote(int tipoPacote) {
	this->tipoPacote = tipoPacote;
}
Fluxo *Pacote::getFluxo() {
	return fluxo;
}
void Pacote::setFluxo(Fluxo *fluxo) {
	this->fluxo = fluxo;
}
unsigned Pacote::getTamanho() {
	return tamanho;
}
void Pacote::setTamanho(unsigned tamanho) {
	this->tamanho = tamanho;
}
unsigned Pacote::getRodada() {
	return rodada;
}
void Pacote::setRodada(unsigned rodada) {
	this->rodada = rodada;
}
unsigned long long Pacote::getSn() {
	return sn;
}
void Pacote::setSn(unsigned long long sn) {
	this->sn = sn;
}

Pacote *Pacote::getProximo() {
	return proximo;  
}
void Pacote::setProximo(Pacote *Pacote) {
	proximo = Pacote;
}

bool Pacote::antesDe(Pacote *outro) {
	
	if (sn < outro->getSn())
		return true;
	if (sn > outro->getSn())
		return false;
	if (sn == outro->getSn())
		if (getChegada() < outro->getChegada())	
			return true;
	return false;
	
}
void Pacote::atualizaEstatisticas() {

	double tempoGasto;
	Fluxo *fluxo = getFluxo();

	tempoGasto = (getSaida() - getChegada());
	fluxo->acumulaTempoGasto(tempoGasto);
	if (!(flagJitter))
		fluxo->acumulaJitter(getSaida() - fluxo->ultimaSaida);	
	
	fluxo->concluiPacote();						
}
