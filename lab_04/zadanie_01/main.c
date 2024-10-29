#include <stdio.h>
#include <unistd.h>  // dla sleep()
#include <time.h>    // dla nanosleep()

// komenda: top -d 0.1

int main() {
    struct timespec ts1;
    ts1.tv_sec = 0;           // 0 sekund
    ts1.tv_nsec = 10000000;   // 10 milisekund (10 milionów nanosekund)

    struct timespec ts2;
    ts2.tv_sec = 1;           // 1 sekund
    ts2.tv_nsec = 0;   // 0 milisekund (10 milionów nanosekund)


    int i = 0;

    while (1) {
        ++i;  // Przykładowa operacja
        printf("Ilosc i: %d\n", i);
        nanosleep(&ts1, NULL);  // Opóźnienie 10 ms
    }

    return 0;
}
