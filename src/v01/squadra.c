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

int main(int argc, char *argv[]){
  int teamNumber=atoi(argv[1]);
  int semaphoreSetId=atoi(argv[2]);
  printf("Team %d, semaphoreSet %d", teamNumber, semaphoreSetId);
  //semctl(semaphoreSetId)
  return 0;
}
