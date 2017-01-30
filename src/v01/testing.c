
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
void sig_handler(int signalId){
  if (signalId==SIGUSR1){
    printf("Received sigurs1");
  }
}
int main(){
  bool createTeam(int teamNumber){
    int pid = getpid();
    if(pid==getpid()){
      pid_t team = fork();
      if (team==0){
        signal(SIGUSR1, sig_handler);
        while (1){
          sleep(1);
        }
      }
    }
  }

  sleep(1);
  SIGNAL(SIGUSR1);
}
