/*
Funzione arbitro, crea i processi fato, squadraA, squadraB e avvia il cronometro.
Legge da file config.txt la configurazione della partita e la salva in relative variabili.
I parametri di configurazione vengono inseriti in memoria condivisa per accesso a fato.

Realizzato da Mattia Nocerino e Paolo Peira
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include <sys/types.h>

int sembal;

void signalHandler(int sig);


void signalHandler(int sig){
  
}



int main(){

  int pid = getpid();
  //creo cronometro
  if(pid==getpid()){
    pid_t cronometro= fork();

    if (cronometro==0){

      execl("cronometro",(char* )0);
      return 0;
    }
    //eseguo file cronometro
  }
  //creo palla (sem)
  int p = palla();

  //creo squadra A
  if (pid==getpid()){
    sleep(1);
    pid_t squadraA = fork();
   if (squadraA==0){
     char* str;
      sprintf(str, "%d", p);
     execl("c","c",&str,(char* )0);
      return 1;
    }
  }

  if (pid==getpid()){
    sleep(2);
  //creo squadra B
     pid_t squadraB = fork();
     if (squadraB==0){
	execl("squadra",(char* )0);
      return 1;
      }

  }

  if (pid==getpid()){
    sleep(3);
  //creo il fato
    pid_t fato = fork();
    if (fato==0){
      printf("nato processo fato\n");
      execl("fato",(char* )0);

    }
  }
  return 0;

 }


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/errno.h>
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



int palla(){

  sembal = semget(IPC_PRIVATE, 1, 0777);
  printf("\n\nPalla %d\n\n",sembal);
  return sembal;


}
