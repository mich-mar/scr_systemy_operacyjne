#include <openssl/md5.h>
#include <openssl/evp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_HASHES_SIZE 1000
#define MAX_DICTIONARY_SIZE 1000
#define MD5_HASH_LENGTH 32
#define MAX_WORD_LENGTH 256

char dict[MAX_DICTIONARY_SIZE][MAX_WORD_LENGTH + 1];
char hashes[MAX_HASHES_SIZE][MD5_HASH_LENGTH + 1];
char mails[MAX_HASHES_SIZE][MAX_WORD_LENGTH + 1];

// funckja zaimporotowana ze strony "https://kcir.pwr.edu.pl/~krzewnicka/?page=haszowanie"
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

// wczytywanie słów z pliku do tablicy "dict"
int loadDictionary(const char *filename, int max_words) {
    FILE *file = fopen(filename, "r");

    if (!file) {
        fprintf(stderr, "Nie udało się otworzyć pliku słownika: %s\n", filename);
        exit(-1);
    }

    int count = 0;

    while (fscanf(file, "%255s", dict[count]) == 1 && count < max_words) {
        count++;
    }

    fclose(file);
    return count;
}


// wczytywanie haseł do tablicy "hashes"
int loadHashes(const char *filename, int max_hashes) {
    FILE *file = fopen(filename, "r");

    if (!file) {
        fprintf(stderr, "Nie udało się otworzyć pliku haseł: %s\n", filename);
        exit(-1);
    }

    int count = 0, hash_count = 0, mail_count = 0;

    char temp[MD5_HASH_LENGTH + 1];

    while (fscanf(file, "%32s", temp) == 1 && count < max_hashes) {

        if (count%4 == 1) {
            strcpy(hashes[hash_count], temp);
            hash_count++;
        }

        if (count%4 == 2) {
            strcpy(mails[mail_count], temp);
            mail_count++;
        }

        count++;
    }

    fclose(file);
    return hash_count;
}


// generowanie wersji pisowni słowa
void generateCaseVariants(const char *word, char lower[], char upper[], char capitalized[]) {
    int length = strlen(word);

    // małe litery i z pierwszą dużą
    for (int i = 0; i < length; ++i) {
        lower[i] = tolower(word[i]);
        capitalized[i] = tolower(word[i]);
    }
    capitalized[0] = toupper(word[0]);

    // wielkie litery
    for (int i = 0; i < length; ++i) {
        upper[i] = toupper(word[i]);
    }

    upper[length] = '\0';
    lower[length] = '\0';
    capitalized[length] = '\0';
}

// funkcja sprawdzająca, czy hash wygenerowany funkcją "bytes2md5" dla danego słowa ze słownika
// jest obecny w tablicy hashy
void checkHashMatch(const char *word, int hash_count) {
    char hash_result[MD5_HASH_LENGTH + 1];

    // oblicz hash MD5 dla słowa
    bytes2md5(word, hash_result);

    for (int j = 0; j < hash_count; j++) {
        if (strcmp(hash_result, hashes[j]) == 0) {
            printf("Złamane hasło: %s odpowiada mailowi: %s\n", word, mails[j]);
        }
    }
}

// Funkcja generująca kombinacje słowa z liczbami i sprawdzająca ich hash
void checkNumberCombinations(const char *word, int hash_count) {
    char modified_word[MAX_WORD_LENGTH + 1];

    for (int num_1 = 0; num_1 <= 99; ++num_1) {
        // liczba na początku
        sprintf(modified_word, "%d%s", num_1, word);
        checkHashMatch(modified_word, hash_count);

        // liczba na końcu
        sprintf(modified_word, "%s%d", word, num_1);
        checkHashMatch(modified_word, hash_count);

        // liczba na początku i końcu
        for (int num_2 = 0; num_2 <= 99; ++num_2) {
            sprintf(modified_word, "%d%s%d", num_2, word, num_1);
            checkHashMatch(modified_word, hash_count);
        }
    }
}

// funkcja do dekodowania haseł
void crackPasswords(int dict_size, int hash_count) {
    printf("-> Start dekodowania haszów\n");

    for (int i = 0; i < dict_size; ++i) {
        char lower[MAX_WORD_LENGTH + 1], upper[MAX_WORD_LENGTH + 1], capitalized[MAX_WORD_LENGTH + 1];
        generateCaseVariants(dict[i], lower, upper, capitalized);

        const char *variants[] = {lower, upper, capitalized};
        const int num_variants = 3;

        for (int v = 0; v < num_variants; ++v) {
            // wariant bez liczb
            checkHashMatch(variants[v], hash_count);

            // wariant z liczbami
            checkNumberCombinations(variants[v], hash_count);
        }
    }

    printf("-> Koniec dekodowania haszów\n");
}


// flagi: -lcrypto
int main() {
    int dict_size, hash_size;

    // wczytywanie słów ze słownika
    dict_size = loadDictionary("slowniki/slownik_2.txt",MAX_DICTIONARY_SIZE);

    // wczytywanie haseł z pliku
    hash_size = loadHashes("hasla/hasla_2.txt", MAX_HASHES_SIZE);

    // generowanie i sprawdzanie haszy
    crackPasswords(dict_size, hash_size);

    return 0;
}
