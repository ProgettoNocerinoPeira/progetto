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
int teamNumber, semaphoreSetId,messageQueueId;


struct sembuf ops;
//Protitype our functions
void tiro();
int infortunio();

int connectToSemaphore();
void sig_handler(int signo);
void increaseSemaphore(int teamNumber);

void sig_handler(int signo){
  if (signo == SIGINT){
    printf("received SIGINT - giocatore\n");
    exit(0);
  }
}
//From here we start writing our functions

void tiro(){
  if (teamNumber==1){
    //goal teamNumberNumber 1
    raise(SIGUSR1);

  }else{
    //goal teamNumber 2
    raise(SIGUSR2);

  }
}

int infortunio(){
  //decremento di 1 il semaforo
  printf("Giocatore %d della squadra %d infortunato\n",(int) getpid(),teamNumber);
  increaseSemaphore(teamNumber); //Release teamNumberPlayer
  //releaseSemaphore(3); //Release palla
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

void increaseSemaphore(){
  ops.sem_num=teamNumber;
  ops.sem_op=1;
  ops.sem_flg = 0;
  semop(semaphoreSetId, &ops, 1);
}
int main (int argc, char *argv[]){
  teamNumber=atoi(argv[1]);
  signal(SIGINT, sig_handler);

  semaphoreSetId=connectToSemaphore(); printf("Semaphoreset id %d\n",semaphoreSetId);
  messageQueueId=connectToMessageQueue();
  if (semaphoreSetId==-1 || messageQueueId==-1) {
    printf("Non sono collegato al semaforo o alla coda messaggi");
    raise(SIGINT);
  }
  while(1){
    sleep(1);
    infortunio();
  }
  return 1;
}
