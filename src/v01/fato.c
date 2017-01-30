
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
int messageQueueId;
int teamNumber;
char msglog [256];


struct mymsg
{
  int  mtype;	/* Message type */
  int  mtext;     /* Message body */
} msg;

int team,type;
int createMessageQueue();
void writeLog(char *message);
int generateRandom(int value);
int readMessage();
void sig_handler(int signo);

void sig_handler(int signo){
  if (signo == SIGINT){
    printf("received SIGINT - fato\n");
    exit(0);
  }
}

int createMessageQueue(){
  int messageQueue;
  key_t messageKey = KEYMESSAGEQUEUE;
  messageQueue=msgget(messageKey, IPC_CREAT | 0666);
  return messageQueue;
}


int readAndAnswerMessage(){
  msgrcv(messageQueueId, &msg, sizeof(msg), 0, 0);
  teamNumber = msg.mtext;
  type = msg.mtype;
  if (msg.mtype==1){
    if (generateRandom(30)==1) msg.mtext=1;
  }
  else if (msg.mtype==2){
    if (generateRandom(30)==1) msg.mtext=1;
  }
  else if (msg.mtype==3){
    if (generateRandom(30)==1) msg.mtext=1;
  }
  else msg.mtext=0;
  msg.mtype=4;
  return msg.mtext;
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
  sleep(1);//per cambiare il risultato ad ogni accesso
  if (value>rand()%100){
    return 0;
  }else{
    return 1;
  }
}

int main(int argc, char *argv[]){
  signal(SIGINT, sig_handler);
  messageQueueId=createMessageQueue();
  if ((messageQueueId==-1)) writeLog("Failed to create/attach to messageQueue");
  while(1){
    int response = readAndAnswerMessage();
    if (response==1){
      if (type==1){
        sprintf(msglog, "La squadra %d ha fatto Goal.", teamNumber);
        printf("Sending %d type %d", msg.mtext, msg.mtype);
        msgsnd(messageQueueId, &msg, sizeof(msg),0);
        printf("Sent %d type %d", msg.mtext, msg.mtype);
        writeLog (msglog);
      }
      if (type==2){
        sprintf(msglog, "Il giocatore della squadra %d ha subito un infortunio.", teamNumber);
        printf("Sending %d type %d", msg.mtext, msg.mtype);
        msgsnd(messageQueueId, &msg, sizeof(msg),0);
        printf("Sent %d type %d", msg.mtext, msg.mtype);
        writeLog (msglog);
      }
      if (type==3){
        sprintf(msglog, "Il giocatore della squadra %d ha vinto il dribbling.", teamNumber);
        printf("Sending %d type %d", msg.mtext, msg.mtype);
        msgsnd(messageQueueId, &msg, sizeof(msg),0);
        printf("Sent %d type %d", msg.mtext, msg.mtype);;
        writeLog (msglog);
      }
    }
    sleep(1);
  }
  return 0;
}
