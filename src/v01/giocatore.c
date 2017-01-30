#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/errno.h>
#include <sys/stat.h>

//Include our commonKeys header file
#include "commonKeys.h"

/*
TODO: Finish the comment.
*/

//Protitype our functions
void tiro(int team);
int infortunio(int team);


//From here we start writing our functions
void tiro(int team){
  if ((goal==1)&&(team==1)){
    //goal team 1
    signal(SIGUSR1);
  }else{
    //goal team 2
    signal(SIGUSR2);
  }
}

int infortunio(int team){
//decremento di 1 il semaforo
  printf("Giocatore %d della squadra %d infortunato\n",(int) getpid(),team);
exit();//dovrebbe chiudere il processo
}


int main (int argc, char *argv[]){
  int myteam=atoi(argv[1]);
  int team=atoi(argv[2]);
}
