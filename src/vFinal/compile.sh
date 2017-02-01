#!/bin/bash
git pull
gcc arbitro.c -o arbitro
gcc squadra.c -o squadra
gcc giocatore.c -o giocatore
gcc fato.c -o fato

echo Ho finito di compilare
