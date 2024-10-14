#!/bin/bash

# =====================================================================================================================
# Zad.1. (2 punkty - na zajęciach)
# Wykorzystaj date do wyświetlania różnych komunikatów powitalnych w zależności czy aktualnie jest dzień roboczy
# czy świąteczny (dla uproszczenia: dzień tygodnia czy weekend).
#
# Uwaga: Porównywanie napisów wyświetlanych przez program date (i inne programy) jest zależne od języka i lokalizacji.
#
# Sprawdź możliwość ustawienia zmiennych lokalizacji LC_* (np. LC_ALL=C) i ich wpływ na postać komunikatu.
# Która/które z nich sterują postacią wyświetlanej daty? Ustawiając tę zmienną w skrypcie na wartość C (lub POSIX),
# wymuszamy lokalizację kanoniczną, co ułatwia sprawdzanie wartości daty, i uniezależnia działanie skryptu
# od lokalizacji (będzie on poprawnie sprawdzał warunek również np. w Japonii).
# =====================================================================================================================

# pobranie aktualnego dnia tygodnia
# date +%u <- zwraca numer dnia tygodnia, gdzie 1 to poniedziałek, a 7 to niedziela.
day_of_week=$(date +%u)

# Sprawdzenie czy jest dzień roboczy (1-5 to dni robocze: Pon-Pt)
# -gt – większe niż (greater than)
# -lt – mniejsze niż (less than)
# -eq – równe (equal)
# -ne – różne (not equal)
if [ "$day_of_week" -ge 1 ] && [ "$day_of_week" -le 5 ]; then
    echo "Witaj, dzisiaj jest dzień roboczy..."
else
    # Jeśli dzień to sobota (6) lub niedziela (7)
    echo "Witaj, dzisiaj jest weekend!"
fi
