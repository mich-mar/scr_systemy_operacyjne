#!/bin/bash

#Zapoznaj się z programem ps (man ps). Wypróbuj dwa formaty wyświetlania rozszerzonych informacji: -f i -l.
# Przeczytaj podręcznik polecenia ps aby zrozumieć wyświetlane parametry. Naucz się wybierać zestaw
# procesów do wyświetlania argumentami opcjonalnymi -u, -t, i -p.
#
#Odpowiedz:
#(a) Ile Twoich procesów istniało w systemie w chwili przykładowego wywołania ps? Podaj wywołanie ps
# wyświetlające te procesy.
# (b) Ile procesów sshd istniało w systemie (serwerze) w chwili wykonywania ćwiczenia
# (sprawdź polecenie pgrep)? Podaj wywołanie ps/pgrep wyświetlające rozszerzoną informację o procesach,
# z nazwą użytkownika i datą uruchomienia.

# -u $USER - wyswietla procesy aktualnego użytkownika
count_user_processes=$(ps -u $USER | wc -l)
count_user_processes=$((count_user_processes - 1))
echo "Liczba procesów bieżącego użytkownika: $count_user_processes"

# pgrep - szuka po nazwie kluczowym
# -c - zwraca  ilosc znalezionych
count_sshd_processes=$(pgrep -c sshd)
echo "Liczba procesów sshd: $count_sshd_processes"




# flagi do ps:
#-p
#w poleceniu ps służy do filtrowania procesów na podstawie ich PID-ów (Process ID).
#Umożliwia wyświetlenie informacji tylko o konkretnych procesach, których identyfikatory (PID) są podane.

#-u (user)
#Pozwala wyświetlać procesy specyficzne dla podanego użytkownika.
#Przykładowo, użycie -u $USER wyświetli procesy należące do aktualnie zalogowanego użytkownika.

#-t TTY
#TTY (Terminal Type) to terminal, z którego został uruchomiony dany proces. Każdy terminal (np. sesja SSH,
#terminal w środowisku graficznym) ma swoje własne oznaczenie, takie jak pts/0, tty1, itp.




#-l =========
#UID -ID użytkownika, który uruchomił proces.
#PID - ID procesu.
#PPID - ID procesu macierzystego (parent process ID), czyli procesu, który uruchomił dany proces.
#C (Processor utilization) Procent czasu CPU zużywanego przez proces.
#PRI (Priority) - Priorytet procesu. Procesy o niższym numerze priorytetu są traktowane priorytetowo przez scheduler systemu.
#NI (Nice) - Wartość "nice" procesu, która wpływa na priorytet procesu. Może zostać zmieniona przez użytkownika lub system.
# ADDR - Adres pamięci, w której znajduje się proces. (Może być nieistotny w nowoczesnych systemach operacyjnych).
#SZ - Rozmiar procesu w pamięci w stronach (pages).
#WCHAN - "Waiting channel" – pokazuje, w jakiej funkcji jądra proces oczekuje (jeśli proces jest w stanie oczekiwania).
#TTY - Terminal, z którego proces został uruchomiony (jeśli dotyczy).
#TIME - Całkowity czas CPU zużywany przez proces (czas procesora, który został użyty od momentu uruchomienia procesu).
#CMD - Polecenie, które uruchomiło proces (nazwa i argumenty programu).

#-f ========
#UID: ID użytkownika, który uruchomił proces.
#PID: ID procesu.
#PPID: ID procesu macierzystego (parent process).
#C: Priorytet procesu.
#STIME: Czas uruchomienia procesu.
#TTY: Terminal, z którego został uruchomiony proces.
#TIME: Całkowity czas CPU używany przez proces.
#CMD: Polecenie użyte do uruchomienia procesu.
