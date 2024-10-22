#!/bin/bash

#"Zad.2. (1 punkt - na zajęciach)
#W jednym okienku terminala uruchom hierarchię co najmniej trzech procesów, tzn. proces "A", który uruchomi proces potomny "B",
#który uruchomi proces potomny "C". Mogą to być kolejno uruchamiane interpretery poleceń, albo uruchamiające się kolejno
#skrypty lub programy. W innym oknie terminala sprawdź programem ps zależności potomków i rodziców w utworzonej hierarchii.
#
#Poleceniem kill zabij proces w środku tej hierarchii ("B"). Sprawdź poleceniem ps co pozostało z pierwotnych procesów,
#i czy osierocony proces zostanie poprawnie adoptowany przez proces init."

# Proces A
# Uruchamianie procesów w tle (&): Procesy są uruchamiane w tle,
# co zapobiega ich zakończeniu wraz z procesem nadrzędnym.
bash ./proces_b.sh &

while :
do
  echo "Proces A"
  sleep 2
done

# ps -ef --forest | grep proces_
# -e (all processes)
# Flaga -e powoduje, że ps wyświetla wszystkie procesy działające w systemie, niezależnie od użytkownika lub terminala.

#ps -o pid,ppid,cmd | grep proces_c.sh
