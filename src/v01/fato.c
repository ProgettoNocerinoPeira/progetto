
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


int createMessageQueue();
void writeLog(char *message);
int generateRandom(int value);
int readMessage();


int createMessageQueue(){
  int messageQueue;
  key_t messageKey = KEYMESSAGEQUEUE;
  messageQueue=msgget(messageKey, IPC_CREAT | 0666);
  return messageQueue;
}

int readMessage(){
  //restituisco il type in caso di successo
   msgrcv(messageQueueId,&msg,sizeof(msg), 0,0);
   if(generateRandom(30)==1){
     return (msg.mtype);
     teamNumber=msg.mtext;
     //modifico i parametri e rispondo vero
     msg.mtype=4;
      msg.mtext=1;
      msgsnd(messageQueueId, &msg, sizeof(msg),0);
   }else{
     //modifico i parametri e rispondo falso
     msg.mtype=4;
     msg.mtext=0;
     msgsnd(messageQueueId, &msg, sizeof(msg),0);
   }
   
  //leggo il numero della squadra
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
  writeLog("Testing the log writer");
  messageQueueId=createMessageQueue();
  if ((messageQueueId==-1)) writeLog("Failed to create/attach to messageQueue");
  while(1){
  
    if (readMessage()==1){
     //tiro a segno
     msglog [256]="La squadra ";
     sprintf(msglog,"%d ha fatto GOAL",teamNumber);
      writeLog (msglog);
   }
    if (readMessage()==2){
     //infortunio
     msglog [256]="Il giocatore della squadra ";
     sprintf(msglog,"%d si è infortunato",teamNumber);
     writeLog (msglog);
   }
    if (readMessage()==3){
     //dribilg
     msglog [256]="Il giocatore della squadra ";
     sprintf(msglog,"%d ha vinto il contrasto",teamNumber);
     writeLog (msglog);
   }
  }


  return 0;

}
