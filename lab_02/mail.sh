#!/bin/bash

# Zmienne do wysyłki maila
EMAIL_DO="markuzel.michal@gmail.com"
TEMAT="Temat wiadomości"
WIADOMOSC="Treść wiadomości"

# Wysyłanie maila
echo "$WIADOMOSC" | mail -s "$TEMAT" "$EMAIL_DO"