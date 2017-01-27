#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/errno.h>
#include <stdlib.h>
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* L'unione semun è già definita in <sys/sem.h> */
#else
/* Secondo X/OPEN dobbiamo definire semun da soli */
union semun {
  int val; /* Valore per SETVAL */
  struct semid_ds *buf; /* buffer per IPC_STAT, IPC_SET */
  unsigned short int *array; /* array per GETALL, SETALL */
  struct seminfo *__buf; /* buffer per IPC_INFO */
} pippo; // Alloco la unione semun
#endif


struct sembuf pluto; // Alloco la struttura pluto di tipo sembuf

int main(int argc, char* argv[]){


  int semaforo;
  int giocatore;
  int squadra=getpid();
  semaforo = semget(IPC_PRIVATE, 1, 0777);
  printf("\nCreata la squadra: %d\n", semaforo);
  printf("\nPALLA=%d\n\n",atoi(argv[1]));
  semctl(semaforo,0,GETALL,&pippo);

  while(1){

    if( (pippo.val<5)&&(giocatore==0)){
      printf("Sto per creare un nuovo giocatore\n");
      pippo.val = pippo.val+1;
      semctl(semaforo,0,SETVAL,pippo);
      giocatore=fork();
      if (giocatore!=0){
	printf("Giocatore creato %d della squadra %d\n",giocatore,semaforo);
	printf("Numero giocatori: %d\n",pippo.val);

      }

    }


  }



	return 0;
}
