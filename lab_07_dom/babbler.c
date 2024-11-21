#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

// stale z polecenia do lab_07
#define BABBLE_NAME "/michal_michal"
#define BABBLE_MODE 0777
#define BABBLE_LIMIT 10
#define BABBLE_LENGTH 80

// struktura przechowujaca dane w pamieci wspolnej
struct babblespace {
    pthread_mutex_t babble_mutex;
    pthread_cond_t babble_cond;
    int babble_first;
    int babble_total;
    char babbles[BABBLE_LIMIT][BABBLE_LENGTH];
};

// funkcja do otwierania segmentu pamięci wspólnej
int open_shared_memory() {
    int shm_fd = shm_open(BABBLE_NAME, O_RDWR, BABBLE_MODE);

    if (shm_fd == -1) {
        perror(" Blad podczas otwarcia pamieci wspolnej\n");
        exit(0);
    } else {
        printf(" Otwarcie pamieci wspolnej sie powiodlo\n");
    }

    return shm_fd;
}

// funkcja do mapowania segmentu pamięci wspólnej
struct babblespace *map_shared_memory(int shm_fd) {
    struct babblespace *shared_memory = (struct babblespace *) mmap(NULL, sizeof(struct babblespace),
                                                                    PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shared_memory == MAP_FAILED) {
        perror(" Mapowanie pamieci wspolnej sie nie powiodlo\n");
        close(shm_fd);
        exit(0);
    } else {
        printf(" Mapowanie pamieci wspolnej sie powiodlo\n");
    }

    return shared_memory;
}

// funkcja do wyświetlania komunikatów z pamięci wspólnej
void display_messages(struct babblespace *shared_memory) {
    // zablokowanie pamięci wspólnej
    pthread_mutex_lock(&(shared_memory->babble_mutex));

    if (shared_memory->babble_total > 0)
        printf("\n Komunikaty w pamieci:\n");
    else
        printf("\n Brak komunikatow w pamieci :(\n");

    int current_index = shared_memory->babble_first;

    for (int i = 0; i < shared_memory->babble_total; i++) {
        printf("  Wiadomosc nr %d: %s", i + 1, shared_memory->babbles[current_index]);

        // ustalanie indexu zgodnie z buforem kolowym
        current_index = (current_index + 1) % BABBLE_LIMIT;
    }

    // odblokowanie pamięci wspólnej
    pthread_mutex_unlock(&(shared_memory->babble_mutex));
}

// funkcja do pobierania komunikatu od użytkownika i dodawania go do pamięci wspólnej
void add_user_message(struct babblespace *shared_memory) {
    char user_message[BABBLE_LENGTH];
    char formatted_message[BABBLE_LENGTH];

    printf("\n Wpisz swoj komunikat (maksymalnie %d znakow): ", BABBLE_LENGTH - 5);
    fgets(user_message, BABBLE_LENGTH, stdin);

    // "scalanie" dwóch wiadomości
    snprintf(formatted_message, BABBLE_LENGTH, "[MM] %s", user_message);

    // blokada pamięci wspólnej za pomocą mutexa przed wpisaniem nowego komunikatu
    pthread_mutex_lock(&(shared_memory->babble_mutex));

    int new_message_idx = (shared_memory->babble_first + shared_memory->babble_total) % BABBLE_LIMIT;

    // wpisanie komunikatu do tablicy babbles
    strncpy(shared_memory->babbles[new_message_idx], formatted_message, BABBLE_LENGTH - 1);

    // aktualizacja parametrów bufora kolowego
    if (shared_memory->babble_total < BABBLE_LIMIT)
        shared_memory->babble_total++;
    else
        shared_memory->babble_first = (shared_memory->babble_first + 1) % BABBLE_LIMIT;


    // odblokowanie pamięci wspólnej po dodaniu komunikatu
    pthread_mutex_unlock(&(shared_memory->babble_mutex));
}

// funkcja pytająca o decyzję użytkownika
int ask_for_decision() {
    char decision_char[100];
    int decision_int = 0;

    while (decision_int != 1 && decision_int != 2 && decision_int != 3) {
        printf("\n===== MENU ====\n");
        printf("1) Wypisz komunikaty\n");
        printf("2) Wyślij wiadomość\n");
        printf("3) Zakończ program\n");
        printf("Wybierz opcję: ");

        // pobranie wyboru użytkownika
        fgets(decision_char, sizeof(decision_char), stdin);

        // Konwersja tekstu na liczbę
        decision_int = atoi(decision_char);

        // Sprawdzenie poprawności wyboru
        if (decision_int != 1 && decision_int != 2 && decision_int != 3) {
            printf("Niepoprawny wybór. Spróbuj ponownie.\n");
        }
    }

    return decision_int;
}


// Komenda do tworzenia pamięci wspólnej
// (cd ~/../witold/ && ./babbler_lin /michal_michal INIT)
int main() {
    int decision = 0;
    bool loop = true;

    // otwieranie segmentu pamięci wspólnej
    int shm_fd = open_shared_memory();

    // Mapowanie segmentu pamięci wspólnej
    struct babblespace *shared_memory = map_shared_memory(shm_fd);

    while (loop) {
        decision = ask_for_decision();

        switch (decision) {
            case 1:
                display_messages(shared_memory);
                break;

            case 2:
                add_user_message(shared_memory);
                break;

            case 3:
                printf("\n Koniec programu\n");
                loop = false;
                break;

            default: break;
        }
    }

    // odmapowanie segmentu pamięci wspólnej
    munmap(shared_memory, sizeof(struct babblespace));

    // zamknięcie deskryptora segmentu pamięci
    close(shm_fd);

    return 0;
}
