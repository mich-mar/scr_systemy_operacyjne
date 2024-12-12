#include <openssl/md5.h>
#include <openssl/evp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <pthread.h>

// definiujemy maksymalne rozmiary różnych tablic i stałych używanych w programie
#define MAX_HASHES_SIZE 1000 // maksymalna liczba hashów
#define MD5_HASH_LENGTH 32 // długość hashów MD5
#define MAX_WORD_LENGTH 256 // maksymalna długość słowa
#define MAX_DICTIONARY_SIZE 1000 // maksymalna liczba słów w słowniku
#define NUM_THREADS 12 // liczba wątków (po jednym na każdy przypadek)

// deklaracja globalnych zmiennych
char **dict; // tablica dynamiczna na słownik
char hashes[MAX_HASHES_SIZE][MD5_HASH_LENGTH + 1]; // tablica hashów
char mails[MAX_HASHES_SIZE][MAX_WORD_LENGTH + 1]; // tablica emaili
int found[MAX_HASHES_SIZE] = {0}; // tablica statusu znalezienia hasła dla każdego hashu
int hash_count = 0; // liczba załadowanych hashów
int dict_size = 0; // liczba słów w słowniku
pthread_mutex_t found_mutex; // mutex do synchronizacji operacji na tablicy `found`

// funkcja konwertująca ciąg znaków na hash MD5
void bytes2md5(const char *data, char *md5buf) {
    int len = strlen(data);
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new(); // tworzymy nowy kontekst MD5
    const EVP_MD *md = EVP_md5(); // pobieramy funkcję hashującą MD5
    unsigned char md_value[EVP_MAX_MD_SIZE]; // tablica na wynik funkcji hashującej
    unsigned int md_len, i;

    // inicjalizujemy, aktualizujemy i finalizujemy proces hashowania
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, data, len);
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    // zamieniamy wynik hashowania na ciąg znaków w formacie szesnastkowym
    for (i = 0; i < md_len; i++) {
        snprintf(&(md5buf[i * 2]), 16 * 2, "%02x", md_value[i]);
    }
}

// funkcja ładująca słownik z pliku
int loadDictionary(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Nie udało się otworzyć pliku słownika: %s\n", filename);
        exit(-1);
    }

    dict = malloc(MAX_DICTIONARY_SIZE * sizeof(char *)); // alokujemy tablicę wskaźników
    if (!dict) {
        fprintf(stderr, "Błąd alokacji pamięci dla tablicy słów.\n");
        exit(-1);
    }

    int count = 0;
    while (count < MAX_DICTIONARY_SIZE) {
        dict[count] = malloc((MAX_WORD_LENGTH + 1) * sizeof(char)); // alokujemy pamięć dla pojedynczego słowa
        if (!dict[count]) {
            fprintf(stderr, "Błąd alokacji pamięci dla słowa nr %d.\n", count);
            exit(-1);
        }
        if (fscanf(file, "%255s", dict[count]) != 1) { // odczytujemy słowo z pliku
            free(dict[count]);
            break;
        }
        count++;
    }

    fclose(file);
    return count;
}

// funkcja ładująca hashe z pliku
int loadHashes(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Nie udało się otworzyć pliku haseł: %s\n", filename);
        exit(-1);
    }

    int hash_count = 0;
    char line[512]; // bufor na pojedynczą linię

    while (fgets(line, sizeof(line), file) && hash_count < MAX_HASHES_SIZE) {
        char hash[MD5_HASH_LENGTH + 1];
        char mail[MAX_WORD_LENGTH + 1];
        char id[32]; // zmienna na ID
        char rest[256]; // zmienna na pozostałą część linii

        // odczytujemy trzy pierwsze kolumny linii (ignorujemy czwartą)
        if (sscanf(line, "%31s %32s %255s %[^\n]", id, hash, mail, rest) < 3) {
            fprintf(stderr, "Błąd odczytu linii: %s\n", line);
            continue;
        }

        // kopiujemy hash i email do odpowiednich tablic
        strcpy(hashes[hash_count], hash);
        strcpy(mails[hash_count], mail);

        hash_count++;
    }

    fclose(file);
    return hash_count;
}

// funkcja sprawdzająca, czy dane słowo odpowiada któremuś z hashów
void checkHashMatch(const char *word, int hash_count) {
    char hash_result[MD5_HASH_LENGTH + 1];
    bytes2md5(word, hash_result); // generujemy hash dla podanego słowa

    for (int j = 0; j < hash_count; j++) {
        pthread_mutex_lock(&found_mutex); // blokujemy mutex
        if (strcmp(hash_result, hashes[j]) == 0 && !found[j]) { // sprawdzamy zgodność hashów
            found[j] = 1; // oznaczamy hash jako znaleziony
            printf("Password for %s is %s\n", mails[j], word); // drukujemy wynik
        }
        pthread_mutex_unlock(&found_mutex); // odblokowujemy mutex
    }
}

// funkcja zwalniająca pamięć używaną przez słownik
void freeDictionary(int dict_size) {
    for (int i = 0; i < dict_size; ++i) {
        free(dict[i]); // zwalniamy pamięć dla każdego słowa
    }
    free(dict); // zwalniamy tablicę wskaźników
}

// funkcja wykonywana przez każdy wątek
void *threadFunction(void *arg) {
    int thread_id = *(int *)arg; // odczytujemy identyfikator wątku

    for (int i = 0; i < dict_size; ++i) { // iterujemy po każdym słowie w słowniku
        char modified_word[MAX_WORD_LENGTH + 1];
        char word_lower[MAX_WORD_LENGTH + 1] = "";
        char word_upper[MAX_WORD_LENGTH + 1] = "";
        char word_capitalized[MAX_WORD_LENGTH + 1] = "";

        // przygotowujemy różne wariacje słowa w zależności od ID wątku
        if (thread_id == 0 || thread_id == 3 || thread_id == 4 || thread_id == 5) {
            for (int j = 0; dict[i][j]; ++j)
                word_lower[j] = tolower(dict[i][j]); // wersja z małymi literami
            word_lower[strlen(dict[i])] = '\0';
        }

        if (thread_id == 1 || thread_id == 6 || thread_id == 7 || thread_id == 8) {
            for (int j = 0; dict[i][j]; ++j)
                word_upper[j] = toupper(dict[i][j]); // wersja z dużymi literami
            word_upper[strlen(dict[i])] = '\0';
        }

        if (thread_id == 2 || thread_id == 9 || thread_id == 10 || thread_id == 11) {
            for (int j = 0; dict[i][j]; ++j)
                word_capitalized[j] = tolower(dict[i][j]); // wersja z pierwszą literą dużą
            word_capitalized[0] = toupper(dict[i][0]);
            word_capitalized[strlen(dict[i])] = '\0';
        }

        // w zależności od identyfikatora wątku stosujemy różne modyfikacje słów
        switch (thread_id) {
            case 0: // małe litery
                checkHashMatch(word_lower, hash_count);
                break;

            case 1: // duże litery
                checkHashMatch(word_upper, hash_count);
                break;

            case 2: // pierwsza litera duża
                checkHashMatch(word_capitalized, hash_count);
                break;

            case 3: // liczba z przodu, małe litery
                for (int num = 0; num <= 99; ++num) {
                    snprintf(modified_word, MAX_WORD_LENGTH, "%d%s", num, word_lower);
                    checkHashMatch(modified_word, hash_count);
                }
                break;

            case 4: // liczba z tyłu, małe litery
                for (int num = 0; num <= 99; ++num) {
                    snprintf(modified_word, MAX_WORD_LENGTH, "%s%d", word_lower, num);
                    checkHashMatch(modified_word, hash_count);
                }
                break;

            case 5: // liczba z przodu i tyłu, małe litery
                for (int num1 = 0; num1 <= 99; ++num1) {
                    for (int num2 = 0; num2 <= 99; ++num2) {
                        snprintf(modified_word, MAX_WORD_LENGTH, "%d%s%d", num1, word_lower, num2);
                        checkHashMatch(modified_word, hash_count);
                    }
                }
                break;

            // kolejne przypadki dla dużych liter i kapitalizowanych słów
            case 6:
                for (int num = 0; num <= 99; ++num) {
                    snprintf(modified_word, MAX_WORD_LENGTH, "%d%s", num, word_upper);
                    checkHashMatch(modified_word, hash_count);
                }
                break;

            case 7:
                for (int num = 0; num <= 99; ++num) {
                    snprintf(modified_word, MAX_WORD_LENGTH, "%s%d", word_upper, num);
                    checkHashMatch(modified_word, hash_count);
                }
                break;

            case 8:
                for (int num1 = 0; num1 <= 99; ++num1) {
                    for (int num2 = 0; num2 <= 99; ++num2) {
                        snprintf(modified_word, MAX_WORD_LENGTH, "%d%s%d", num1, word_upper, num2);
                        checkHashMatch(modified_word, hash_count);
                    }
                }
                break;

            case 9:
                for (int num = 0; num <= 99; ++num) {
                    snprintf(modified_word, MAX_WORD_LENGTH, "%d%s", num, word_capitalized);
                    checkHashMatch(modified_word, hash_count);
                }
                break;

            case 10:
                for (int num = 0; num <= 99; ++num) {
                    snprintf(modified_word, MAX_WORD_LENGTH, "%s%d", word_capitalized, num);
                    checkHashMatch(modified_word, hash_count);
                }
                break;

            case 11:
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

// funkcja wypisująca wszystkie załadowane hashe i emaile
void printHashesAndMails(int hash_count) {
    printf("=== Hashes and Emails ===\n");
    for (int i = 0; i < hash_count; i++) {
        printf("Hash %d: %s, Email: %s\n", i, hashes[i], mails[i]);
    }
    printf("=========================\n");
}

// funkcja wypisująca wszystkie słowa ze słownika
void printDictionary(int dict_size) {
    printf("=== Dictionary Words ===\n");
    for (int i = 0; i < dict_size; i++) {
        printf("Word %d: %s\n", i, dict[i]);
    }
    printf("========================\n");
}

// funkcja główna programu
int main() {
    pthread_t threads[NUM_THREADS]; // tablica wątków
    int thread_ids[NUM_THREADS]; // identyfikatory wątków

    pthread_mutex_init(&found_mutex, NULL); // inicjalizujemy mutex

    // ładujemy dane z plików
    dict_size = loadDictionary("slowniki/slownik_3.txt");
    hash_count = loadHashes("hasla/hasla_3.txt");

    // uruchamiamy wątki
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, threadFunction, &thread_ids[i]);
    }

    // czekamy na zakończenie wątków
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // zwalniamy zasoby
    freeDictionary(dict_size);
    pthread_mutex_destroy(&found_mutex);

    return 0;
}
