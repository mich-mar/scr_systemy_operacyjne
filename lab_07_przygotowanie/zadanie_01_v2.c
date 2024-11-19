#include <stdio.h>      // Biblioteka do operacji wejścia/wyjścia (printf, perror)
#include <stdlib.h>     // Biblioteka do funkcji standardowych (exit, malloc)
#include <fcntl.h>      // Biblioteka do operacji na plikach (open)
#include <sys/mman.h>   // Biblioteka do operacji na pamięci (mmap, munmap)
#include <unistd.h>     // Biblioteka do operacji systemowych (close, read, write)
#include <pthread.h>    // Biblioteka do operacji związanych z wątkami (mutex)
#include <errno.h>      // Biblioteka do zarządzania kodami błędów
#include <string.h>     // Biblioteka do operacji na łańcuchach znaków (strlen, strcpy)

// Stałe zgodne ze specyfikacją
#define BABBLE_NAME "/Y01-42e"      // Nazwa segmentu pamięci współdzielonej
#define BABBLE_MODE 0777            // Uprawnienia do segmentu pamięci (odczyt/zapis dla wszystkich)
#define BABBLE_LIMIT 10             // Limit liczby komunikatów w tablicy
#define BABBLE_LENGTH 80            // Maksymalna długość komunikatu (wraz z zakończeniem stringu)

// Struktura przechowująca dane w pamięci współdzielonej
struct babblespace {
    pthread_mutex_t babble_mutex; // Mutex do synchronizacji dostępu do pamięci
    pthread_cond_t babble_cond;   // Zmienna warunkowa do sygnalizowania oczekiwania
    int babble_first;             // Indeks pierwszego komunikatu w tablicy (bufor cykliczny)
    int babble_total;             // Liczba komunikatów w tablicy
    char babbles[BABBLE_LIMIT][BABBLE_LENGTH]; // Tablica przechowująca komunikaty
};

int main() {
    int shm_fd;                            // Zmienna przechowująca deskryptor segmentu pamięci
    struct babblespace *shared_data;       // Wskaźnik do segmentu pamięci współdzielonej

    // Otwórz istniejący segment pamięci współdzielonej
    shm_fd = shm_open(BABBLE_NAME, O_RDWR, BABBLE_MODE);
    if (shm_fd == -1) {                     // Sprawdź, czy otwarcie pamięci powiodło się
        perror("shm_open failed");          // Jeśli nie, wypisz błąd
        exit(EXIT_FAILURE);                 // Zakończ program z kodem błędu
    }

    // Mapowanie segmentu pamięci współdzielonej do przestrzeni adresowej procesu
    shared_data = (struct babblespace *)mmap(NULL, sizeof(struct babblespace),
                                             PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {        // Sprawdź, czy mapowanie powiodło się
        perror("mmap failed");              // Jeśli nie, wypisz błąd
        close(shm_fd);                      // Zamknij deskryptor segmentu pamięci
        exit(EXIT_FAILURE);                 // Zakończ program z kodem błędu
    }

    // Zablokowanie pamięci współdzielonej przy użyciu mutexa (w celu synchronizacji)
    pthread_mutex_lock(&(shared_data->babble_mutex));

    // Wyświetlenie istniejących komunikatów w pamięci współdzielonej
    printf("Current Messages in the Shared Memory:\n");
    int current_index = shared_data->babble_first;  // Ustawienie wskaźnika na pierwszy komunikat
    int total_messages = shared_data->babble_total; // Pobranie liczby komunikatów

    // Pętla po wszystkich dostępnych komunikatach
    for (int i = 0; i < total_messages; i++) {
        printf("Message %d: %s\n", i + 1, shared_data->babbles[current_index]);  // Wyświetlenie komunikatu
        current_index = (current_index + 1) % BABBLE_LIMIT;  // Inkrementacja indeksu w sposób cykliczny
    }

    // Odblokowanie pamięci współdzielonej po zakończeniu operacji
    pthread_mutex_unlock(&(shared_data->babble_mutex));

    // Czyszczenie: Odłączenie segmentu pamięci i zamknięcie deskryptora
    munmap(shared_data, sizeof(struct babblespace));  // Odłączenie segmentu pamięci od przestrzeni adresowej
    close(shm_fd);  // Zamknięcie deskryptora segmentu pamięci

    return 0;  // Zakończenie programu
}
