#!/bin/bash

# =====================================================================================================================
#Zad.2. (2 punkty - na zajęciach)
#Napisz skrypt o nazwie policz.sh, który policzy i wyświetli liczbę plików w katalogu bieżącym. WSKAZÓWKA:
#opcja -1 do programu ls powoduje wyświetlanie po jednym pliku w linijce, a program wc można wywołać z opcją -l
#powodującą policzenie tylko liczby linijek.
#
#Następnie zmodyfikuj ten skrypt aby rozpoznawał on argument opcjonalny -a, podobnie jak program ls. To znaczy,
#z argumentem -a skrypt obliczy liczbę wszystkich plików w katalogu bieżącym, włącznie z plikami ,,ukrytymi''
#(o nazwach zaczynających się od kropki), a bez -a pominie te pliki.
# =====================================================================================================================

# check for an "-a"
# wc -l <- ounts the number of lines
if [ "$1" == "-a" ]; then
    # count all files, hidden too (ls -A -1)
    echo "Pliki: "
    ls -A -1
    echo $'\n'
   files_ctr=$(ls -A -1 | wc -l)
else
    # count only visible files (ls -1)
    echo "Pliki: "
    ls -1
    echo $'\n'
   files_ctr=$(ls -1 | wc -l)
fi

echo "Liczba plików: $files_ctr"
