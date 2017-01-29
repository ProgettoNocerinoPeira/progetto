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
//Protitype our functions
int tiro(int goal,int team);

//From here we start writing our functions
int tiro(int goal, int team){
  if ((goal==1)&&(team==1)){
    //goal team 1
    SIGUSR1;
  }else{
    //goal team 2
    SIGUSR2;
  }
  
}


int main (int argc, char *argv[]){
  int myteam=atoi(argv[1]);
  int team=atoi(argv[2]);
}
