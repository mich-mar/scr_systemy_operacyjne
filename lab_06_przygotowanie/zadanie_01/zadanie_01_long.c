// wykład
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// biblioteka do odczytu plików
#include <fcntl.h>

int main(int argc, char *argv[]) {
    system("python3 random_file.py");

    int potok_fd[2];
    pid_t pid;

    // pkt A - utworzenie potoku
    if (pipe(potok_fd) == -1) {
        // "perror" to odpowiednik "std::cerr" w c
        perror("Błąd przy tworzeniu potoku");
        exit(1);
    }

    // pkt B - utworzenie podprocesu
    pid = fork();
    if (pid == -1) {
        perror("Błąd przy tworzeniu podprocesu");
        exit(1);
    }

    // === proces rodzica ===
    if (pid != 0) {
        close(potok_fd[0]); // zamknięcie końca odczytu potoku (nieużywany przez rodzica)

        // pkt C - otwarcie pliku tekstowego
        int plik_fd = open(argv[1], O_RDONLY);
        if (plik_fd == -1) {
            perror("Błąd przy otwieraniu pliku");
            close(potok_fd[1]);
            exit(1);
        }

        // wysyłanie zawartości pliku przez potok
        char bufor[BUFSIZ];
        int licz;

        while ((licz = read(plik_fd, bufor, BUFSIZ)) > 0) {
            if (write(potok_fd[1], bufor, licz) == -1) {
                perror("Błąd przy zapisie do potoku");
                close(plik_fd);
                close(potok_fd[1]);
                exit(1);
            }
        }

        if (licz == -1) {
            perror("Błąd przy odczycie z pliku");
        }

        close(plik_fd); // zamknięcie pliku
        close(potok_fd[1]); // zamknięcie końca zapisu potoku

        // oczekiwanie na zakończenie procesu potomnego
        wait(NULL);
    }

    // === proces potomny ===
    if (pid == 0) {
        close(potok_fd[1]); // zamknięcie końca zapisu potoku (nieużywany przez potomka)

        char bufor[BUFSIZ];
        int licz;

        // pkt D - odczytywanie danych z potoku i wyświetlanie na ekranie
        while ((licz = read(potok_fd[0], bufor, BUFSIZ)) > 0) {
            write(1, "@@@ ", 5);
            write(1, bufor, licz);
            write(1, " ###\n", 6);
        }

        if (licz == -1) {
            perror("Błąd przy odczycie z potoku");
        }

        close(potok_fd[0]); // zamknięcie końca odczytu potoku
        exit(0); // zakończenie procesu potomnego
    }

    return 0;
}
