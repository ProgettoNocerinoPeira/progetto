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
int arbitro;


struct mymsg {
  int mtype;
  int mtext;
};

struct mymsg msg;

struct sembuf ops;
//Protitype our functions
void tiro();
int infortunio();

int connectToSemaphore();
void sig_handler(int signo);
void increaseSemaphore();
void decreaseSemaphore();
bool sendTiro();
bool sendInfortunio();
bool sendDribbling();
void takeBall();
void releaseBall();


void sig_handler(int signo){
  if (signo == SIGINT){
    printf("received SIGINT - giocatore\n");
    exit(0);
  }
}
//From here we start writing our functions

void takeBall(){
  ops.sem_num=3;
  ops.sem_op=-1;
  ops.sem_flg = 0;
  if (semop(semaphoreSetId, &ops, 1)==-1) exit(0);
}
void releaseBall(){
  ops.sem_num=3;
  ops.sem_op=+1;
  ops.sem_flg = 0;
  semop(semaphoreSetId, &ops, 1);
}

void tiro(){
  if (teamNumber==1){
    //goal teamNumberNumber 1
    printf("Invio SIGUSR1 a %d", arbitro);
    kill(arbitro,SIGUSR1);

  }else{
    //goal teamNumber 2
    kill(arbitro,SIGUSR2);
    printf("Invio SIGUSR2 a %d", arbitro);

  }
}

int infortunio(){
  //decremento di 1 il semaforo
  printf("Giocatore %d della squadra %d infortunato\n",(int) getpid(),teamNumber);
  releaseBall();
  increaseSemaphore(); //Release teamNumberPlayer
  //releaseSemaphore(3); //Release palla
  exit(1);//dovrebbe chiudere il processo
}



int connectToSemaphore(){
  key_t semaphoreKey = KEYSEMAPHORES;
  int semaphoreId;
  semaphoreId=semget(semaphoreKey, 4, IPC_CREAT | 0666);
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

bool sendTiro(){
  msg.mtype=1;
  msg.mtext=teamNumber;
  msgsnd(messageQueueId, &msg, sizeof(msg),0);
  msgrcv(messageQueueId,&msg,sizeof(msg), 4,0);
  int response = msg.mtext;
  if (response==1) return true;
  else return false;
}

bool sendInfortunio(){
  msg.mtype=2;
  msg.mtext=teamNumber;
  msgsnd(messageQueueId, &msg, sizeof(msg),0);
  msgrcv(messageQueueId,&msg,sizeof(msg), 4,0);
  int response = msg.mtext;
  if (response==1) return true;
  else return false;
}

bool sendDribbling(){
  msg.mtype=3;
  msg.mtext=teamNumber;
  msgsnd(messageQueueId, &msg, sizeof(msg),0);
  msgrcv(messageQueueId,&msg,sizeof(msg), 4,0);
  int response = msg.mtext;
  if (response==1) return true;
  else return false;
}
void main (int argc, char *argv[]){
  teamNumber=atoi(argv[1]);
  arbitro = atoi(argv[2]);
  signal(SIGINT, sig_handler);
  printf("Sono il giocatore %d della squadra %d\n",getpid(),teamNumber);
  semaphoreSetId=connectToSemaphore(); //printf("Semaphoreset id %d\n",semaphoreSetId);
  messageQueueId=connectToMessageQueue();
  if (semaphoreSetId==-1 || messageQueueId==-1) {
    printf("Non sono collegato al semaforo o alla coda messaggi");
    raise(SIGINT);
  }
  while(1){
    if (semctl(semaphoreSetId,teamNumber,GETVAL)==-1){
      exit(0);
    }
    takeBall();
    if (semctl(semaphoreSetId,teamNumber,GETVAL)==-1){
      exit(0);
    }
    printf("Ho preso la palla\n");
    bool dribbling = true;
    while (dribbling){
      if (sendTiro()){
        dribbling=false;

      }
      else if (sendInfortunio()){
        dribbling=false;
        printf("Muoio.\n");
      }
      else if (sendDribbling()){

      }
      else dribbling=false;
      sleep(1);
    }
    releaseBall();
  }
}
