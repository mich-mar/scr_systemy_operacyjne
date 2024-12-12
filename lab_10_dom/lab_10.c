#include <openssl/md5.h>
#include <openssl/evp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>


#define MAX_HASHES_SIZE 1000
#define MD5_HASH_LENGTH 32
#define MAX_WORD_LENGTH 256
#define MAX_DICTIONARY_SIZE 1000
#define NUM_THREADS 12
#define QUEUE_SIZE 100

char **dict;
char hashes[MAX_HASHES_SIZE][MD5_HASH_LENGTH + 1];
char mails[MAX_HASHES_SIZE][MAX_WORD_LENGTH + 1];
int found_flags[MAX_HASHES_SIZE] = {0};

int hash_count = 0;
int dict_size = 0;
int queue_head = 0, queue_tail = 0;
bool stop_processing = false;

pthread_mutex_t found_mutex;

typedef struct {
    char hash[MD5_HASH_LENGTH + 1];
    char email[MAX_WORD_LENGTH + 1];
} CrackedPassword;

CrackedPassword queue[QUEUE_SIZE];
CrackedPassword found[MAX_HASHES_SIZE];

pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_not_full = PTHREAD_COND_INITIALIZER;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;


void bytes2md5(const char *data, char *md5buf) {
    int len = strlen(data);
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_md5();
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;

    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, data, len);
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    for (i = 0; i < md_len; i++) {
        snprintf(&(md5buf[i * 2]), 16 * 2, "%02x", md_value[i]);
    }
}

int loadDictionary(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (!file) {
        fprintf(stderr, "Nie udało się otworzyć pliku słownika: %s\n", filename);
        exit(-1);
    }

    dict = malloc(MAX_DICTIONARY_SIZE * sizeof(char *));
    if (!dict) {
        fprintf(stderr, "Błąd alokacji pamięci dla tablicy słów.\n");
        exit(-1);
    }

    int count = 0;

    while (count < MAX_DICTIONARY_SIZE) {
        dict[count] = malloc((MAX_WORD_LENGTH + 1) * sizeof(char));
        if (!dict[count]) {
            fprintf(stderr, "Błąd alokacji pamięci dla słowa nr %d.\n", count);
            exit(-1);
        }

        if (fscanf(file, "%255s", dict[count]) != 1) {
            free(dict[count]);
            break;
        }
        count++;
    }

    fclose(file);
    return count;
}

int loadHashes(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (!file) {
        fprintf(stderr, "Nie udało się otworzyć pliku haseł: %s\n", filename);
        exit(-1);
    }

    int hash_count = 0;
    char line[512]; // Bufor na pojedynczą linię

    while (fgets(line, sizeof(line), file) && hash_count < MAX_HASHES_SIZE) {
        char hash[MD5_HASH_LENGTH + 1];
        char mail[MAX_WORD_LENGTH + 1];
        char id[32]; // ID placeholder
        char rest[256]; // Placeholder na resztę linii

        // Odczytaj pierwsze trzy kolumny, a czwartą ignoruj
        if (sscanf(line, "%31s %32s %255s %[^\n]", id, hash, mail, rest) < 3) {
            fprintf(stderr, "Błąd odczytu linii: %s\n", line);
            continue;
        }

        // Skopiuj hash i email do odpowiednich tablic
        strcpy(hashes[hash_count], hash);
        strcpy(mails[hash_count], mail);

        hash_count++;
    }

    fclose(file);
    return hash_count;
}


void checkHashMatch(const char *word, int hash_count) {
    char hash_result[MD5_HASH_LENGTH + 1];
    bytes2md5(word, hash_result);

    for (int j = 0; j < hash_count; j++) {
        pthread_mutex_lock(&found_mutex);
        if (strcmp(hash_result, hashes[j]) == 0 && !found_flags[j]) {
            found_flags[j] = 1;
            found

            pthread_mutex_lock(&queue_mutex);
            while ((queue_head + 1) % QUEUE_SIZE == queue_tail) {
                pthread_cond_wait(&queue_not_full, &queue_mutex);
            }

            strcpy(queue[queue_head].hash, hashes[j]);
            strcpy(queue[queue_head].email, mails[j]);
            queue_head = (queue_head + 1) % QUEUE_SIZE;

            pthread_cond_signal(&queue_not_empty);
            pthread_mutex_unlock(&queue_mutex);
        }
        pthread_mutex_unlock(&found_mutex);
    }
}

void freeDictionary(int dict_size) {
    for (int i = 0; i < dict_size; ++i) {
        free(dict[i]);
    }
    free(dict);
}

void *threadFunction(void *arg) {
    int thread_id = *(int *) arg;

    for (int i = 0; i < dict_size; ++i) {
        char modified_word[MAX_WORD_LENGTH + 1];
        char word_lower[MAX_WORD_LENGTH + 1] = "";
        char word_upper[MAX_WORD_LENGTH + 1] = "";
        char word_capitalized[MAX_WORD_LENGTH + 1] = "";

        // Precompute variations of the dictionary word as needed
        if (thread_id == 0 || thread_id == 3 || thread_id == 4 || thread_id == 5) {
            for (int j = 0; dict[i][j]; ++j)
                word_lower[j] = tolower(dict[i][j]);
            word_lower[strlen(dict[i])] = '\0';
        }

        if (thread_id == 1 || thread_id == 6 || thread_id == 7 || thread_id == 8) {
            for (int j = 0; dict[i][j]; ++j)
                word_upper[j] = toupper(dict[i][j]);
            word_upper[strlen(dict[i])] = '\0';
        }

        if (thread_id == 2 || thread_id == 9 || thread_id == 10 || thread_id == 11) {
            for (int j = 0; dict[i][j]; ++j)
                word_capitalized[j] = tolower(dict[i][j]);
            word_capitalized[0] = toupper(dict[i][0]);
            word_capitalized[strlen(dict[i])] = '\0';
        }

        switch (thread_id) {
            case 0: // Lowercase
                checkHashMatch(word_lower, hash_count);
                break;

            case 1: // Uppercase
                checkHashMatch(word_upper, hash_count);
                break;

            case 2: // Capitalized
                checkHashMatch(word_capitalized, hash_count);
                break;

            case 3: // Number prefix, lowercase
                for (int num = 0; num <= 99; ++num) {
                    snprintf(modified_word, MAX_WORD_LENGTH, "%d%s", num, word_lower);
                    checkHashMatch(modified_word, hash_count);
                }
                break;

            case 4: // Number suffix, lowercase
                for (int num = 0; num <= 99; ++num) {
                    snprintf(modified_word, MAX_WORD_LENGTH, "%s%d", word_lower, num);
                    checkHashMatch(modified_word, hash_count);
                }
                break;

            case 5: // Number prefix and suffix, lowercase
                for (int num1 = 0; num1 <= 99; ++num1) {
                    for (int num2 = 0; num2 <= 99; ++num2) {
                        snprintf(modified_word, MAX_WORD_LENGTH, "%d%s%d", num1, word_lower, num2);
                        checkHashMatch(modified_word, hash_count);
                    }
                }
                break;

            case 6: // Number prefix, uppercase
                for (int num = 0; num <= 99; ++num) {
                    snprintf(modified_word, MAX_WORD_LENGTH, "%d%s", num, word_upper);
                    checkHashMatch(modified_word, hash_count);
                }
                break;

            case 7: // Number suffix, uppercase
                for (int num = 0; num <= 99; ++num) {
                    snprintf(modified_word, MAX_WORD_LENGTH, "%s%d", word_upper, num);
                    checkHashMatch(modified_word, hash_count);
                }
                break;

            case 8: // Number prefix and suffix, uppercase
                for (int num1 = 0; num1 <= 99; ++num1) {
                    for (int num2 = 0; num2 <= 99; ++num2) {
                        snprintf(modified_word, MAX_WORD_LENGTH, "%d%s%d", num1, word_upper, num2);
                        checkHashMatch(modified_word, hash_count);
                    }
                }
                break;

            case 9: // Number prefix, capitalized
                for (int num = 0; num <= 99; ++num) {
                    snprintf(modified_word, MAX_WORD_LENGTH, "%d%s", num, word_capitalized);
                    checkHashMatch(modified_word, hash_count);
                }
                break;

            case 10: // Number suffix, capitalized
                for (int num = 0; num <= 99; ++num) {
                    snprintf(modified_word, MAX_WORD_LENGTH, "%s%d", word_capitalized, num);
                    checkHashMatch(modified_word, hash_count);
                }
                break;

            case 11: // Number prefix and suffix, capitalized
                for (int num1 = 0; num1 <= 99; ++num1) {
                    for (int num2 = 0; num2 <= 99; ++num2) {
                        snprintf(modified_word, MAX_WORD_LENGTH, "%d%s%d", num1, word_capitalized, num2);
                        checkHashMatch(modified_word, hash_count);
                    }
                }
                break;
        }
    }
    return NULL;
}

void printHashesAndMails(int hash_count) {
    printf("=== Hashes and Emails ===\n");
    for (int i = 0; i < hash_count; i++) {
        printf("Hash %d: %s, Email: %s\n", i, hashes[i], mails[i]);
    }
    printf("=========================\n");
}

void printDictionary(int dict_size) {
    printf("=== Dictionary Words ===\n");
    for (int i = 0; i < dict_size; i++) {
        printf("Word %d: %s\n", i, dict[i]);
    }
    printf("========================\n");
}

void sighupHandler(int signum) {
    pthread_mutex_lock(&queue_mutex);
    printf("\n=== Złamane hasła na sygnał SIGHUP ===\n");
    for (int i = 0; i < hash_count; i++) {
        if (found[i]) {
            printf("Hash: %s, Email: %s\n", hashes[i], mails[i]);
        }
    }
    printf("======================================\n");
    pthread_mutex_unlock(&queue_mutex);
}

void *consumerThread(void *arg) {
    while (!stop_processing) {
        pthread_mutex_lock(&queue_mutex);
        while (queue_head == queue_tail && !stop_processing) {
            pthread_cond_wait(&queue_not_empty, &queue_mutex);
        }

        if (stop_processing) {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }

        CrackedPassword password = queue[queue_tail];
        queue_tail = (queue_tail + 1) % QUEUE_SIZE;

        printf("Złamane hasło: Hash: %s, Email: %s\n", password.hash, password.email);
        pthread_cond_signal(&queue_not_full);
        pthread_mutex_unlock(&queue_mutex);
    }
    return NULL;
}


int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    pthread_t consumer_thread;

    pthread_create(&consumer_thread, NULL, consumerThread, NULL);

    // Tworzenie wątków producentów
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, threadFunction, &thread_ids[i]);
    }

    // Dołączanie wątków producentów
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Informowanie konsumenta o zakończeniu
    pthread_mutex_lock(&queue_mutex);
    stop_processing = true;
    pthread_cond_signal(&queue_not_empty);
    pthread_mutex_unlock(&queue_mutex);

    // Dołączenie konsumenta
    pthread_join(consumer_thread, NULL);

    // Finalne wypisanie złamanych haseł
    printf("\n=== Ostateczne złamane hasła ===\n");
    for (int i = 0; i < hash_count; i++) {
        if (found[i]) {
            printf("Hash: %s, Email: %s\n", hashes[i], mails[i]);
        }
    }
    printf("================================\n");

    freeDictionary(dict_size);
    pthread_mutex_destroy(&found_mutex);
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_not_empty);
    pthread_cond_destroy(&queue_not_full);
}
