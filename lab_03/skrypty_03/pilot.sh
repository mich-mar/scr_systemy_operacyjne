#!/bin/bash

if [ -z "$1" ]; then
    echo "Brak PID projektora. ELO"
    exit 1
fi

PID_PROJEKTORA=$1

while true; do
    echo "Wybierz sygnał do wysłania:"
    echo "1 - SIGFPE"
    echo "2 - SIGWINCH"
    echo "3 - SIGUSR1"
    echo "q - Wyjście"

#   -r <- znaki backslash (\) nie będą traktowane jako znaki specjalne
    read -r opcja

    case $opcja in
        1)
            kill -SIGFPE $PID_PROJEKTORA
            echo "Wysłano SIGFPE"
            ;;
        2)
            kill -SIGWINCH $PID_PROJEKTORA
            echo "Wysłano SIGWINCH"
            ;;
        3)
            kill -SIGUSR1 $PID_PROJEKTORA
            echo "Wysłano SIGUSR1"
            ;;
        q)
            echo "Wyjście z pilota."
            exit 0
            ;;
        *)
            echo "Nieznana opcja."
            ;;
    esac
done
