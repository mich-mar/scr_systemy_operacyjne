#!/bin/bash

# Projektor - symulacja działania
echo "Projektor uruchomiony. PID: $$"

shutdown_requested=0  # Flaga do śledzenia pierwszego sygnału SIGUSR1

# Funkcja obsługująca sygnał SIGWINCH
handle_winch() {
    echo "WINDOWS CHANGE - redraw"
}

# Funkcja obsługująca sygnał SIGFPE
ignore() {
    echo "ignoruje SIGFPE"
}

# Funkcja obsługująca sygnał SIGUSR1
handle_usr1() {
    if [ $shutdown_requested -eq 0 ]; then
        echo "Czy na pewno chcesz wyłączyć? Wyślij SIGUSR1 ponownie, aby potwierdzić."
        shutdown_requested=1
        # Czekaj 5 sekund na drugi sygnał
        sleep 5
        shutdown_requested=0  # Resetowanie flagi po czasie oczekiwania
        echo "Anulowano wyłączenie. Projektor kontynuuje pracę."
    else
        echo "Projektor wyłączony."
        exit 0
    fi
}

# Ignorowanie sygnału SIGFPE
trap "ignore" SIGFPE

# Obsługa sygnału SIGWINCH
trap "handle_winch" SIGWINCH

# Obsługa sygnału SIGUSR1
trap "handle_usr1" SIGUSR1

ctr=0
while true; do
    echo "Projektor działa... ($ctr s)"
    sleep 2  # Program czeka i może odbierać sygnały
    ctr=$((ctr+2))
done