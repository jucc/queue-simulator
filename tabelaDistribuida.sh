#!/bin/bash

rm -f results/simMode*
rm -f results/tabelaMode*

cenario=1 

for fluxos in 14 16 18; do	
	for chamadas in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 ; do

		./Simulador -m $cenario -f $fluxos -n $chamadas -t $1 >> results/simMode${cenario}Fluxos${fluxos}chamadas${chamadas} 
		
		cat results/simMode${cenario}Fluxos${fluxos}chamadas${chamadas} | grep Tabela >> tabelaMode${cenario}Fluxos${fluxos}
 
	done
done		 
	
