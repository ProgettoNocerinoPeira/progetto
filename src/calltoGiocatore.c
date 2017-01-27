#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/errno.h>
int main(void){
  char str[10];
  int p = palla();
  printf("Id palla = %d\n", p);
  int pid = getpid();
  //creo cronometro
  if (pid == getpid()) {
    pid_t cronometro = fork();

    if (cronometro == 0) {
      printf("sono il figlio \n");
      //Devo rendere l'int un char per poterlo passare con execl.
      sprintf(str, "%d", p);
      execl("giocatore","giocatore", &str, (char* )0);
      return 0;
    }
    //eseguo file cronometro
  }
  printf("Ho eseguito la fork\n");


  if (semctl(p, 0, IPC_RMID)!=-1) printf("Ho eliminato il semaforo\n");
  else printf("Non ho eliminato il semaforo\n");

  return 0;
}

int palla() {
  int sembal;
  sembal = semget(IPC_PRIVATE, 1, 0777);
  //printf("\n\nPalla %d\n\n", sembal);
  return sembal;
}
