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


union semun {
  // value for SETVAL
  int val;
  // buffer for IPC_STAT, IPC_SET
  struct semid_ds* buf;
  // array for GETALL, SETALL
  unsigned short* array;
  // Linux specific part
  #if defined(__linux__)
  // buffer for IPC_INFO
  struct seminfo* __buf;
  #endif
} semaphore, ballSemaphore;
struct sembuf buffer;



int semaphoreSetId, teamNumber;
int connectToBall();
bool spawn();
void sig_handler(int signo);


void sig_handler(int signo){
  if (signo == SIGINT){
    printf("received SIGINT- squadra\n");
    semaphore.val=-1;
    semctl(semaphoreSetId,teamNumber, SETVAL, semaphore);
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
int main (int argc, char *argv[]){

  teamNumber = atoi(argv[1]);
  signal(SIGINT, sig_handler);
  semaphoreSetId=connectToSemaphore();
  printf("Semaforo squadra %d, id: %d\n", teamNumber,semaphoreSetId);
  semaphore.val=5;
  semctl(semaphoreSetId,1, SETVAL, semaphore);
  bool finished = false;

  while (1){
    int semValue = semctl(semaphoreSetId,teamNumber, GETVAL, semaphore);
    printf("Semaforo team: %d, valore: %d\n", teamNumber, semaphore.val);
    sleep(1);

    while(semaphore.val>0 && semaphore.val<=5){
      //TODO Spawn players
      semaphore.val=semaphore.val-1;
      semctl(semaphoreSetId,teamNumber, SETVAL, semaphore);
      printf("Team: %d ,numero giocatori: %d\n", teamNumber,(semaphore.val));
      spawn(teamNumber);
    }
  }
}
