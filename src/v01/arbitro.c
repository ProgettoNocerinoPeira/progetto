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
int semaphoreSetId, messageQueueId, sharedMemoryId;
int Perc_Infortunio, Perc_Tiro, Perc_Dribbling, Durata_Partita;
int score[] = {0,0};


//Protitype our functions

void sig_handler(int signo);

bool readConfigFile();
//It's scope is to read from our config.txt file the values like DurataPartita, Perc_Tiro etc.

int createSemaphores();
//This function will create semaphores and return the id of the semaphore itself.
int createSharedMemory();
//This function will create a shared memory

int createMessageQueue();
//This function will create a message queue.

bool destroySharedResources(int type, int idResource);
//Scope of this function is to delete all shared resources like semaphores and message queues.The type is passed as int, 1 equals semaphore set, 2 message queues, 3 shared memory
void destroyAll();

//These functions will fork and execute other code
bool createTeam(int teamNumber);
bool createFato();

bool writeConfigToSharedMemorySegment();
//Scope of this function is to write config data to the shared memory segment.

//From here we start writing our functions

void sig_handler(int signo){
  if (signo == SIGINT){
    printf("received SIGINT\n");
    destroyAll();
    exit(0);
  }
  if(signo==SIGALRM){
    printf("Timer up.\n");
    destroyAll();
    exit(0);
  }
}

bool readConfigFile() {
  char *token;
  char *search = "=";
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
        char buffer[256];
        //sprintf(buffer, "Dato Durata_Partita valido: %d", Durata_Partita);
        //printf(buffer);
      }
      else if ((strcmp(token, "Perc_Infortunio")) == 0) {
        token = strtok(NULL, search);
        char *value = token;
        if (atoi(value)<1 && atoi(value)>100){
          printf("Dato Perc_Infortunio non valido");
          return false;
        }
        Perc_Infortunio = atoi(value);
        char buffer[256];
        //sprintf(buffer, "Dato Perc_Infortunio valido: %d", Perc_Infortunio);
        //printf(buffer);
      }
      else if ((strcmp(token, "Perc_Tiro")) == 0) {
        token = strtok(NULL, search);
        char *value = token;
        if (atoi(value)<1 && atoi(value)>100){
          printf("Dato Perc_Tiro non valido");
          return false;
        }
        Perc_Tiro = atoi(value);
        char buffer[256];
        //sprintf(buffer, "Dato Perc_Tiro valido: %d", Perc_Tiro);
        //printf(buffer);
      }
      else if ((strcmp(token, "Perc_Dribbling")) == 0) {
        token = strtok(NULL, search);
        char *value = token;
        if (atoi(value)<1 || atoi(value)>100){
          printf("Dato Perc_Dribbling non valido");
          return false;
        }
        Perc_Dribbling = atoi(value);
        char buffer[256];
        //sprintf(buffer, "Dato Perc_Dribbling valido: %d", Perc_Dribbling);
        //printf(buffer);
      }
    }
    fclose (file);
    printf("Tutti i dati di configurazione sono stati trovati e caricati");
    return true;
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
int createSharedMemory(){
  int id;
  key_t sharedMemorykey = 461328;
  size_t size = sizeof(SIZESHAREDMEMORY);
  id = shmget(sharedMemorykey, size, IPC_CREAT | 0666);
  return id;
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
  else if (type==3) {
    if(shmctl(id, IPC_RMID, NULL)) return 1;
    else return -1;
  }
  return -1;
}

void destroyAll(){
  printf("Destroying shared resources...\n");
  printf("Destroying semaphores %d\n", destroySharedResources(1,semaphoreSetId));
  printf("Destroying message queue %d\n", destroySharedResources(2,messageQueueId));
  printf("Destroying shared memory segment %d\n", destroySharedResources(3,sharedMemoryId));
}

/*
bool writeConfigToSharedMemorySegment(){
bool completed=false;
char *data;
data = (char *) shmat(sharedMemoryId,(void *)0,0);
if (data == (char *)(-1)) return completed;
completed = true;
return completed;
}
*/
bool createTeam(int teamNumber){
  int pid = getpid();
  int numeroTeam = teamNumber;
  if(pid==getpid()){
    pid_t team = fork();
    if (team==0){
      char teamName[16];
      sprintf(teamName, "%d", numeroTeam);
      execl("squadra", "squadra", &teamName, (char* )0);
      return 0;
    }
  }
}

int main(){
  signal(SIGINT, sig_handler);
  signal(SIGALRM, sig_handler);

  //First of all I'll create a semaphoreset with 2 semaphores, 1 for the ball, and 1 to let "fato" know when I'll have the configuration data.
  //The ball semaphore will be locked and released when all the children will be running.
  //I'll also create the message queue and the handlers for our signals.

  //We're creating a set of 3 semaphore, first two will be used by teams to manage
  if((semaphoreSetId=createSemaphores(3))==-1){
    printf("Errore semaforo\n");
    exit(-1);
  }
  if((messageQueueId = createMessageQueue())==-1) {
    printf("Errore message\n");
    destroyAll();
    exit(-1);
  }
  if((sharedMemoryId = createSharedMemory())==-1) {
    printf("Errore mem\n");
    destroyAll();
    exit(-1);
  }
  //If i can read the config file I can go on with the execution of the program, otherwise I'll exit the program.
  if (readConfigFile()){
    // I store the values on the shared memory to let fato know the probabiliy values.
    printf("\n\nsemaphoreSetId: %d, messageQueueId: %d, sharedMemoryId: %d\n", semaphoreSetId, messageQueueId, sharedMemoryId);
    printf("Everyting looks fine right now.\n");
    //printf("Attached? %d\n", writeConfigToSharedMemorySegment());


    //From here we start to create all the other process needed for our simulation
    createTeam(1);
    createTeam(2);

    sleep(10);
    destroyAll();
  }
  return -1;
}
