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

//Global variables
int team;
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
//Protitype our functions
void tiro();
int infortunio();
void releseSemaphore();
int connectToSemaphore();

//From here we start writing our functions
void releaseSemaphore(int semaphoreNumber, int type){
  if (type==1){
    int semValue = semctl(semaphoreSetId, semaphoreNumber, GETVAL, semaphore);
    
  }
  int semValue = semctl(semaphoreSetId, semaphoreNumber, GETVAL, semaphore);

}
void tiro(){
  if (team==1){
    //goal team 1
    raise(SIGUSR1);
    releaseSemaphore();
  }else{
    //goal team 2
    raise(SIGUSR2);
    releaseSemaphore();
  }
}

int infortunio(){
  //decremento di 1 il semaforo
  printf("Giocatore %d della squadra %d infortunato\n",(int) getpid(),team);
  releaseSemaphore(team);
  releaseSemaphore(3)
  exit(1);//dovrebbe chiudere il processo
}



int connectToSemaphore(){
  key_t semaphoreKey = KEYSEMAPHORES;
  int semaphoreId;
  semaphoreId=semget(semaphoreKey, 3, IPC_CREAT | 0666);
  return semaphoreId;
}
int connectToMessageQueue(){
  int messageQueue;
  key_t messageKey = KEYMESSAGEQUEUE;
  messageQueue=msgget(messageKey, IPC_CREAT | 0666);
  return messageQueue;
}
int main (int argc, char *argv[]){
  team=atoi(argv[1]);
  int semaphoreSetId=connectToSemaphore();
  int messageQueueId=connectToMessageQueue();
  if (semaphoreSetId==-1 || messageQueueId==-1) {
    printf("Non sono collegato al semaforo o alla coda messaggi");
    raise(SIGINT);
  }
  while(1){
    sleep(10);
    exit(1);
  }
  return 1;
}
