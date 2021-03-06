/*
______                     _   _          _____  _____
| ___ \                   | | | |        /  ___||  _  |
| |_/ / __ ___   __ _  ___| |_| |_ ___   \ `--. | | | |
|  __/ '__/ _ \ / _` |/ _ \ __| __/ _ \   `--. \| | | |
| |  | | | (_) | (_| |  __/ |_| || (_) | /\__/ /\ \_/ /
\_|  |_|  \___/ \__, |\___|\__|\__\___/  \____/  \___/
__/ |
|___/
_   _                     _              ______    _
| \ | |                   (_)             | ___ \  (_)
|  \| | ___   ___ ___ _ __ _ _ __   ___   | |_/ /__ _ _ __ __ _
| . ` |/ _ \ / __/ _ \ '__| | '_ \ / _ \  |  __/ _ \ | '__/ _` |
| |\  | (_) | (_|  __/ |  | | | | | (_) | | | |  __/ | | | (_| |
\_| \_/\___/ \___\___|_|  |_|_| |_|\___/  \_|  \___|_|_|  \__,_|


Arbitro.c created on 01/27/2017 by Mattia Nocerino & Paolo Peira @UniTO
Scope of this file is to create all shared components in our simulation and start every process needed to this software to function.
This is where the magic happens.
*/

//Start to import all libraries necessary to our "arbitro"

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
#include <sys/time.h>		/* for setitimer */
#include <signal.h>		/* for signal */

//Include our commonKeys header file
#include "commonKeys.h"


//Declaration of global variables
int semaphoreSetId, messageQueueId,messageAnswerId,team1,team2,fato;
int Perc_Infortunio, Perc_Tiro, Perc_Dribbling, Durata_Partita;
int score[] = {0,0};
struct sembuf ops;

//Protitype our functions

void sig_handler(int signo);

bool readConfigFile();
//It's scope is to read from our config.txt file the values like DurataPartita, Perc_Tiro etc.

int createSemaphores();
//This function will create semaphores and return the id of the semaphore itself.


int createMessageQueue();
//This function will create a message queue.

bool destroySharedResources(int type, int idResource);
//Scope of this function is to delete all shared resources like semaphores and message queues.The type is passed as int, 1 equals semaphore set, 2 message queues
void destroyAll();

//These functions will fork and execute other code
int createTeam(int teamNumber);
int createFato();


//From here we start writing our functions

void sig_handler(int signo){
  if (signo==SIGINT){
    kill(0,SIGALRM);
  }
  else if(signo==SIGALRM){
    alarm(0);
    destroyAll();
    printf("\n\n=====FINE PARTITA=====\n\n");
    sleep(1);
    printf("\n\n*****Risultato: squadra 1: %d-%d :squadra 2*****\n\n",score[0],score[1]);
    kill(0,SIGKILL);
  }
  else if (signo==SIGUSR1){
    printf("\nGOAL squadra 1\n");
    score[0]=score[0]+1;
    printf("Nuovo risultato:\n squadra 1 %d-%d squadra 2\n",score[0],score[1]);
  }
  else if(signo==SIGUSR2){
    printf("\nGOAL squadra 2\n");
    score[1]=score[1]+1;
    printf("Nuovo risultato:\n squadra 1 %d-%d squadra 2\n",score[0],score[1]);
  }
}

bool readConfigFile() {
  char *token;
  char *search = "=";
  int complete=0;
  static const char filename[] = "config.txt";
  FILE *file = fopen ( filename, "r" );
  if ( file != NULL )
  {
    char line [ 128 ];
    while ( fgets ( line, sizeof line, file ) != NULL ) //Lettura del file riga per riga
    {
      token = strtok(line, search);
      if ((strcmp(token, "Durata_Partita")) == 0) {
        token = strtok(NULL, search);
        char *value = token;
        if (atoi(value)<1){
          printf("Dato durata partita non valido");
          return false;
        }
        Durata_Partita = atoi(value);
        complete++;
      }
      else if ((strcmp(token, "Perc_Infortunio")) == 0) {
        token = strtok(NULL, search);
        char *value = token;
        if (atoi(value)<0 && atoi(value)>100){
          printf("Dato Perc_Infortunio non valido");
          return false;
        }
        Perc_Infortunio = atoi(value);
        complete++;
      }
      else if ((strcmp(token, "Perc_Tiro")) == 0) {
        token = strtok(NULL, search);
        char *value = token;
        if (atoi(value)<0 && atoi(value)>100){
          printf("Dato Perc_Tiro non valido");
          return false;
        }
        Perc_Tiro = atoi(value);
        char buffer[256];
        complete++;
      }
      else if ((strcmp(token, "Perc_Dribbling")) == 0) {
        token = strtok(NULL, search);
        char *value = token;
        if (atoi(value)<0 || atoi(value)>100){
          printf("Dato Perc_Dribbling non valido");
          return false;
        }
        Perc_Dribbling = atoi(value);
        complete++;
      }
    }
    fclose (file);
    if (complete==4){
      printf("Tutti i dati di configurazione sono stati trovati e caricati\n\n");
      return true;
    }
    else {
      printf("Dati non validi\n");
      return false;
    }
  }
  else printf("Errore nell'apertura del file");
  return false;
}

//Create a semaphore set using a numSem int var as the number of sempahore in the set we create.
int createSemaphores(int numSem){
  key_t semaphoreKey = KEYSEMAPHORES;
  int semaphoreId;
  semaphoreId=semget(semaphoreKey, numSem, IPC_CREAT | 0666);
  return semaphoreId;
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


bool destroySharedResources(int type, int id){
  if(type==1){
    if(semctl(id, 0, IPC_RMID)) return 1;
    else return -1;
  }
  else if (type==2) {
    if(msgctl(id, IPC_RMID, 0)) return 1;
    else return -1;
  }

  return -1;
}

void destroyAll(){
  printf("Destroying shared resources...\n");
  printf("Destroying semaphores %d\n", destroySharedResources(1,semaphoreSetId));
  printf("Destroying message queue %d\n", destroySharedResources(2,messageQueueId));
  printf("Destroying message answer queue %d\n", destroySharedResources(2,messageAnswerId));
}

int createTeam(int teamNumber){
  int pid = getpid();
  int parent = pid;
  int numeroTeam = teamNumber;
  if(pid==getpid()){
    pid_t team = fork();
    if (team==0){
      char teamName[16];
      sprintf(teamName, "%d", numeroTeam);
      char parentId[16];
      sprintf(parentId, "%d", parent);
      execl("squadra", "squadra", &teamName,&parentId, (char* )0);
    }
    return team;
  }
}
int createFato(){
  int pid = getpid();

  if(pid==getpid()){
    pid_t fato = fork();
    if (fato==0){
      execl("fato", "fato", (char* )0);
    }
    return fato;
  }
  else return -1;
}

int main(){
  signal(SIGINT, sig_handler);
  signal(SIGALRM, sig_handler);
  signal(SIGUSR1, sig_handler);//goal squadra 1
  signal(SIGUSR2, sig_handler);//goal squadra 2

  //First of all I'll create a semaphoreset with 3 semaphores, 1 for the ball,2 for the teams.

  //I'll also create the message queue and the handlers for our signals.

  //We're creating a set of 3 semaphore, first two will be used by teams to manage
  printf("Process id: %d\n\n",getpid());
  if((semaphoreSetId=createSemaphores(4))==-1){
    printf("Errore semaforo\n");
    exit(-1);
  }
  if((messageQueueId = createMessageQueue())==-1) {
    printf("Errore message\n");
    destroyAll();
    exit(-1);
  }
  if((messageAnswerId = createAnswerQueue())==-1) {
    printf("Errore message answer\n");
    destroyAll();
    exit(-1);
  }

  //If i can read the config file I can go on with the execution of the program, otherwise I'll exit the program.
  if (readConfigFile()){

    //From here we start to create all the other process needed for our simulation
    semctl(semaphoreSetId,3,SETVAL,0);
    team1 = createTeam(1);
    team2 = createTeam(2);
    fato = createFato();
    sleep(2);
    printf("Fischio di inizio!\n");
    semctl(semaphoreSetId,3,SETVAL,1);
    alarm(Durata_Partita);
    while(1){
      //Do nothing, wait for signals from children.
    }
  }
  return -1;
}
