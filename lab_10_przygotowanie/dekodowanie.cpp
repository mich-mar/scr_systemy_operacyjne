#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <openssl/md5.h>
#include <cctype>

#define MAX_WORD_LENGTH 256
#define MAX_HASHES 1000
#define MD5_HASH_LENGTH 32

// Funkcja obliczająca hash MD5 dla podanego tekstu i zapisująca wynik w formacie szesnastkowym (hex).
void calculate_md5(const char *word, char *output) {
    // Tablica o rozmiarze 16 bajtów (stała MD5_DIGEST_LENGTH), w której zostanie zapisany wynik funkcji MD5.
    // Hash MD5 zawsze ma długość 16 bajtów (128 bitów).
    unsigned char digest[MD5_DIGEST_LENGTH];

    // Wywołanie funkcji MD5 z biblioteki OpenSSL.
    // Parametry:
    // - (unsigned char *)word: tekst wejściowy przekonwertowany na unsigned char*.
    // - strlen(word): długość tekstu wejściowego.
    // - digest: wskaźnik do tablicy, w której zostanie zapisany wynik (16 bajtów).
    MD5((unsigned char *)word, strlen(word), digest);

    // Pętla przekształcająca wynik hashowania (16 bajtów) na zapis szesnastkowy (hex) w ciągu znaków.
    // Każdy bajt (8 bitów) jest konwertowany na dwa znaki hex, dlatego długość ciągu hex wynosi 32 znaki.
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        // sprintf zapisuje wynik do wskaźnika 'output' w formacie "%02x":
        // - %02x: konwersja bajtu do dwóch cyfr szesnastkowych (z wiodącym zerem, jeśli potrzeba).
        // - &output[i * 2]: zapisujemy kolejne dwa znaki w odpowiedniej pozycji w ciągu wyjściowym.
        sprintf(&output[i * 2], "%02x", digest[i]);
    }
}


// Wczytywanie słów z pliku do tablicy
int load_dictionary(const char *filename, char dictionary[][MAX_WORD_LENGTH], int max_words) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Nie udało się otworzyć pliku słownika");
        return -1;
    }

    int count = 0;
    while (fscanf(file, "%255s", dictionary[count]) == 1 && count < max_words) {
        count++;
    }

    fclose(file);
    return count;
}

// Wczytywanie zahaszowanych haseł do tablicy
int load_hashes(const char *filename, char hashes[][MD5_HASH_LENGTH + 1], int max_hashes) {
    FILE *file = fopen(filename, "r");

    if (!file) {
        perror("Nie udało się otworzyć pliku haseł");
        return -1;
    }

    int count = 0;
    while (fscanf(file, "%32s", hashes[count]) == 1 && count < max_hashes) {
        count++;
    }

    fclose(file);
    return count;
}

// Generowanie wersji pisowni słowa
void generate_case_variants(const char *word, char lower[], char upper[], char capitalized[]) {
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
void crack_passwords(const char dictionary[][MAX_WORD_LENGTH], int dict_size,
                     const char hashes[][MD5_HASH_LENGTH + 1], int hash_count) {
    char hash_result[MD5_HASH_LENGTH + 1];
    char modified_word[MAX_WORD_LENGTH];

    printf("Rozpoczynam łamanie haseł...\n");

    for (int i = 0; i < dict_size; i++) {
        char lower[MAX_WORD_LENGTH], upper[MAX_WORD_LENGTH], capitalized[MAX_WORD_LENGTH];
        generate_case_variants(dictionary[i], lower, upper, capitalized);

        const char *variants[] = {lower, upper, capitalized};
        const int num_variants = 3;

        for (int v = 0; v < num_variants; ++v) {
            for (int num = 0; num <= 99; ++num) {
                // Generowanie wariantów słów z liczbami
                sprintf(modified_word, "%d%s", num, variants[v]);
                calculate_md5(modified_word, hash_result);
                for (int j = 0; j < hash_count; j++) {
                    if (strcmp(hash_result, hashes[j]) == 0) {
                        printf("Złamane hasło: %s odpowiada hashowi: %s\n", modified_word, hashes[j]);
                    }
                }

                sprintf(modified_word, "%s%d", variants[v], num);
                calculate_md5(modified_word, hash_result);
                for (int j = 0; j < hash_count; j++) {
                    if (strcmp(hash_result, hashes[j]) == 0) {
                        printf("Złamane hasło: %s odpowiada hashowi: %s\n", modified_word, hashes[j]);
                    }
                }

                sprintf(modified_word, "%d%s%d", num, variants[v], num);
                calculate_md5(modified_word, hash_result);
                for (int j = 0; j < hash_count; j++) {
                    if (strcmp(hash_result, hashes[j]) == 0) {
                        printf("Złamane hasło: %s odpowiada hashowi: %s\n", modified_word, hashes[j]);
                    }
                }
            }
        }
    }

    printf("Proces łamania haseł zakończony.\n");
}

int main() {
    // Tablice na słownik i zahaszowane hasła
    char dictionary[10000][MAX_WORD_LENGTH]; // Maksymalna liczba słów w słowniku
    char hashes[MAX_HASHES][MD5_HASH_LENGTH + 1]; // Maksymalna liczba haseł

    // Wczytanie słownika
    int dict_size = load_dictionary("dictionaries/test_1.txt", dictionary, 10000);
    if (dict_size < 0) return 1;

    // Wczytanie haseł
    int hash_count = load_hashes("passwords/test_1.txt", hashes, MAX_HASHES);
    if (hash_count < 0) return 1;

    // Sprawdzanie haseł
    crack_passwords(dictionary, dict_size, hashes, hash_count);

    return 0;
}
