#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>

struct globalInfo {
    int N; /* liczba liczb pierwszych do znalezienia (arg.wywolania) */
    int M; /* poczatkowa liczba do sprawdzania (drugi arg.wywolania, domyslnie 2) */
    int sprawdz; /* nastepna liczba do sprawdzania */
    int znalezione; /* liczba znalezionych i zsumowanych l.pierwszych */
    long suma; /* to bedzie obliczona suma liczb pierwszych */
};

// zmienne globalne
struct globalInfo info;
int N, M, K;
pthread_mutex_t muteks_pobierania;
pthread_mutex_t muteks_sumowania;

// funkcja inicjalizująca strukturę globalInfo
void globaInfoInit(int N, int M, int K) {
    info.N = N;
    info.M = M;
    info.sprawdz = M;
    info.znalezione = 0;
    info.suma = 0;
}

// funkcja sprawdzająca, czy liczba jest liczbą pierwszą
bool isPrime(long num) {
    if (num < 2)
        return false;

    for (int i = 2; i <= sqrt(num); i++) {
        if (num % i == 0)
            return false;
    }

    return true;
}

// funkcja sprawdzająca i sumująca liczby pierwsze
void primeSum() {
    long tempNum;

    // sekcja krytyczna - pobieranie liczby do sprawdzenia
    pthread_mutex_lock(&muteks_pobierania);
    tempNum = info.sprawdz;
    info.sprawdz++;
    pthread_mutex_unlock(&muteks_pobierania);

    if (isPrime(tempNum)) {
        // sekcja krytyczna - sumowanie liczby i inkrementacja liczby znalezionych
        pthread_mutex_lock(&muteks_sumowania);

        // ważny warunek, ponieważ mimo tego że przy włączeniu funkcji primeSum()
        // był prawdziwy, to mógł się on zmienić w trakcie sprawdzania liczby pierwszej
        if (info.znalezione < info.N) {
            info.suma += tempNum;
            info.znalezione++;
        }
        pthread_mutex_unlock(&muteks_sumowania);
    }
}

// funkcja dla wątków
void *threadFun(void *arg) {
    bool all = false;

    while (true) {
        // sprawdzamy, czy już znaleziono wystarczającą liczbę pierwszych
        pthread_mutex_lock(&muteks_sumowania);
        all = (info.znalezione >= info.N);
        pthread_mutex_unlock(&muteks_sumowania);

        if (all)
            break;

        primeSum();
    }
    pthread_exit(NULL);
}


// funkcja do odczytu argumentów
bool readArgs(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Użycie: zadanie_02 <N> <M> <K>\n");
        return false;
    }

    N = atoi(argv[1]);
    M = atoi(argv[2]);
    K = atoi(argv[3]);

    if (N <= 0 || M <= 0 || K <= 0) {
        printf("N, M, K muszą być liczbami dodatnimi.\n");
        return false;
    }

    return true;
}

// funkcja obsługująca tworzenie i łączenie wątków
void handleThreds() {
    pthread_t threads[K];

    for (long i = 0; i < K; i++) {
        pthread_create(&threads[i], NULL, threadFun, (void *)i);
    }

    for (int i = 0; i < K; i++) {
        pthread_join(threads[i], NULL);
    }
}

// funkcja mierząca czas rzeczywisty z dokładnością do setnych sekundy
double getCurrentTimeInSeconds() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec + (time.tv_usec / 1000000.0);
}

// sprawdzenie:
// dla (N=10000;M=2) suma wynosi 496165411
// flagi:
// -lm
int main(int argc, char *argv[]) {
    if (!readArgs(argc, argv))
        return 1;

    globaInfoInit(N, M, K);

    // inicjalizacja muteksów
    pthread_mutex_init(&muteks_pobierania, NULL);
    pthread_mutex_init(&muteks_sumowania, NULL);

    double start_time = getCurrentTimeInSeconds();

    handleThreds();

    double end_time = getCurrentTimeInSeconds();

    // obliczenie czasu w sekundach z dokładnością do setnych
    double elapsed_time = end_time - start_time;

    printf("Argumenty: M=%d, N=%d, K=%d\n", N, M, K);
    printf("Suma %d liczb pierwszych począwszy od %d wynosi: %ld\n", N, M, info.suma);
    printf("Czas obliczeń: %.2f sekund\n", elapsed_time);

    // niszczenie muteksów
    pthread_mutex_destroy(&muteks_pobierania);
    pthread_mutex_destroy(&muteks_sumowania);

    return 0;
}