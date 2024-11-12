#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int potok_fd[2], status;
    pid_t pid;

    // pkt A - utworzenie potoku
    pipe(potok_fd);

    // pkt B - utworzenie podprocesu
    pid = fork();
    // Wartość -1: Wystąpił błąd, co oznacza, że proces potomny nie został utworzony.
    // Wartość 0: Znajdujemy się w procesie potomnym.
    // Wartość PID > 0: Znajdujemy się w procesie rodzica (zwracany PID to PID procesu potomnego).

    // === proces rodzica ===
    if (pid != 0) {
        close(potok_fd[0]); // zamknięcie końca odczytu potoku (nieużywany przez rodzica)

        // pkt C - otwarcie pliku tekstowego z fopen
        FILE *plik = fopen(argv[1], "r");
        if (plik == NULL) {
            perror("Błąd przy otwieraniu pliku");
            close(potok_fd[1]);
            exit(1);
        }

        char bufor[BUFSIZ];
        int licz;

        // pkt C - wysyłanie zawartości pliku przez potok za pomocą fread
        while ((licz = fread(bufor, 1, BUFSIZ, plik)) > 0) {
            write(potok_fd[1], bufor, licz);  // zapis do potoku
        }

        fclose(plik); // zamknięcie pliku
        close(potok_fd[1]); // zamknięcie końca zapisu potoku

        // oczekiwanie na zakończenie procesu potomnego
        wait(&status);
    }

    // === proces potomny ===
    if (pid == 0) {
        close(potok_fd[1]); // zamknięcie końca zapisu potoku (nieużywany przez potomka)

        char bufor[BUFSIZ];
        int licz;

        // pkt D - odczytywanie danych z potoku i wyświetlanie na ekranie
        while ((licz = read(potok_fd[0], bufor, BUFSIZ)) > 0) {
            write(1, "@@@ ", 5);          // znacznik przed buforem
            write(1, bufor, licz);        // zawartość bufora
            write(1, " ###\n", 6);       // znacznik po buforze
        }

        close(potok_fd[0]); // zamknięcie końca odczytu potokud
    }

    return(status);
}
