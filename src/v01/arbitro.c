/*
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

//Include our commonKeys header file
#include "commonKeys.h"

//Declaration of global variables
int semaphoreSetId, messageQueueId, sharedMemoryId;
int Perc_Infortunio, Perc_Tiro, Perc_Dribbling, Durata_Partita;

//Protitype our functions

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

bool writeConfigToSharedMemorySegment();
//Scope of this function is to write config data to the shared memory segment.

//From here we start writing our functions
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
        sprintf(buffer, "Dato Durata_Partita valido: %d", Durata_Partita);
        printf(buffer);
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
        sprintf(buffer, "Dato Perc_Infortunio valido: %d", Perc_Infortunio);
        printf(buffer);
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
        sprintf(buffer, "Dato Perc_Tiro valido: %d", Perc_Tiro);
        printf(buffer);
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
        sprintf(buffer, "Dato Perc_Dribbling valido: %d", Perc_Dribbling);
        printf(buffer);
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
  if (messageQueue=msgget(messageKey, IPC_CREAT | 0666)){
    return messageQueue;
  }
  else return 0;
}

int createSharedMemory(){
  int id;
  key_t key = KEYSHAREDMEMORY;
  id = shmget(key, SIZESHAREDMEMORY, IPC_CREAT | 0655);
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
  printf("Destrying shared resources...\n");
  printf("Destroying semaphores %d\n", destroySharedResources(1,semaphoreSetId));
  printf("Destroying message queue %d\n", destroySharedResources(2,messageQueueId));
  printf("Destroying shared memory segment %d\n", destroySharedResources(3,sharedMemoryId));
}

bool writeConfigToSharedMemorySegment(){
  return 1;
}
int main(){
  //First of all I'll create a semaphoreset with 2 semaphores, 1 for the ball, and 1 to let "fato" know when I'll have the configuration data.
  //The ball semaphore will be locked and released when all the children will be running.
  //I'll also create the message queue.
  if(!(semaphoreSetId=createSemaphores(2))) return -1;
  if(!(messageQueueId = createMessageQueue())) return -1;
  if(!(sharedMemoryId = createSharedMemory())) return -1;
  //If i can read the config file I can go on with the execution of the program, otherwise I'll exit the program.
  if (readConfigFile()){
    // I store the values on the shared memory to let fato know the probabiliy values.
    printf("\n\nsemaphoreSetId: %d, messageQueueId: %d, sharedMemoryId: %d\n", semaphoreSetId, messageQueueId, sharedMemoryId);
    printf("Everyting looks fine right now.\n");
    writeConfigToSharedMemorySegment();

    destroyAll();
  }
  return -1;
}
