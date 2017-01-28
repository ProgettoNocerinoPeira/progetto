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

bool spawn(){
  int pid = getpid();
  if(pid==getpid()){
    pid_t player = fork();
    if (player==0){
      char father[8];
      sprintf(father, "%d", pid);
      execl("giocatore", "giocatore", &father, (char* )0);
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
  semaphoreSetId=connectToSemaphore();
  bool finished = false;

  while (1 && finished==false){
    int semValue = semctl(semaphoreSetId,teamNumber, GETVAL, semaphore);
    while(semaphore.val<5){
      //TODO Spawn players
      semaphore.val=semaphore.val+1;
      semctl(semaphoreSetId,1, SETVAL, semaphore);
      
      printf("Team: ,stato del semaforo: %d\n", teamNumber,semaphore.val);
      if (semaphore.val==5) finished=true;
      //spawn();

    }
  }
}
