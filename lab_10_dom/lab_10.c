#include <openssl/md5.h>
#include <openssl/evp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#define MAX_HASHES_SIZE 1000
#define MD5_HASH_LENGTH 32
#define MAX_WORD_LENGTH 256
#define MAX_QUEUE_SIZE 100

typedef struct {
    char **dict;
    int dict_size;
    int hash_count;
} ProducerArgs;

typedef struct {
    char password[MAX_WORD_LENGTH + 1];
    int hash_index;
} CrackedPassword;

pthread_mutex_t found_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

char **dict;
char hashes[MAX_HASHES_SIZE][MD5_HASH_LENGTH + 1];
char mails[MAX_HASHES_SIZE][MAX_WORD_LENGTH + 1];
int found[MAX_HASHES_SIZE] = {0};
CrackedPassword cracked_queue[MAX_QUEUE_SIZE];
int queue_start = 0, queue_end = 0;
int stop_processing = 0;
int sighup_received = 0;
pthread_cond_t sighup_cond = PTHREAD_COND_INITIALIZER;


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

int loadDictionary(const char *filename, int max_words) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Nie udało się otworzyć pliku słownika: %s\n", filename);
        exit(-1);
    }

    dict = malloc(max_words * sizeof(char*));
    if (!dict) {
        fprintf(stderr, "Błąd alokacji pamięci dla tablicy słów.\n");
        exit(-1);
    }

    int count = 0;
    while (count < max_words) {
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

int loadHashes(const char *filename, int max_hashes) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Nie udało się otworzyć pliku haseł: %s\n", filename);
        exit(-1);
    }

    int count = 0, hash_count = 0, mail_count = 0;
    char temp[MD5_HASH_LENGTH + 1];
    while (fscanf(file, "%32s", temp) == 1 && count < max_hashes) {
        if (count % 4 == 1) {
            strcpy(hashes[hash_count], temp);
            hash_count++;
        }
        if (count % 4 == 2) {
            strcpy(mails[mail_count], temp);
            mail_count++;
        }
        count++;
    }
    fclose(file);
    return hash_count;
}

void queueCrackedPassword(const char *password, int hash_index) {
    pthread_mutex_lock(&queue_mutex);
    while ((queue_end + 1) % MAX_QUEUE_SIZE == queue_start) {
        pthread_cond_wait(&queue_cond, &queue_mutex);
    }
    strcpy(cracked_queue[queue_end].password, password);
    cracked_queue[queue_end].hash_index = hash_index;
    queue_end = (queue_end + 1) % MAX_QUEUE_SIZE;
    pthread_cond_signal(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);
}

void checkHashMatch(const char *password, int hash_count) {
    char md5_hash[MD5_HASH_LENGTH + 1];
    bytes2md5(password, md5_hash);
    // printf("Checking password: %s, MD5: %s\n", password, md5_hash); // Debugging
    for (int i = 0; i < hash_count; i++) {
        if (strcmp(md5_hash, hashes[i]) == 0 && !found[i]) {
            found[i] = 1;
            queueCrackedPassword(password, i);
            // printf("Found matching hash: %s\n", password); // Debugging
        }
    }
}


void checkNumberCombinations(const char *word, int hash_count) {
    char modified_word[MAX_WORD_LENGTH + 1];

    for (int num_1 = 0; num_1 <= 99; ++num_1) {
        sprintf(modified_word, "%d%s", num_1, word);
        checkHashMatch(modified_word, hash_count);

        sprintf(modified_word, "%s%d", word, num_1);
        checkHashMatch(modified_word, hash_count);

        for (int num_2 = 0; num_2 <= 99; ++num_2) {
            sprintf(modified_word, "%d%s%d", num_2, word, num_1);
            checkHashMatch(modified_word, hash_count);
        }
    }
}



void generateCaseVariants(const char *word, char lower[], char upper[], char capitalized[]) {
    int length = strlen(word);
    for (int i = 0; i < length; ++i) {
        lower[i] = tolower(word[i]);
        capitalized[i] = tolower(word[i]);
    }
    capitalized[0] = toupper(word[0]);
    for (int i = 0; i < length; ++i) {
        upper[i] = toupper(word[i]);
    }
    upper[length] = '\0';
    lower[length] = '\0';
    capitalized[length] = '\0';
}

void *consumerThread(void *arg) {
    while (!stop_processing || queue_start != queue_end) {
        pthread_mutex_lock(&queue_mutex);
        while (queue_start == queue_end && !stop_processing) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }
        if (queue_start != queue_end) {
            CrackedPassword cp = cracked_queue[queue_start];
            queue_start = (queue_start + 1) % MAX_QUEUE_SIZE;
            pthread_mutex_unlock(&queue_mutex);

            printf("Złamane hasło: %s odpowiada mailowi: %s\n", cp.password, mails[cp.hash_index]);
        } else {
            pthread_mutex_unlock(&queue_mutex);
        }
    }
    return NULL;
}

void handle_sighup(int signum) {
    pthread_mutex_lock(&found_mutex);
    sighup_received = 1;
    pthread_cond_signal(&sighup_cond);
    pthread_mutex_unlock(&found_mutex);
}

void* consumer_display_results(void* args) {
    while (1) {
        pthread_mutex_lock(&found_mutex);
        while (!sighup_received) {
            pthread_cond_wait(&sighup_cond, &found_mutex);
        }

        sighup_received = 0;
        printf("-> Lista złamanych haseł:\n");
        for (int i = 0; i < MAX_HASHES_SIZE; ++i) {
            if (found[i]) {
                printf("Hasło: %s, Mail: %s\n", hashes[i], mails[i]);
            }
        }
        pthread_mutex_unlock(&found_mutex);
    }

    pthread_exit(NULL);
}

void freeDictionary(int dict_size) {
    for (int i = 0; i < dict_size; ++i) {
        free(dict[i]);
    }
    free(dict);
}

void* producer_case_variants(void* args) {
    ProducerArgs* p_args = (ProducerArgs*)args;

    for (int i = 0; i < p_args->dict_size; ++i) {
        char lower[MAX_WORD_LENGTH + 1], upper[MAX_WORD_LENGTH + 1], capitalized[MAX_WORD_LENGTH + 1];
        generateCaseVariants(p_args->dict[i], lower, upper, capitalized);

        checkHashMatch(lower, p_args->hash_count);
        checkHashMatch(capitalized, p_args->hash_count);
        checkHashMatch(upper, p_args->hash_count);
    }

    pthread_exit(NULL);
}

void* producer_number_combinations(void* args) {
    ProducerArgs* p_args = (ProducerArgs*)args;

    for (int i = 0; i < p_args->dict_size; ++i) {
        checkNumberCombinations(p_args->dict[i], p_args->hash_count);
    }

    pthread_exit(NULL);
}


// flagi: -lcrypto
int main() {
    int dict_size, hash_size;

    dict_size = loadDictionary("slowniki/slownik_2.txt", MAX_HASHES_SIZE);
    hash_size = loadHashes("hasla/hasla_2.txt", MAX_HASHES_SIZE);

    dict_size = loadDictionary("slowniki/slownik_5.txt", MAX_HASHES_SIZE);
    hash_size = loadHashes("hasla/hasla_5.txt", MAX_HASHES_SIZE);
    printf("Loaded dictionary with %d words\n", dict_size);
    printf("Loaded %d hashes\n", hash_size);


    signal(SIGHUP, handle_sighup);

    pthread_t producer1, producer2, consumer;
    ProducerArgs args = {dict, dict_size, hash_size};

    pthread_create(&producer1, NULL, producer_case_variants, &args);
    pthread_create(&producer2, NULL, producer_number_combinations, &args);
    pthread_create(&consumer, NULL, consumer_display_results, NULL);

    pthread_join(producer1, NULL);
    pthread_join(producer2, NULL);

    // Wyświetlenie wyników końcowych
    raise(SIGHUP);

    pthread_cancel(consumer); // Zatrzymaj wątek konsumenta
    pthread_join(consumer, NULL);

    printf("-> Lista złamanych haseł:\n");
    for (int i = 0; i < MAX_HASHES_SIZE; ++i) {
        if (found[i]) {
            printf("Hasło: %s, Mail: %s\n", hashes[i], mails[i]);
        }
    }

    freeDictionary(dict_size);
    return 0;
}

