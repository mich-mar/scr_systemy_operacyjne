#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

int sigusr1 = 0, sigusr2 = 0, sigalrm = 0, sigterm = 0;

void handle_sigusr1() {
    sigusr1 = 1;
}

void handle_sigusr2() {
    sigusr2 = 1;
}

void handle_sigalrm() {
    sigalrm = 1;
}

void handle_sigterm() {
    sigterm = 1;
}

void handle_signals() {
    if (sigusr1) {
        sigusr1 = 0;
        printf("Odebrano sygnał SIGUSR1\n");
    }

    if (sigusr2) {
        sigusr2 = 0;
        printf("Odebrano sygnał SIGUSR2\n");
    }

    if (sigalrm) {
        sigalrm = 0;
    }

    if (sigterm) {
        sigterm = 0;
        printf("Odebrano sygnał SIGTERM - wyłączenie programu\n");
        exit(0);
    }
}

// gcc -o <nazwa_binarki> zadanie_2.c
// kill -SIGTERM $(pgrep <nazwa_binarki>)
int main() {
    int idx = 0;
    bool recive = true;

    sigset_t signalList;

    // inicjalizacja listy sygnałów do blokowania
    sigemptyset(&signalList);
    sigaddset(&signalList, SIGUSR1);
    sigaddset(&signalList, SIGUSR2);
    sigaddset(&signalList, SIGTERM);
    sigaddset(&signalList, SIGALRM);

    while (1) {
        idx++;

        if (idx % 5000 == 0) {
            recive = !recive;
        }

        if (recive) {
            // odblokowanie odbierania sygnałów
            sigprocmask(SIG_UNBLOCK, &signalList, NULL);
        } else {
            // zablokowanie odbierania sygnałów
            sigprocmask(SIG_BLOCK, &signalList, NULL);
        }

        // odbieranie sygnałów
        signal(SIGUSR1, handle_sigusr1);
        signal(SIGALRM, handle_sigalrm);
        signal(SIGTERM, handle_sigterm);
        signal(SIGUSR2, handle_sigusr2);

        handle_signals();
    }

    return 0;
}
