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

struct sembuf ops;
int semaphoreSetId, teamNumber;


int connectToBall();
bool spawn();
void sig_handler(int signo);
void decreaseSemaphore();

void sig_handler(int signo){
  if (signo == SIGINT){
    printf("received SIGINT- squadra\n");
    exit(0);
  }
}
bool spawn(int teamNumber){
  int pid = getpid();
  if(pid==getpid()){
    pid_t player = fork();
    if (player==0){
      char team[2];
      sprintf(team, "%d", teamNumber);
      execl("giocatore", "giocatore",&team, (char* )0);
      return 0;
    }
  }
}

int connectToSemaphore(){
  key_t semaphoreKey = KEYSEMAPHORES;
  int semaphoreId;
  semaphoreId=semget(semaphoreKey, 3, IPC_CREAT | 0666);
  return semaphoreId;
}
void decreaseSemaphore(){
  ops.sem_num=teamNumber;
  ops.sem_op=-1;
  ops.sem_flg = 0;
  semop(semaphoreSetId, &ops, 1);
}

int main (int argc, char *argv[]){

  teamNumber = atoi(argv[1]);
  signal(SIGINT, sig_handler);
  semaphoreSetId=connectToSemaphore();
  semctl(semaphoreSetId,teamNumber, SETVAL, 5);
  while (1){
    if (semctl(semaphoreSetId,teamNumber,GETVAL)<=5){
      decreaseSemaphore();
      printf("Team: %d ,numero giocatori: %d\n", teamNumber,semctl(semaphoreSetId,teamNumber,GETVAL));
      spawn(teamNumber);
    }
    else sleep(2);
  }
}
