#include <stdio.h>
#include <pthread.h>
#include <math.h>

// funkcja, która będzie wykonywana przez każdy wątek
// funkcja musi mieć składnie "void* function_name(void* arg)"
void* print_hello(void* arg) {
    long thread_id = (long)arg;
    double wynik = 0;

    // obliczanie logarytmów (opcjonalnie)
    if (0)
        for (int i = 1; i < 1000; ++i) wynik += log(i);

    // wyświetlanie komunikatu z ID
    printf("Hello OpSys. Written by thread ID: %ld\n", thread_id);

    pthread_exit(NULL);
}

// kompilacja:
// gcc zadanie_01.c -o myhello -lpthread -lm
int main() {
    int threads_number = 5;

    pthread_t threads[threads_number];

    // tworzenie wątków
    for (long i = 0; i < threads_number; i++) {
        pthread_create(&threads[i], NULL, print_hello, (void*)i);
    }

    // czekanie na zakończenie wszystkich wątków
    for (int i = 0; i < threads_number; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
