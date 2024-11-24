
Programowanie wątków - podstawowe mechanizmy
Przygotowanie do wykonania zadania

Zapoznaj się z dokumentacją następujących funkcji biblioteki Pthread:

    pthread_create: tworzy nowy wątek i inicjuje go wywołaniem podanej funkcji,
    pthread_join: oczekuje na zakończenie wykonania wątku, na którym została wywołana ta funkcja,
    pthread_exit: kończy działanie bieżącego wątku i zwraca wartość; zwróć uwagę na specjalne działanie w przypadku wywołania w wątku głównym programu,
    pthread_mutex_init: inicjuje obiekt muteksu, który służy do synchronizacji dostępu do zasobów współdzielonych,
    pthread_mutex_lock i pthread_mutex_unlock: blokują i odblokowują mutex, aby zapewnić wyłączny dostęp do chronionego obszaru kodu.

Zadania do wykonania
Zad.1. (4 punkty - na zajęciach) Tworzenie wątków

Napisz w ANSI C prosty program wykorzystujący Pthreads, który:

    tworzy kilka wątków, z których każdy wypisuje na standardowym wyjściu napis zdefiniowany w następnym punkcie. Jako argument funkcja otrzymuje ID wątku,
    funkcja wyświetla na standardowym wyjściu napis "Hello OpSys. Written by thread ID" (gdzie ID jest identyfikatorem wątku), a następnie wywołuje pthread_exit aby zakończyć wątek.

Uwaga 1: kompilacja programów z wykorzystaniem biblioteki Pthread wymaga podania opcji -l z nazwą biblioteki w wywołaniu kompilatora:

cc -o myhello myhello.c -lpthread

(a) Uruchom swój program myhello kilka razy i zwróć uwagę na jego wyjście, i w szczególności kolejność uruchamiania i wykonywania wątków. Czy jest tym, czego oczekiwałeś? Sforułuj wnioski. Dla porównania, możesz skompilować i uruchomić dołączony przykładowy program hello.c.

(b) Dodaj wątkom w stworzonym programie wykonywanie konkretnej pracy obliczeniowej:

for (i=1; i<1000; ++i) wynik += log(i);

Użyj flagi -lm kompilacji w celu dolinkowania biblioteki matematycznej. Ponownie uruchom program wiele razy i porównaj zachowanie wątków.

(c) Następnie przejrzyj, skompiluj i uruchom dostarczony program bug5.c. Wyjaśnij co obserwujesz i jak to naprawić.

Jak to się porównuje z sytuacją, gdyby zamiast wątków w programie były wykorzystane podprocesy?

(d) Zmniejsz liczbę iteracji w wątkach do 10. Uruchom program co najmniej 5 razy, obserwując kolejność zakończenia wątków. Następnie dodaj do programu barierę, aby wymusić jednoczesne wystartowanie obliczeń wszystkich wątków. Ponownie uruchom program co najmniej 5 razy, podsumuj uzyskany wynik, i porównaj z poprzednim.
Zad.2. (1 punkt - na zajęciach) Przekazywanie argumentów

    Przejrzyj kod programów hello_arg1.c oraz hello_arg2.c. Zauważ w jaki sposób do wątku przekazywany jest pojedynczy argument, oraz jak przekazać wiele argumentów przez strukturę. Skompiluj oraz uruchom oba programy i obserwuj wyjście.
    Przejrzyj, skompiluj i uruchom program bug3.c. Co poszło nie tak? Jak to naprawić?

Zad.3. (5 punktów - na zajęciach, lub 3 punkty - w domu) Muteksy

(a) Napisz w języku ANSI C program obliczający sumę kolejnych N liczb pierwszych poczynając od M.

Program powinien sprawdzać kolejno wszystkie liczby od M wzwyż, czy nie dzielą się przez jakąkolwiek liczbę większą od 1 i mniejszą od niej samej. W przypadku gdy liczba dzieli się przez cokolwiek w tym przedziale, jej sprawdzanie powinno być natychmiast porzucane (bo nie jest pierwsza), a w przeciwnym wypadku liczba powinna być dodawana do obliczanej sumy, i licznik znalezionych liczb pierwszych powinien być inkrementowany. Procedura zatrzymuje się gdy ten licznik osiągnie zadaną wartość N, która powinna być odczytywana jako pierwszy argument wywołania programu, i suma powinna być wyświetlona na wyjściu.

Dla potwierdzenia poprawności działania programu na przykład sprawdź, że suma pierwszych 10000 liczb pierwszych (N=10000;M=2) wynosi 496165411. Potwierdź to w raporcie.

(b) Aby przyspieszyć działanie programu wykorzystując wielordzeniowy procesor komputera, zamień program na wielowątkowy. Wykorzystaj następujące zmienne globalne:

int N; /* liczba liczb pierwszych do znalezienia (arg.wywolania) */
int M; /* poczatkowa liczba do sprawdzania (drugi arg.wywolania, domyslnie 2) */
int K; /* liczba watkow obliczeniowych (trzeci arg.wywolania, domyslnie 1) */
int sprawdz = M; /* nastepna liczba do sprawdzania */
int znalezione = 0; /* liczba znalezionych i zsumowanych l.pierwszych */
long suma = 0; /* to bedzie obliczona suma liczb pierwszych */

Program powinien utworzyć K wątków obliczeniowych, z których każdy będzie powtarzalnie wykonywał następujące kroki:

1. pobierz aktualną wartość liczby `sprawdz` i natychmiast zinkrementuj ją
2. sprawdź czy liczba `sprawdz` jest pierwsza
3. jeśli tak, to dodaj ją do `suma` i zinkrementuj `znalezione`
4. jeśli wartość `znalezione` jest równa lub większa `N` to zakończ pracę
5. w przeciwnym wypadku powtórz kroki od 1.

Liczba wątków obliczeniowych K powinna być zadana trzecim argumentem wywołania programu. Wątek główny (funkcja main) powinien czekać na zakończenie pracy wszystkich wątków obliczeniowych, po czym wyświetlać na wyjściu obliczoną sumę liczb pierwszych.

Uruchom program wielokrotnie dla różnych wartości N i K (M=2) i porównaj otrzymywane wyniki z wersją jednowątkową. Odpowiedz w raporcie na pytania:

    Czy wersja wielowątkowa zawsze daje te same wyniki co wersja jednowątkowa?
    Jeśli nie, to co może być tego przyczyną?

(c) Dodaj w programie następujące dwa muteksy:

pthread_mutex_t muteks_pobierania;
pthread_mutex_t muteks_sumowania;

Dodaj kod poprawnie inicjalizujący oba muteksy, a następnie dodaj w wątkach obliczeniowych ochronę kroku pobierania i inkrementowania liczby do sprawdzenia pierwszym muteksem, oraz ochronę kroku sumowania liczb pierwszych i inkrementowania ich liczby drugim muteksem. Zadbaj by sekcje krytyczne chronione muteksami miały minimalny zakres.

Ponownie uruchom wielokrotnie program dla różnych wartości N i K (M=2) i porównaj otrzymywane wyniki z wersją jednowątkową. Odpowiedz w raporcie na pytania:

    Czy teraz wersja wielowątkowa zawsze daje te same wyniki co wersja jednowątkowa? Wyjaśnij.
    O ile szybciej działa wersja wielowątkowa od jednowątkowej (jeśli w ogóle działa szybciej)?
    Aby znaleźć odpowiedź na to pytanie możesz uruchamiać program za pomocą polecenia time.
    W tych eksperymentach przyjmij jako wartość K liczbę fizycznych rdzeni systemu komputerowego, minus 1 lub 2.
    Jeśli wersja wielowątkowa nie działa w każdym przypadku K razy szybciej niż jednowątkowa, to spróbuj wyjaśnić dlaczego.
    Podaj w raporcie z wykonania zadania kilka wybranych wartości obliczonych sum i odpowiednich czasów działania programu z parametrami N i K.
    W szczególności oblicz sumę pierwszych ćwierć miliona liczb pierwszych, (N=250000;M=2) oraz stu kolejnych liczb pierwszych poczynając od liczby 1000000 (N=100;M=1000000), i podaj w raporcie otrzymane wartości oraz użytą liczbę K i czasy działania programu.

