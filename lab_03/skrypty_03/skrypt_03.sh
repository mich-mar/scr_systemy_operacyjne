#!/bin/bash

#Zasymuluj działanie projektora i pilota. Pilot wyposażony jest w kilka przycisków wysyłających sygnały sterujące
#projektorem. Utwórz:
#
#    (a) skrypt pilot, który przyjmuje jako argument numer procesu projektora i pozwala użytkownikowi wysłać do
#      projektora następujące sygnały: SIGFPE, SIGWINCH, SIGUSR1 (polecenie kill)
#    (b) skrypt projektor, który będzie reagował na odbierane sygnały (polecenie trap):
#        SIGFPE - sygnał zostanie zignorowany przez projektor,
#        SIGWINCH - projektor wypisze na ekranie "WINDOWS CHANGE - redraw" i będzie kontynuował działanie,
#        SIGUSR1 - po pierwszym odebraniu sygnału, projektor wyświetli komunikat "Czy na pewno chcesz wyłączyć?"
#        i będzie oczekiwać przez określony czas na drugi sygnał, który spowoduje wyłączenie projektora. Po upływie
#        czasu oczekiwania projektor będzie kontynuował swoje działanie (można więc ponownie próbować wyłączyć projektor,
#        wysyłając sygnał SIGUSR1 dwukrotnie).
#
#Następnie:
#
#    (c) sprawdź, że sygnałem SIGKILL możesz zabić proces, a następnie spróbuj go przechwycić w projektorze,
#    (d) sprawdź, że sygnałem SIGSTOP możesz zatrzymać proces, a sygnałem SIGCONT go wznowić,
#    (e) sprawdź, że sygnał SIGSTOP daje taki sam efekt jak naciśnięcie klawisza Ctrl+Z na terminalu,
#    (f) sprawdź, że sygnał SIGCONT daje taki sam efekt jak wykonanie polecenia fg lub bg (którego bardziej?).
#
#Uwaga: trap jest wbudowanym poleceniem shella, i jego wywołanie trochę się różni pod różnymi shellami. Najłatwiej jest
#wykonać to ćwiczenie przy użyciu Bourne shella (/bin/sh), ponieważ bash wykonuje różne potajemne zabiegi z obsługą sygnałów.
#Jednak polecenie trap w Bourne shellu jest prymitywne - czytaj man sh.\


#Zadnia:
#c) kill -SIGKILL <PID_procesu>, procesu nie da się przechwycic po unicestwieniu
#d) kill -SIGSTOP <PID_procesu>, kill -SIGCONT <PID_procesu>,
#e) ctrl + z, działa tak samo
#f) fg przenosi proces na pierwszy plan.
#   bg wznawia proces w tle.
