#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>    // do wait()
#include <fcntl.h>      // do potoku FIFO

#define BUFFER_SIZE BUFSIZ

int main(int argc, char *argv[]) {

    const char *fifo_name = "FIFO";
    int status;

    // 1) utworzenie procesu potomnego do "mkfifo" i "tail"
    pid_t pid = fork();

    if (pid == 0) {
        // uruchomienie "mkfifo" aby stworzyć potok FIFO
        execlp("mkfifo", "mkfifo", fifo_name, NULL);

        // uruchomienie "tail -c +1 -f" na potok FIFO
        execlp("tail", "tail", "-c", "+1", "-f", fifo_name, NULL);
    }

    // czekamy, aż potomek zakończy pracę
    wait(&status);

    // 2) proces macierzysty kontynuuje wysyłanie plików do potoku FIFO

    // pkt A - otwarcie potoku FIFO do zapisu
    int fifo_pipe = open(fifo_name, O_WRONLY);

    char bufor[BUFFER_SIZE];
    int licz;

    // przetwarzanie każdego pliku podanego w arg
    for (int i = 1; i < argc; i++) {

        // pkt B - otwarcie pliku
        FILE *plik = fopen(argv[i], "r");

        // wysłanie nagłówku z nr pliku do potoku (dla czytelności)
        char info[20];
        snprintf(info, sizeof(info), "\nPlik nr %d:\n", i);
        write(fifo_pipe, info, strlen(info));

        // pkt C - wysyłanie zawartości pliku przez FIFO
        while ((licz = fread(bufor, 1, BUFFER_SIZE, plik)) > 0) {
            write(fifo_pipe, bufor, licz);
            write(fifo_pipe, "\n", 2);
        }

        fclose(plik);

        // przerwa w wysyłaniu plików
        sleep(5);
    }

    close(fifo_pipe);

    return (status);
}