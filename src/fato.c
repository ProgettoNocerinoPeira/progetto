/*
  Processo del fato, in fase di test legge autonomamente il file di configurazione
  in fase di produzione leggerà i dati da memoria condivisa con Arbitro
  La funzione può essere copiata direttamente in Arbitro.c
  La funzione controlla anche la presenza di errori nella impostazioni delle percentuali.
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/msg.h>
#include "chiaviComuni.h"

int Perc_Infortunio, Perc_Tiro, Perc_Dribbling, Durata_Partita;
int idMessaggi; 
bool writeLog(char* text);
int leggiConfigurazione();


int leggiConfigurazione() {
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
			return -1;
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
			return -1;
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
			return -1;
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
			return -1;
		}
		Perc_Dribbling = atoi(value);
		char buffer[256];
		sprintf(buffer, "Dato Perc_Dribbling valido: %d", Perc_Dribbling);
		writeLog(buffer);
      }
    }
    fclose ( file );
	writeLog("Tutti i dati di configurazione sono stati trovati e caricati");
    return 1;
  }
  else writeLog("Errore nell'apertura del file");
  return 0;
}
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
int main() {
  if (leggiConfigurazione()) {
	 idMessaggi=msgget(KEYMESSAGGI, IPC_CREAT | 0666);
	 if (idMessaggi==-1) {
		writeLog("Errore nella creazione coda di messaggi");
		
	}
		else 
		{
			writeLog("Ho creato la coda di messaggi");
			msgctl(idMessaggi, IPC_RMID, 0);
			writeLog("Ho eliminato la coda messaggi");
		}
    return 0;
  }
  else writeLog("Errore lettura file di configurazione");
  return -1;
}