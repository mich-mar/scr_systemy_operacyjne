
# Programowanie procesów - komunikacja przez potoki

## Zad.1 (6 punktów - na zajęciach)

Napisz program, który:

1. (a) utworzy potok (funkcją `pipe`),
2. (b) uruchomi podproces, z którym proces nadrzędny będzie komunikował się przez ten potok (funkcja `fork`),
3. (c) proces nadrzędny otworzy plik tekstowy podany w argumencie wywołania programu, a następnie wyśle procesowi potomnemu zawartość tego pliku przez potok,
4. (d) proces potomny odczyta dane z potoku, a następnie wyświetli je na standardowym wyjściu, dodając znacznik `@@@@` na początku i `####` na końcu każdej odebranej paczki danych. Wielkość paczek jest dowolna.

---

## Zad.2 (4 punkty - na zajęciach, lub 2 punkty - w domu)

Zmodyfikuj program z punktu 1 tak aby:

1. (a) proces nadrzędny zamiast pliku tekstowego przesyłał plik graficzny (JPEG lub PNG),
2. (b) proces potomny zamiast odczytywać bezpośrednio z potoku wykonał przekierowanie zawartości potoku na standardowe wejście (funkcje `close` i `dup`), a następnie uruchomił program (jedna z funkcji `exec*`) wyświetlający obrazek pobrany z tego wejścia (deskryptora 0).

Uruchom program i sprawdź, czy proces potomny zacznie wyświetlać obrazek od razu, czy dopiero po zamknięciu potoku przez proces nadrzędny.

Jako program wyświetlający obrazki można zastosować między innymi programy:

- `display` z pakietu Image Magick (`display -update 1`)
- `qiv` (`qiv --watch`)
- `feh` (`feh --reload 1`)
- `eog`/`eom`

---

## Zad.3 (4 punkty - w domu)

Zmodyfikuj program z punktu 1 w taki sposób, by, wykorzystując potok nazwany (FIFO), był w stanie wysyłać do tego potoku zawartość wielu kolejnych plików, których nazwy poda użytkownik w formie kolejnych argumentów wywołania. Pomiędzy przesyłaniem zawartości poszczególnych plików program ma odczekać 5 sekund.

Ponieważ potoki FIFO istnieją jako pliki na dysku i każdy program może je dowolnie otwierać, nie ma tu już potrzeby uruchamiania procesu czytającego przez klonowanie. Program czytający może być uruchomiony całkowicie niezależnie.

Do wyświetlania plików FIFO najlepiej wykorzystać program `tail` z argumentami `-c +NUM` oraz `-f` (flaga `-f` powoduje, że po osiągnięciu końca pliku, program `tail` nie kończy pracy tylko czeka na dalsze dane).
winno dobrze sformatować treść zadania, ułatwiając odczytanie kolejnych kroków.