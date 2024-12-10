#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <openssl/md5.h>
#include <cctype>

#define MAX_HASHES_SIZE 1000
#define MAX_DICTIONARY_SIZE 1000
#define MD5_HASH_LENGTH 32
#define MAX_WORD_LENGTH 256

char dict[MAX_DICTIONARY_SIZE][MAX_WORD_LENGTH + 1];
char hashes[MAX_HASHES_SIZE][MD5_HASH_LENGTH + 1];

// Funkcja obliczająca hash MD5 dla podanego tekstu i zapisująca wynik w formacie szesnastkowym (hex).
void calculateMD5(const char *inpout_word, char *out_md5) {

    // Tablica o rozmiarze 16 bajtów (stała MD5_DIGEST_LENGTH), w której zostanie zapisany wynik funkcji MD5.
    // Hash MD5 zawsze ma długość 16 bajtów (128 bitów).
    unsigned char MD5_output[MD5_DIGEST_LENGTH];

    // Wywołanie funkcji MD5 z biblioteki OpenSSL.
    // Parametry:
    // - (unsigned char *)word: tekst wejściowy przekonwertowany na unsigned char*.
    // - strlen(word): długość tekstu wejściowego.
    // - digest: wskaźnik do tablicy, w której zostanie zapisany wynik (16 bajtów).
    MD5((unsigned char *) inpout_word, strlen(inpout_word), MD5_output);

    // Pętla przekształcająca wynik hashowania (16 bajtów) na zapis szesnastkowy (hex) w ciągu znaków.
    // Każdy bajt (8 bitów) jest konwertowany na dwa znaki hex, dlatego długość ciągu hex wynosi 32 znaki.
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        // sprintf zapisuje wynik do wskaźnika 'output' w formacie "%02x":
        // - %02x: konwersja bajtu do dwóch cyfr szesnastkowych (z wiodącym zerem, jeśli potrzeba).
        // - &output[i * 2]: zapisujemy kolejne dwa znaki w odpowiedniej pozycji w ciągu wyjściowym.
        sprintf(&out_md5[i * 2], "%02x", MD5_output[i]);
    }
}


// Wczytywanie słów z pliku do tablicy
int loadDictionary(const char *filename, char dictionary[][MAX_WORD_LENGTH + 1], int max_words) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Nie udało się otworzyć pliku słownika: %s\n", filename);
        exit(-1);
    }

    int count = 0;
    while (fscanf(file, "%255s", dictionary[count]) == 1 && count < max_words) {
        count++;
    }

    fclose(file);
    return count;
}


// Wczytywanie zahaszowanych haseł do tablicy
int loadHashes(const char *filename, char hashes[][MD5_HASH_LENGTH + 1], int max_hashes) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Nie udało się otworzyć pliku haseł: %s\n", filename);
        exit(-1);
    }

    int count = 0;
    while (fscanf(file, "%32s", hashes[count]) == 1 && count < max_hashes) {
        count++;
    }

    fclose(file);
    return count;
}



// Generowanie wersji pisowni słowa
void generateCaseVariants(const char *word, char lower[], char upper[], char capitalized[]) {
    int length = strlen(word);

    // Małe litery
    for (int i = 0; i < length; ++i) lower[i] = tolower(word[i]);
    lower[length] = '\0';

    // Wielkie litery
    for (int i = 0; i < length; ++i) upper[i] = toupper(word[i]);
    upper[length] = '\0';

    // Pierwsza wielka litera
    capitalized[0] = toupper(word[0]);
    for (int i = 1; i < length; ++i) capitalized[i] = tolower(word[i]);
    capitalized[length] = '\0';
}

// Sprawdzanie haseł z rozszerzeniami
// Funkcja sprawdzająca, czy hash wygenerowany dla danego słowa jest obecny w tablicy hashy
void checkHashMatch(const char *word, const char hashes[][MD5_HASH_LENGTH + 1], int hash_count) {
    char hash_result[MD5_HASH_LENGTH + 1];
    calculateMD5(word, hash_result); // Oblicz hash MD5 dla słowa

    for (int j = 0; j < hash_count; ++j) {
        if (strcmp(hash_result, hashes[j]) == 0) {
            printf("Złamane hasło: %s odpowiada hashowi: %s\n", word, hashes[j]);
        }
    }
}

// Funkcja generująca kombinacje słowa z liczbami i sprawdzająca ich hash
void checkNumberCombinations(const char *word, const char hashes[][MD5_HASH_LENGTH + 1], int hash_count) {
    char modified_word[MAX_WORD_LENGTH + 1];

    for (int num = 0; num <= 99; ++num) {
        // Liczba na początku
        sprintf(modified_word, "%d%s", num, word);
        checkHashMatch(modified_word, hashes, hash_count);

        // Liczba na końcu
        sprintf(modified_word, "%s%d", word, num);
        checkHashMatch(modified_word, hashes, hash_count);

        // Liczba na początku i końcu
        sprintf(modified_word, "%d%s%d", num, word, num);
        checkHashMatch(modified_word, hashes, hash_count);
    }
}

// Główna funkcja łamania haseł
void crackPasswords(const char dictionary[][MAX_WORD_LENGTH + 1], int dict_size,
                     const char hashes[][MD5_HASH_LENGTH + 1], int hash_count) {
    printf("-> Start dekodowania haszów\n");

    for (int i = 0; i < dict_size; ++i) {
        char lower[MAX_WORD_LENGTH + 1], upper[MAX_WORD_LENGTH + 1], capitalized[MAX_WORD_LENGTH + 1];
        generateCaseVariants(dictionary[i], lower, upper, capitalized);

        const char *variants[] = {lower, upper, capitalized};
        const int num_variants = 3;

        for (int v = 0; v < num_variants; ++v) {
            // Sprawdź sam wariant bez liczb
            checkHashMatch(variants[v], hashes, hash_count);

            // Sprawdź wariant z liczbami
            checkNumberCombinations(variants[v], hashes, hash_count);
        }
    }

    printf("-> Koniec dekodowania haszów\n");
}


// flagi: -lcrypto
int main() {
    int dict_size, hash_size;

    // Wczytanie słownika
    dict_size = loadDictionary("dictionaries/test_1.txt", dict,MAX_DICTIONARY_SIZE);

    // Wczytanie haseł
    hash_size = loadHashes("passwords/test_1.txt", hashes, MAX_HASHES_SIZE);

    // Sprawdzanie haseł
    crackPasswords(dict, dict_size, hashes, hash_size);

    return 0;
}
