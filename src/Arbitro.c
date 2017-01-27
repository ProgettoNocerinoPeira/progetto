/*
Funzione arbitro, crea i processi fato, squadraA, squadraB e avvia il cronometro.
Legge da file config.txt la configurazione della partita e la salva in relative variabili.
I parametri di configurazione vengono inseriti in memoria condivisa per accesso a fato.

Realizzato da Mattia Nocerino e Paolo Peira
*/
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
//Variabile per il salvataggio dell'id del semaforo palla
int sembal;
//Variabili per il salvataggio delle variabili di configurazione
int Perc_Infortunio, Perc_Tiro, Perc_Dribbling, Durata_Partita;
//Prototipo le funzioni del programma
void signalHandler(int sig);
int palla();
//Inserisco writeLog solo per la prima gestione nei test, in produzione sarà compito di fato gestire i log con apposita coda di messaggi.
bool writeLog(char* text);
bool leggiConfigurazione();

bool writeLog(char* text){
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
		return 1;
	}
	printf("Errore nel file di log!!!\n");
	return 0;
}

bool leggiConfigurazione() {
  char *token;
  char *search = "=";
  int valore;
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
          writeLog("Dato durata partita non valido");
          return false;
        }
        Durata_Partita = atoi(value);
        char buffer[256];
        sprintf(buffer, "Dato Durata_Partita valido: %d", Durata_Partita);
        writeLog(buffer);
      }
      else if ((strcmp(token, "Perc_Infortunio")) == 0) {
        token = strtok(NULL, search);
        char *value = token;
        if (atoi(value)<1 && atoi(value)>100){
          writeLog("Dato Perc_Infortunio non valido");
          return false;
        }
        Perc_Infortunio = atoi(value);
        char buffer[256];
        sprintf(buffer, "Dato Perc_Infortunio valido: %d", Perc_Infortunio);
        writeLog(buffer);
      }
      else if ((strcmp(token, "Perc_Tiro")) == 0) {
        token = strtok(NULL, search);
        char *value = token;
        if (atoi(value)<1 && atoi(value)>100){
          writeLog("Dato Perc_Tiro non valido");
          return false;
        }
        Perc_Tiro = atoi(value);
        char buffer[256];
        sprintf(buffer, "Dato Perc_Tiro valido: %d", Perc_Tiro);
        writeLog(buffer);
      }
      else if ((strcmp(token, "Perc_Dribbling")) == 0) {
        token = strtok(NULL, search);
        char *value = token;
        if (atoi(value)<1 || atoi(value)>100){
          writeLog("Dato Perc_Dribbling non valido");
          return false;
        }
        Perc_Dribbling = atoi(value);
        char buffer[256];
        sprintf(buffer, "Dato Perc_Dribbling valido: %d", Perc_Dribbling);
        writeLog(buffer);
      }
    }
    fclose ( file );
    writeLog("Tutti i dati di configurazione sono stati trovati e caricati");
    return true;
  }
  else writeLog("Errore nell'apertura del file");
  return false;
}
void signalHandler(int sig){
}

int main(){
  if (leggiConfigurazione()){
    int pid = getpid();
    //creo cronometro
    if(pid==getpid()){
      pid_t cronometro= fork();
      if (cronometro==0){
        execl("cronometro",(char* )0);
        return 0;
      }
      //eseguo file cronometro
    }
    //creo palla (sem)
    int p = palla();
    //creo squadra A
    if (pid==getpid()){
      sleep(1);
      pid_t squadraA = fork();
      if (squadraA==0){
        char* str;
        sprintf(str, "%d", p);
        execl("c","c",&str,(char* )0);
        return 1;
      }
    }
    if (pid==getpid()){
      sleep(2);
      //creo squadra B
      pid_t squadraB = fork();
      if (squadraB==0){
        execl("squadra",(char* )0);
        return 1;
      }
    }
    if (pid==getpid()){
      sleep(3);
      //creo il fato
      pid_t fato = fork();
      if (fato==0){
        printf("nato processo fato\n");
        execl("fato",(char* )0);
      }
    }
    return 0;
  }
  #if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
  /* L'unione semun è già definita in <sys/sem.h> */
  #else
  /* Secondo X/OPEN dobbiamo definire semun da soli */
  union semun {
    int val; /* Valore per SETVAL */
    struct semid_ds *buf; /* buffer per IPC_STAT, IPC_SET */
    unsigned short int *array; /* array per GETALL, SETALL */
    struct seminfo *__buf; /* buffer per IPC_INFO */
  } pippo; // Alloco la unione semun
  #endif
}
int palla(){

  sembal = semget(IPC_PRIVATE, 1, 0777);
  printf("\n\nPalla %d\n\n",sembal);
  return sembal;


}
