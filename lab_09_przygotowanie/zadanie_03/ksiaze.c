#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 10    // liczba synów
#define S 10    // liczba prób dla każdego syna
#define X 10    // liczba wierszy
#define Y 5    // liczba kolumn

int pola[X][Y]; // tablica pól
pthread_mutex_t mutex; // mutex do synchronizacji
pthread_cond_t syn_koniec;   // zmienna warunkowa do sygnalizowania końca pracy syna
int zakonczeni_synowie = 0;  // Licznik zakończonych synów

void *syn(void *arg) {
    int id = (int)arg;   // numer syna
    int szansa = 0;      // liczba zdobytych terytoriów przez tego syna
    int x, y;

    while (szansa < S) {
        x = (int)(drand48() * X);
        y = (int)(drand48() * Y);

        pthread_mutex_lock(&mutex);

        if (pola[x][y] == -1)  // sprawdzamy, czy pole jest wolne
        {
            pola[x][y] = id;    // zajmujemy pole (id syna)
            printf(":) syn nr %d przejął pole [%i,%i]\n",id,x,y);    // komunikat o zajeciu pól
        }
        else
        {
             printf(":( syn nr %d nie przejął pola [%i,%i]\n",id,x,y);    // komunikat o nie zajeciu pól
        }

        szansa++;  // Zwiększamy liczbę zużytych szans
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    zakonczeni_synowie++; // zwiększamy licznik zakończonych synów
    pthread_cond_signal(&syn_koniec);  // sygnalizujemy koniec pracy dla jednego syna
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void *rejent(void *arg) {
    // czekamy, aż wszyscy synowie zakończą swoje próby
    while (zakonczeni_synowie < N) {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&syn_koniec, &mutex); // czekamy na sygnał od synów
        pthread_mutex_unlock(&mutex);
    }

    // po zakończeniu przez wszystkich synów, zliczamy wolne pola
    int puste = 0;
    printf("pola:\n");
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            if (pola[i][j] == -1) {
                printf("[_] ");
                puste++;
            } else {
                printf("[%d] ", pola[i][j]);
            }
        }
        printf("\n");
    }

    printf("ilosc pustych pol: %d\n",puste);

    pthread_exit(NULL);
}

void game() {
    pthread_t threads[N + 1];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&syn_koniec, NULL);

    // inicjalizacja tablicy terytoriów (-1 dla wolnego pola)
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            pola[i][j] = -1;
        }
    }

    // wątki synów
    for (int i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, syn, (void *)(i));
    }

    // wątek rejenta
    pthread_create(&threads[N], NULL, rejent, NULL);

    // czekanie na zakończenie wszystkich wątków
    for (int i = 0; i < N + 1; i++) {
        pthread_join(threads[i], NULL);
    }

    // usuwanie muteksu i warunków
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&syn_koniec);
}

int main() {
    for (int i = 0; i < 1; i++) {
        zakonczeni_synowie = 0;
        printf("zmienne: N=%d, S=%d, X=%d, Y=%d\n",N,S,X,Y);
        game();
        printf("\n");
    }
    return 0;
}