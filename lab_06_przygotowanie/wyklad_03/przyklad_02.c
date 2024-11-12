#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int potok_fd[2], licz; char bufor[BUFSIZ];
    pipe(potok_fd);
    if (fork() == 0) { /* podproces tylko piszacy */
        close(1); /* zamykamy stdout prawdziwy */
        dup(potok_fd[1]); /* odzyskujemy fd 1 w potoku */
        close(potok_fd[1]); /* dla porzadku */
        close(potok_fd[0]); /* dla porzadku */
        close(0); /* dla porzadku */
        execlp("ps","ps","-fu",getenv("LOGNAME"),NULL);
    }
    close(potok_fd[1]); /* wazne */
    while ((licz=read(potok_fd[0], bufor, BUFSIZ)) > 0)
        write(1, bufor, licz);
}