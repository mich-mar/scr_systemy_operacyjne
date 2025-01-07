#include <openssl/md5.h>
#include <openssl/evp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_HASHES_SIZE 1000
#define MD5_HASH_LENGTH 32
#define MAX_WORD_LENGTH 256
#define MAX_DICTIONARY_SIZE 1000

char **dict;
char hashes[MAX_HASHES_SIZE][MD5_HASH_LENGTH + 1];
char mails[MAX_HASHES_SIZE][MAX_WORD_LENGTH + 1];
int found[MAX_HASHES_SIZE] = {0};
int hash_count = 0;

// funkcja zaimportowana ze strony "https://kcir.pwr.edu.pl/~krzewnicka/?page=haszowanie"
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

// wczytywanie słów z pliku do dynamicznej tablicy "dict"
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

// wczytywanie haseł do tablicy "hashes"
int loadHashes(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (!file) {
        fprintf(stderr, "Nie udało się otworzyć pliku haseł: %s\n", filename);
        exit(-1);
    }

    int count = 0, hash_count = 0, mail_count = 0;

    char temp[MD5_HASH_LENGTH + 1];

    while (fscanf(file, "%32s", temp) == 1 && count < MAX_HASHES_SIZE) {
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

void generateLowerVariant(const char *word, char lower[]) {
    int length = strlen(word);

    for (int i = 0; i < length; ++i) {
        lower[i] = tolower(word[i]);
    }

    lower[length] = '\0';
}

void generateUpperVariant(const char *word, char upper[]) {
    int length = strlen(word);

    for (int i = 0; i < length; ++i) {
        upper[i] = toupper(word[i]);
    }

    upper[length] = '\0';
}

void generateCapitalizedVariant(const char *word, char capitalized[]) {
    int length = strlen(word);

    for (int i = 0; i < length; ++i) {
        capitalized[i] = tolower(word[i]);
    }
    capitalized[0] = toupper(word[0]);

    capitalized[length] = '\0';
}

void checkHashMatch(const char *word, int hash_count) {
    char hash_result[MD5_HASH_LENGTH + 1];
    bytes2md5(word, hash_result);

    for (int j = 0; j < hash_count; j++) {
        if (strcmp(hash_result, hashes[j]) == 0 && !found[j]) {
            printf("Złamane hasło: %s odpowiada mailowi: %s\n", word, mails[j]);
            found[j] = 1; // Oznacz jako znaleziony
        }
    }
}


void checkNumberCombinationDS(const char *word, int hash_count) {
    char modified_word[MAX_WORD_LENGTH + 1];

    for (int num_1 = 0; num_1 <= 99; ++num_1) {
        sprintf(modified_word, "%d%s", num_1, word);
        checkHashMatch(modified_word, hash_count);
    }
}

void checkNumberCombinationSD(const char *word, int hash_count) {
    char modified_word[MAX_WORD_LENGTH + 1];

    for (int num_1 = 0; num_1 <= 99; ++num_1) {
        sprintf(modified_word, "%s%d", word, num_1);
        checkHashMatch(modified_word, hash_count);
    }
}

void checkNumberCombinationSDS(const char *word, int hash_count) {
    char modified_word[MAX_WORD_LENGTH + 1];

    for (int num_1 = 0; num_1 <= 99; ++num_1) {
        for (int num_2 = 0; num_2 <= 99; ++num_2) {
            sprintf(modified_word, "%d%s%d", num_2, word, num_1);
            checkHashMatch(modified_word, hash_count);
        }
    }
}

void checkNumberCombinations(const char *word, int hash_count) {
    checkNumberCombinationDS(word, hash_count);
    checkNumberCombinationSD(word, hash_count);
    checkNumberCombinationSDS(word, hash_count);
}

void crackPasswords(int dict_size, int hash_count) {
    printf("-> Start dekodowania haszów\n");

    for (int i = 0; i < dict_size; ++i) {
        char lower[MAX_WORD_LENGTH + 1], upper[MAX_WORD_LENGTH + 1], capitalized[MAX_WORD_LENGTH + 1];
        generateLowerVariant(dict[i], lower);
        generateUpperVariant(dict[i], upper);
        generateCapitalizedVariant(dict[i], capitalized);

        // Sprawdź dla wersji lower
        checkHashMatch(lower, hash_count);
        checkNumberCombinations(lower, hash_count);

        // Sprawdź dla wersji capitalized, jeśli różni się od lower
        if (strcmp(lower, capitalized) != 0) {
            checkHashMatch(capitalized, hash_count);
            checkNumberCombinations(capitalized, hash_count);
        }

        // Sprawdź dla wersji upper, jeśli różni się od lower i capitalized
        if (strcmp(upper, lower) != 0 && strcmp(upper, capitalized) != 0) {
            checkHashMatch(upper, hash_count);
            checkNumberCombinations(upper, hash_count);
        }
    }

    printf("-> Koniec dekodowania haszów\n");
}


void freeDictionary(int dict_size) {
    for (int i = 0; i < dict_size; ++i) {
        free(dict[i]);
    }
    free(dict);
}

// flagi: -lcrypto
int main() {
    int dict_size, hash_size;

    dict_size = loadDictionary("slowniki/slownik_5.txt");
    hash_size = loadHashes("hasla/hasla_5.txt");

    crackPasswords(dict_size, hash_size);

    freeDictionary(dict_size);

    return 0;
}