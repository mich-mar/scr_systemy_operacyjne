/******************************************************************************
* FILE: bug5.c
* DESCRIPTION:
*   A simple pthreads program that dies before the threads can do their
*   work. Figure out why.
* AUTHOR: 07/06/05 Blaise Barney
* LAST REVISED: 07/11/12
******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define NUM_THREADS	5

void *PrintHello(void *threadid) {
    int i;
    double myresult = 0.0;
    printf("thread=%ld: starting...\n", threadid);

    for (i = 0; i < 1000000; i++)
        myresult += sin(i) * tan(i);
    printf("thread=%ld result=%e. Done.\n", threadid, myresult);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc;
    long t;
    for (t = 0; t < NUM_THREADS; t++) {
        printf("Main: creating thread %ld\n", t);
        rc = pthread_create(&threads[t], NULL, PrintHello, (void *) t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (t = 0; t < NUM_THREADS; t++) {
        printf("Main: joining thread %ld\n", t);
        pthread_join(threads[t], NULL);
    }
    printf("Main: Done.\n");
}

// Jeśli zamiast wątków użyłbyś podprocesów (każdy tworzony przy użyciu fork()), to program działałby inaczej pod względem
// zarządzania pamięcią i komunikacją między jednostkami wykonawczymi:
//
//     Pamięć:
//         > Wątki współdzielą tę samą przestrzeń adresową procesu, co oznacza, że wszystkie dane globalne są wspólne
// i dostępne dla wszystkich wątków. Dlatego komunikacja między wątkami jest łatwa, ale wymaga mechanizmów synchronizacji
// (np. mutexów) w przypadku współdzielenia zasobów.
//         > Podprocesy mają oddzielne przestrzenie adresowe, więc każde wywołanie fork() tworzy niezależną kopię procesu.
// Dane nie są współdzielone między podprocesami, co sprawia, że komunikacja między nimi wymaga mechanizmów IPC
// (komunikacja międzyprocesowa), takich jak potoki, kolejki wiadomości czy pamięć dzielona.
//
//     Wydajność:
//         > Tworzenie wątków jest zazwyczaj lżejsze i szybsze niż tworzenie podprocesów, ponieważ wątki współdzielą zasoby
// i nie ma potrzeby kopiowania pamięci.
//         > Podprocesy są bardziej kosztowne w tworzeniu, ponieważ wymagają stworzenia pełnej kopii procesu, ale zapewniają
// izolację, co może być korzystne w kontekście bezpieczeństwa lub stabilności.
//
//     Zakończenie i synchronizacja:
//         > Wątki wymagają jawnej synchronizacji (pthread_join) w celu zapewnienia, że wszystkie zadania zakończą się
// poprawnie.
//         > Podprocesy można kontrolować za pomocą funkcji takich jak wait() lub waitpid(), które pełnią podobną rolę co
// pthread_join w wątkach.