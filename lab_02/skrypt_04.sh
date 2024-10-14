#!/bin/bash

# =====================================================================================================================
# Zad.4. (3 punkty - na zajęciach, lub 2 punkty - w domu)
# Ćwiczenia z programem find: napisz skrypt znajdujący programem find wszystkie found_files z podanego katalogu, o podanym
# rozszerzeniu (końcówce w nazwie), które były modyfikowane w ciągu ostatnich N dni i tworzący z nich archiwum tar
# o zadanej nazwie. Mamy tu cztery parametry: nazwa katalogu, rozszerzenie nazwy plików, liczbę N i nazwę archiwum.
# Parametry będą przekazane do skryptu jako argumenty pozycyjne, to znaczy skrypt zawsze będzie wywołany z czterema
# parametrami w podanej kolejności.
#
# Uwaga: spróbuj tak napisać skrypt, aby uwzględniał tylko found_files, do których użytkownik go wywołujący ma dostęp
# przynajmniej typu read, to znaczy by nie generował błędów dla plików niedostępnych, tylko pomijał te found_files.
# =====================================================================================================================

# exit program while user prompts wrong number of arguments
# $# <- how many arg
# -ne <- not equal
if [ "$#" -ne 4 ]; then
    echo "Zła ilość argumentów (nazwa katalogu, rozszerzenie nazwy plików, liczbę N i nazwę archiwum)."
    exit 1
fi

# save args to virables
path="$1"
extension="$2"
time="$3"
tar_name="$4"

# saving matchig files to "found_files"
# -type f <- looks only for files
# -name "*.$extension" <- looks for files ened with "ectension"
# -mtime <- how long ago was this file modified
# -perm /u=r <- files with read permision to avoid errors
# -exec basename {} \; <- saves only files names exept whole path
found_files=$(find "$path" -type f -name "*.$extension" -mtime "-$time" -perm /u=r -exec basename {} \;)

# check if program found any files
# -z <- zero-length
if [ -z "$found_files" ]; then
    echo "Nie znaleziono żadnych plików."
    exit 1
fi

# make tar archive
tar -czf "$tar_name.tar" $found_files