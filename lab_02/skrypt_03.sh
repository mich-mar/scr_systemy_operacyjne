#!/bin/bash

# =====================================================================================================================
#Zad.3. (2 punkty - na zajęciach, lub 1 punkt - w domu)
#Wykorzystując instrukcję pętli for napisz skrypt, który dla dowolnej liczby argumentów pozycyjnych wyświetli informacje
#o każdym z nich traktowanym jako nazwa pliku. Jeśli dany argument określa istniejący plik, to powinien
#się pojawić komunikat "plik istnieje", i analogicznie, jeśli nie istnieje plik o podanej nazwie. W przypadku gdyby
#podany argument określał katalog, należy wyświetlić "katalog", ale jeśli jest to katalog pusty (nie zawiera żadnych
#plików, ani podkatalogów, z wyjątkiem . i ..), to należy wyświetlić "katalog pusty".
# =====================================================================================================================

# for loop for every file from arguments
for arg in "$@"; do
    # -d <- check if arg is a directory
    if [ -d "$arg" ]; then
        # ls -A <- show files in a directory
        if [ "$(ls -A "$arg")" ]; then
            echo "$arg - katalog"
        else
            echo "$arg - katalog pusty"
        fi
    # case when agr is a file
    # -f <- check if arg is a file
    elif [ -f "$arg" ]; then
        echo "$arg - plik istnieje"
    # in case when there isn't such an arg
    else
        echo "$arg - plik/katalog nie istnieje"
    fi
done
