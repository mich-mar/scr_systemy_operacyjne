#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <sys/time.h>

struct globalInfo {
    int N; /* liczba liczb pierwszych do znalezienia (arg.wywolania) */
    int M; /* poczatkowa liczba do sprawdzania (drugi arg.wywolania, domyslnie 2) */
    int K; /* liczba watkow obliczeniowych (trzeci arg.wywolania, domyslnie 1) */
    int sprawdz; /* nastepna liczba do sprawdzania */
    int znalezione; /* liczba znalezionych i zsumowanych l.pierwszych */
    long suma; /* to bedzie obliczona suma liczb pierwszych */
};

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

// funkcja mierząca czas rzeczywisty z dokładnością do setnych sekundy - zadanie 2c
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
    if (argc < 3) {
        printf("Użycie: %s <N> <M>\n", argv[0]);
        return 1;
    }

    long N = atol(argv[1]);
    long M = atol(argv[2]);

    if (N <= 0 || M <= 0) {
        printf("N i M muszą być nummi dodatnimi.\n");
        return 1;
    }

    long sum = 0;
    int znalezioneLiczby = 0;
    long num = M;

    double start_time = getCurrentTimeInSeconds();

    // szukanie N liczb pierwszych i ich sumowanie
    while (znalezioneLiczby < N) {
        if (isPrime(num)) {
            sum += num;
            znalezioneLiczby++;
        }
        num++;
    }

    double end_time = getCurrentTimeInSeconds();

    // obliczenie czasu w sekundach z dokładnością do setnych
    double elapsed_time = end_time - start_time;

    printf("Suma %ld liczb pierwszych począwszy od %ld wynosi: %ld\n", N, M, sum);
    printf("Czas obliczeń: %.2f sekund\n", elapsed_time);

    return 0;
}
