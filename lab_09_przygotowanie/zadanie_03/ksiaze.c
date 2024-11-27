#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 5    // Liczba synów
#define S 3    // Liczba prób na syna
#define X 5    // Liczba wierszy w tablicy
#define Y 5    // Liczba kolumn w tablicy

int terytoria[X][Y]; // Tablica terytoriów
pthread_mutex_t mutex; // Mutex do synchronizacji
pthread_cond_t cond;   // Zmienna warunkowa do sygnalizowania
int zakończeni_synowie = 0;  // Licznik zakończonych synów

// Funkcja, którą wykonują synowie
void *syn(void *arg) {
    int id = (int)arg;   // Numer syna
    int szansa = 0;      // Liczba zdobytych terytoriów przez tego syna
    int x, y;

    while (szansa < S) {
        // Losowanie współrzędnych terytorium (używamy drand48() do losowania)
        x = (int)(drand48() * X);  // Losowanie wiersza
        y = (int)(drand48() * Y);  // Losowanie kolumny

        pthread_mutex_lock(&mutex);

        if (terytoria[x][y] == -1) {  // Sprawdzamy, czy terytorium jest wolne
            terytoria[x][y] = id;    // Zajmujemy terytorium
            szansa++;  // Zwiększamy liczbę zajętych terytoriów
        }

        pthread_mutex_unlock(&mutex);
    }

    // Kiedy syn zakończy swoje próby, zwiększamy licznik zakończonych synów
    pthread_mutex_lock(&mutex);
    zakończeni_synowie++;
    pthread_cond_signal(&cond);  // Sygnalizujemy, że jeden syn zakończył
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

// Funkcja wykonująca rolę Rejenta
void *rejent(void *arg) {
    // Czekamy, aż wszyscy synowie zakończą swoje próby
    while (zakończeni_synowie < N) {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);  // Czekamy na sygnał od synów
        pthread_mutex_unlock(&mutex);
    }

    // Po zakończeniu przez wszystkich synów, zliczamy wolne terytoria
    printf("Podsumowanie terytoriów:\n");
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            if (terytoria[i][j] == -1) {
                printf("[_] ");  // Puste terytorium
            } else {
                printf("[%d] ", terytoria[i][j]);  // Zajęte terytorium
            }
        }
        printf("\n");
    }

    pthread_exit(NULL);
}

void game() {
    pthread_t threads[N + 1];  // N wątków synów + 1 wątek Rejenta
    pthread_attr_t attr;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Inicjalizacja tablicy terytoriów (-1 oznacza, że terytorium jest wolne)
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            terytoria[i][j] = -1;
        }
    }

    // Tworzymy wątki dla synów
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for (int i = 0; i < N; i++) {
        pthread_create(&threads[i], &attr, syn, (void *)(i));
    }

    // Tworzymy wątek dla Rejenta
    pthread_create(&threads[N], &attr, rejent, NULL);

    // Czekamy na zakończenie wszystkich wątków
    for (int i = 0; i < N + 1; i++) {
        pthread_join(threads[i], NULL);
    }

    // Sprzątanie
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

int main() {
    for (int i = 0; i < 10; i++) {
        game();
        printf("\n");
    }
    return 0;
}
