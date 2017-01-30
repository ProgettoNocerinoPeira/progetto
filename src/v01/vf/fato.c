
//Libraries includes
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
//Global variables needed for this piece of software
int messageQueueId,messageAnswerId;
int teamNumber;
char msglog [256];


struct mymsg
{
  int  mtype;	/* Message type */
  int  mtext;     /* Message body */
} msg;
struct mymsg msgbuf;
int team,type;
int createMessageQueue();
void writeLog(char *message);
int generateRandom(int value);
int readMessage();
void sig_handler(int signo);

void sig_handler(int signo){
  if (signo == SIGKILL){
    printf("received SIGKILL - fato\n");
    exit(0);
  }
}

int createMessageQueue(){
  int messageQueue;
  key_t messageKey = KEYMESSAGEQUEUE;
  messageQueue=msgget(messageKey, IPC_CREAT | 0666);
  return messageQueue;
}

int createAnswerQueue(){
  int messageQueue;
  key_t messageKey = KEYMESSAGEANSWER;
  messageQueue=msgget(messageKey, IPC_CREAT | 0666);
  return messageQueue;
}



void writeLog(char* text){
  FILE *logFile = fopen("log.txt", "a");
  if (logFile!=NULL){
    time_t mytime;
    mytime=time(NULL);
    fputs(strtok(ctime(&mytime), "\n"),logFile);
    fputs(" | ", logFile);
    fputs(text, logFile);
    fputs("\n", logFile);
    printf(text);
    printf("\n");
    fclose(logFile);
  }
  else printf("Errore nel file di log!!!\n");
}
//funzione che randomizza in base al tempo  preso in input un parametro restituisce 0/1
int generateRandom(int value){
  srand(time(NULL));
  if (value<=rand()%100+1){
    return 0;
  }else{
    return 1;
  }
}

int main(int argc, char *argv[]){
  signal(SIGKILL, sig_handler);
  messageQueueId=createMessageQueue();
  if ((messageQueueId==-1)) writeLog("Failed to create/attach to messageQueue");
  messageAnswerId=createAnswerQueue();
  if ((messageAnswerId==-1)) writeLog("Failed to create/attach to messageQueue");
  while(1){
    msg.mtype=0;
    //sleep(1);
    msgrcv(messageQueueId,&msg,sizeof(msg),0,IPC_NOWAIT);
    if (msg.mtype!=0){
      teamNumber=msg.mtext;
      type=msg.mtype;
      if (type==1){
        if(generateRandom(60)==1){
          msg.mtype=4;
          msg.mtext=1;
          sprintf(msglog, "La squadra %d ha fatto Goal.", teamNumber);

          msgsnd(messageAnswerId, &msg, sizeof(msg),0);
          writeLog (msglog);
        }
        else {
          msg.mtype=4;
          msg.mtext=0;
          msgsnd(messageAnswerId,&msg,sizeof(msg),0);
        }
      }
      else if (type==2){

        if(generateRandom(60)==1){
          msg.mtype=4;
          msg.mtext=1;
          sprintf(msglog, "Il giocatore della squadra %d ha subito un infortunio.", teamNumber);
          msgsnd(messageAnswerId, &msg, sizeof(msg),0);
          writeLog (msglog);
        }
        else {
          msg.mtype=4;
          msg.mtext=0;
          msgsnd(messageAnswerId,&msg,sizeof(msg),0);
        }
      }
      else if (type==3){
        if(generateRandom(30)==1){
          msg.mtype=4;
          msg.mtext=1;
          sprintf(msglog, "Il giocatore della squadra %d ha vinto il DRIBBLING.", teamNumber);
          msgsnd(messageAnswerId, &msg, sizeof(msg),0);
          writeLog (msglog);
        }
        else {
          msg.mtype=4;
          msg.mtext=0;
          msgsnd(messageAnswerId,&msg,sizeof(msg),0);
        }
      }
      else {
        msg.mtype=4;
        msg.mtext=0;
        printf("Mesaggio non valido\n");
        msgsnd(messageAnswerId, &msg, sizeof(msg),0);
      }
    }
  }
  return 0;
}
