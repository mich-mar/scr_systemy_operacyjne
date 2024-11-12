
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define KOM "Komunikat dla rodzica.\n"

int main() {
    int potok_fd[2], licz, status;
    char bufor[BUFSIZ];

    pipe(potok_fd);
    if (fork() == 0) {
        write(potok_fd[1], KOM, strlen(KOM));
        exit(0);
    }
    close(potok_fd[1]); /* wazne */

    while ((licz=read(potok_fd[0], bufor, BUFSIZ)) > 0)
        write(1, bufor, licz);
    wait(&status);
    return(status);
}