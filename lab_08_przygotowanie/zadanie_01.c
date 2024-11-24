#include <stdio.h>
#include <pthread.h>
#include <math.h>

// Deklaracja zmiennej bariery
pthread_barrier_t barrier;

// Funkcja, która będzie wykonywana przez każdy wątek
void* thread_function(void* arg) {
    long thread_id = (long)arg;
    double wynik = 0;

    // Czekanie na synchronizację z innymi wątkami
    pthread_barrier_wait(&barrier);

    // Obliczanie logarytmów dla liczb od 1 do 10 (dla uproszczenia)
    for (int i = 1; i < 10; ++i) {
        wynik += log(i);
    }

    // Wyświetlanie komunikatu z identyfikatorem wątku
    printf("Thread ID: %ld completed the calculations. Result: %lf\n", thread_id, wynik);

    pthread_exit(NULL);
}

// flagi: -lpthread -lm
int main() {
    int num_threads = 5;  // Liczba wątków
    pthread_t threads[num_threads];

    // Inicjalizacja bariery
    pthread_barrier_init(&barrier, NULL, num_threads);

    // Tworzenie wątków
    for (long i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, thread_function, (void*)i);
    }

    // Czekanie na zakończenie wszystkich wątków
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Zniszczenie bariery po zakończeniu programu
    pthread_barrier_destroy(&barrier);

    return 0;
}
