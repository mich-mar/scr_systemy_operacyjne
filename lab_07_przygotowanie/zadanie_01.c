#include <stdio.h>    
#include <stdlib.h>   
#include <fcntl.h>     
#include <sys/mman.h>  
#include <unistd.h>    
#include <pthread.h>    
#include <errno.h>      
#include <string.h>    

// stałe z plecenia do lab_07
#define BABBLE_NAME "/Y01-42e"     
#define BABBLE_MODE 0777           
#define BABBLE_LIMIT 10          
#define BABBLE_LENGTH 80        

// struktura przechowująca dane w pamięci współdzielonej
struct babblespace {
    pthread_mutex_t babble_mutex; 
    pthread_cond_t babble_cond;  
    int babble_first;           
    int babble_total;          
    char babbles[BABBLE_LIMIT][BABBLE_LENGTH]; 
};

int main() {
    int shm_fd;                           
    struct babblespace *shared_memory;       // wskaźnik do segmentu pamięci współdzielonej (musi być "struct" w c)

    // 1a) otwórz istniejący segment pamięci współdzielonej
    shm_fd = shm_open(BABBLE_NAME, O_RDWR, BABBLE_MODE);

    // sprawdź czy otwarcie pamięci się udało
    if (shm_fd == -1) {
        perror("Blad podczas otwarcia pamieci wspolnej\n");
        exit(0);
    }

    // 1b) mapowanie segmentu pamięci współdzielonej
    shared_memory = (struct babblespace *)mmap(NULL, sizeof(struct babblespace),
                                             PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // sprawdź czy mapowanie się udało
    if (shared_memory == MAP_FAILED) {
        perror("Mapowanie pamieci wspolnej sie nie powiodlo\n");
        close(shm_fd);
        exit(0);
    }

    // 2) zablokowanie pamięci współdzielonej przy użyciu mutexa
    pthread_mutex_lock(&(shared_memory->babble_mutex));

    // 1c) wyświetlenie istniejących komunikatów w pamięci
    printf("Komunikaty w pamieci:\n");

    // ustawienie wskaźnika na pierwszy komunikat
    int current_index = shared_memory->babble_first;

    // pętla po wszystkich dostępnych komunikatach
    for (int i = 0; i < shared_memory->babble_total; i++) {
        printf("Wiadomosc nr%d: %s\n", i + 1, shared_memory->babbles[current_index]);
        current_index = current_index + 1;
    }

    // 2) odblokowanie pamięci współdzielonej po zakończeniu operacji
    pthread_mutex_unlock(&(shared_memory->babble_mutex));

    // odmapowanie segmentu pamięci wspólnej
    munmap(shared_memory, sizeof(struct babblespace));

    // zamknięcie deskryptora segmentu pamięci
    close(shm_fd);

    return 0;
}
