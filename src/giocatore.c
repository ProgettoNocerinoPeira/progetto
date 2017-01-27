/*
* Processo giocatore, si aggancia al semaforo di Arbitro per prendere il controllo della palla.
* Se prende il controllo della palla invia dei messaggi a Fato per conoscere il suo destino
*/
#include <stdio.h>
#include <stdlib.h>

 int main(int argc, char *argv[]){
	int idPalla=atoi(argv[1]);
	printf("Ho ricevuto idPalla = %d\n", idPalla);
		
	return 0;
 }