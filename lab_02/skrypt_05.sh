#!/bin/bash

# =====================================================================================================================
# Zad.5. (3 punkty - na zajęciach, lub 2 punkty - w domu)
# W tym ćwiczeniu chcemy wykorzystać instrukcję pętli logicznej while sterowanej strumieniem danych. To znaczy, chcemy
# czytać strumień danych wiersz po wierszu, wykonując jakieś operacje, i zatrzymać wykonywanie tego skryptu
# po napotkaniu końca strumienia danych. Ten schemat można zapisać tak:
#
# while read x y z
# do
# ... # dowolne operacje na danych wczytanych z kolejnych wierszy pliku do zmiennych x y z
# done < plik
#
# Jako przykład zastosowania takiego schematu napisz skrypt do wysyłania emaila o ustalonej treści do wszystkich
# adresów zapisanych w kolejnych wierszach w pliku.
#
# Znajdź program klienta poczty elektronicznej pozwalającego wysyłać maile z wiersza poleceń w trybie nieinterakcyjnym
# (np. mail/mailx, pine/alpine, mutt, itp). Przeczytaj jego opis i opracuj wyrażenie shella pozwalającego automatycznie
# wysłać maila o jakiejś ustalonej treści. Następnie użyj tego wyrażenia w pętli do wysyłania maila automatycznie
# do kolejnych adresów.
#
# Uwaga: emaile muszą być rozesłane indywidualnie do wszystkich adresatów. Niedopuszczalne jest wysłanie jednego emaila
# do listy zawierającej wszystkie adresy.
# =====================================================================================================================

# function to sending emails
send_email() {
    local email_adress="$1"
    local subject="$2"
    local message="$3"

    echo -e "$message" | mail -s "$subject" "$email_adress"
}

echo "Podaj mail'a (wpisz 'EOF' aby zakończyć):"
while true; do
    read temp
    if [ "$temp" == "EOF" ]; then
        break
    else
        # adding mails to list (every mail to a new line)
        mail_list<temp
    fi
done

# check if there are any mails
if [ -z "$mail_list" ]; then
    echo "Brak adresów mail"
    exit 1
else
    echo "Lista maili:"
    echo "$mail_list"
fi

# mail content
temat="SPAM"
tresc="Skrypt_05 działa"

# sending messages to saved mails addresses
echo "$mail_list" | while read -r emial_adress; do
    send_email "$email_adress" "$temat" "$tresc"
done

