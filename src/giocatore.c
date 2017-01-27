/*
* Processo giocatore, si aggancia al semaforo di Arbitro per prendere il controllo della palla.
* Se prende il controllo della palla invia dei messaggi a Fato per conoscere il suo destino
*/
#include <stdio.h>
#include <stdlib.h>

 int main(int argc, char *argv[]){
	int idPalla=atoi(argv[1]);
	printf("Ho ricevuto idPalla = %d\n", idPalla);
/*  while(1){
    //TODO: Accedi a semaforo in modo continuo, se successo, invia messaggio a fato e chiedi che devi fare

  }
  */
	return 0;
 }
