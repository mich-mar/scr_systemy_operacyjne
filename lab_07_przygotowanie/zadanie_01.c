#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      // Dla shm_open, O_RDWR
#include <sys/mman.h>   // Dla mmap
#include <unistd.h>     // Dla ftruncate, close
#include <pthread.h>    // Dla pthread_mutex_t, pthread_cond_t

// Definicje zgodne z podanymi w zadaniu
#define BABBLE_NAME "/SCR_GRUPA_4"  // Zmień na nazwę zgodną z twoją grupą laboratoryjną
#define BABBLE_MODE 0777
#define BABBLE_LIMIT 10
#define BABBLE_LENGTH 80

// Struktura pamięci współdzielonej
struct babblespace {
    pthread_mutex_t babble_mutex;      // Mutex do synchronizacji
    pthread_cond_t babble_cond;        // Zmienna warunkowa do synchronizacji
    int babble_first;                  // Indeks pierwszego komunikatu
    int babble_total;                  // Liczba wszystkich komunikatów
    char babbles[BABBLE_LIMIT][BABBLE_LENGTH];  // Tablica komunikatów
};

void initialize_shared_memory(struct babblespace *shared_memory) {
    // Inicjalizacja mutexa i zmiennej warunkowej
    pthread_mutexattr_t mutex_attr;
    pthread_condattr_t cond_attr;

    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shared_memory->babble_mutex, &mutex_attr);
    pthread_mutexattr_destroy(&mutex_attr);

    pthread_condattr_init(&cond_attr);
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&shared_memory->babble_cond, &cond_attr);
    pthread_condattr_destroy(&cond_attr);

    // Inicjalizacja pól struktury
    shared_memory->babble_first = 0;
    shared_memory->babble_total = 0;
    memset(shared_memory->babbles, 0, sizeof(shared_memory->babbles));
}

//    gcc -o babbler zadanie_01.c -lpthread


int main() {
    // Otwórz lub utwórz segment pamięci współdzielonej
    int shm_fd = shm_open(BABBLE_NAME, O_RDWR | O_CREAT, BABBLE_MODE);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Ustal rozmiar segmentu pamięci (na podstawie struktury)
    size_t babblespace_size = sizeof(struct babblespace);

    // Jeśli segment został nowo utworzony, ustaw jego rozmiar
    if (ftruncate(shm_fd, babblespace_size) == -1) {
        perror("ftruncate");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    // Mapowanie segmentu pamięci do przestrzeni adresowej procesu
    struct babblespace *shared_memory = mmap(NULL, babblespace_size,
                                             PROT_READ | PROT_WRITE,
                                             MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    // Zamknij deskryptor pliku, ponieważ mmap już działa z segmentem pamięci
    close(shm_fd);

    // Jeśli segment został nowo utworzony, zainicjalizuj strukturę
    if (shared_memory->babble_total == 0 && shared_memory->babble_first == 0) {
        printf("Inicjalizacja segmentu pamięci współdzielonej...\n");
        initialize_shared_memory(shared_memory);
    }

    // Wyświetl komunikaty znajdujące się w pamięci współdzielonej
    pthread_mutex_lock(&shared_memory->babble_mutex);  // Zablokuj mutex

    int first = shared_memory->babble_first;
    int total = shared_memory->babble_total;

    printf("Wyświetlanie istniejących komunikatów (łącznie: %d):\n", total);

    // Wyświetlenie komunikatów w odpowiedniej kolejności
    for (int i = 0; i < total; ++i) {
        int index = (first + i) % BABBLE_LIMIT;
        printf("%d: %s\n", i + 1, shared_memory->babbles[index]);
    }

    pthread_mutex_unlock(&shared_memory->babble_mutex);  // Odblokuj mutex

    // Odmapowanie pamięci przed zakończeniem programu
    if (munmap(shared_memory, babblespace_size) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    return 0;
}
