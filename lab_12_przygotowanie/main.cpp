#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>
#include <cstring>

// Struktura do reprezentowania taczki
struct Wheelbarrow {
    int number;          // Numer taczki
    std::string stone_type; // Typ kamienia w taczkach
    int weight;          // Waga kamienia w taczkach
    int quantity;        // Ilość kamienia
    int time_needed;     // Czas potrzebny na wykonanie zadania (obliczany jako weight * quantity)
};

// Struktura do przechowywania danych o danej minucie
struct MinuteData {
    int minute;                  // Minuta, do której odnoszą się dane
    std::vector<Wheelbarrow> barrows; // Lista taczek, które przyjechały w danej minucie
};

std::vector<MinuteData> data; // Globalna zmienna przechowująca dane o taczkach


// Funkcja wczytująca dane o taczkach z pliku
void read_wheelbarrow_data(std::string file_path) {
    std::ifstream file(file_path);  // Otwórz plik do odczytu
    if (!file) {                    // Jeśli nie uda się otworzyć pliku
        std::cerr << "Nie można otworzyć pliku" << std::endl;
        exit(0); // Zakończ program, jeśli nie udało się otworzyć pliku
    }

    std::string line; // Zmienna do przechowywania pojedynczej linii
    while (std::getline(file, line)) {  // Czytaj plik linia po linii
        std::istringstream iss(line);   // Strumień do analizy danej linii
        int minute;                     // Zmienna do przechowywania minuty
        iss >> minute;                  // Wczytaj minutę

        MinuteData md;                 // Utwórz obiekt MinuteData
        md.minute = minute;            // Ustaw minutę

        // Pętla do wczytywania wszystkich taczek w tej minucie
        while (!iss.eof()) {
            Wheelbarrow wb;           // Utwórz obiekt Wheelbarrow
            iss >> wb.number >> wb.stone_type >> wb.weight >> wb.quantity; // Wczytaj dane taczki
            if (iss.fail()) break;    // Jeśli napotkasz błąd w odczycie, zakończ wczytywanie
            wb.time_needed = wb.weight * wb.quantity; // Oblicz czas potrzebny do pracy
            md.barrows.push_back(wb); // Dodaj tą taczkę do listy
        }

        data.push_back(md); // Dodaj dane tej minuty do listy danych
    }
}

// Funkcja do wyświetlania wczytanych danych
void print_wheelbarrow_data() {
    std::cout << "Wczytane dane z pliku:\n";
    for (int i = 0; i < data.size(); ++i) {   // Pętla po danych minut
        MinuteData &md = data[i];  // Dostęp do danych tej minuty
        std::cout << "Minuta: " << md.minute << "\n";
        for (int j = 0; j < md.barrows.size(); ++j) {  // Pętla po taczkach tej minuty
            Wheelbarrow &wb = md.barrows[j];  // Dostęp do danej taczki
            std::cout << "  Taczka " << wb.number
                      << ", Typ kamienia: " << wb.stone_type
                      << ", Waga: " << wb.weight
                      << ", Ilość: " << wb.quantity
                      << ", Czas pracy: " << wb.time_needed << "\n";
        }
    }
    std::cout << "\n";
}

// Funkcja sprawdzająca, czy któryś robot ma aktywne zadanie
bool has_active_robot(const std::vector<int> &robot_states) {
    for (int i = 0; i < robot_states.size(); ++i) { // Przejdź po wszystkich robotach
        if (robot_states[i] > 0) { // Jeśli stan robota jest większy niż 0, robot jest aktywny
            return true;
        }
    }
    return false; // Jeśli żaden robot nie jest aktywny, zwróć false
}

// Funkcja planująca pracę robotów
void plan_robot_work(int num_robots) {
    std::vector<int> robot_states(num_robots, 0);  // Lista stanów robotów (0 = wolny)
    std::vector<std::string> robot_tasks(num_robots, "");  // Lista zadań robotów (początkowo pusta)
    std::queue<Wheelbarrow> queue;  // Kolejka taczek, które czekają na przypisanie robotom

    int current_minute = 0;  // Zmienna śledząca aktualną minutę

    std::cout << num_robots << " robots in the mine\n\n";
    std::cout << "FCFS:\n"; // Nagłówek "First Come First Served"

    // Pętla główna - trwa do momentu, gdy wszystkie taczki zostaną przydzielone
    while (current_minute < (data.size()) || !queue.empty() || has_active_robot(robot_states)) {
        // Dodaj nowe taczki do kolejki
        if (current_minute < (data.size()) && data[current_minute].minute == current_minute) {
            // Jeśli w tej minucie przyjeżdżają nowe taczki, dodaj je do kolejki
            for (int i = 0; i < data[current_minute].barrows.size(); ++i) {
                Wheelbarrow &wb = data[current_minute].barrows[i];
                queue.push(wb);
            }


            // Wyświetl przyjeżdżające taczki
            for (int i = 0; i < data[current_minute].barrows.size(); ++i) {
                Wheelbarrow &wb = data[current_minute].barrows[i];
                std::cout << "        wheelbarrow arrived <" << wb.number << " " << wb.stone_type
                          << " " << wb.weight << " " << wb.quantity << " [" << wb.time_needed << "]>\n";
            }
        }

        // Przypisz taczki do wolnych robotów
        for (int i = 0; i < num_robots; ++i) {
            if (robot_states[i] == 0 && !queue.empty()) {
                // Jeśli robot jest wolny i są dostępne taczki, przypisz robotowi taczkę
                Wheelbarrow wb = queue.front(); // Pobierz taczkę z przodu kolejki
                queue.pop(); // Usuń tą taczkę z kolejki
                robot_states[i] = wb.time_needed; // Ustaw czas pracy robota
                robot_tasks[i] = "[" + wb.stone_type + " " + std::to_string(wb.time_needed) + "]"; // Zapisz kamień i czas w nawiasach
            }
        }

        // Wyświetl stan robotów w tej minucie
        std::cout << "Moment " << current_minute << "\n";
        for (int i = 0; i < num_robots; ++i) {
            if (robot_tasks[i].empty()) {  // Jeśli robot nie ma zadania, wyświetl pustą przestrzeń
                std::cout << "[ ___ ]";
            } else {  // Jeśli robot ma zadanie, wyświetl nazwę zadania z nawiasami kwadratowymi
                std::cout << robot_tasks[i];
            }
        }

        std::cout << "\n";

        // Aktualizuj czas pracy robotów (zmniejsz o 1 minutę)
        for (int i = 0; i < num_robots; ++i) {
            if (robot_states[i] > 0) {
                robot_states[i]--;  // Zmniejsz czas pracy robota
                if (robot_states[i] == 0) {  // Jeśli robot zakończył zadanie
                    robot_tasks[i] = "";  // Usuń zadanie robota
                } else {
                    // Tworzymy nowy string z aktualnym czasem robota (zachowujemy nazwę kamienia)
                    robot_tasks[i] = robot_tasks[i].substr(0, robot_tasks[i].find(" ")) + " " + std::to_string(robot_states[i]) + "]";  // Aktualizuj czas w nawiasach
                }
            }
        }

        current_minute++;  // Przejdź do następnej minuty
    }
}

int main() {
    const std::string file_path = "dane.txt";  // Ścieżka do pliku z danymi
    std::vector<int> robot_count = {1, 2, 6};  // Lista liczby robotów w kopalniach
    std::vector<std::string> mine_names = {"la Carotte", "la Betterave", "le Radis"}; // Nazwy kopalń

    read_wheelbarrow_data(file_path);  // Wczytaj dane o taczkach

    // Wyświetlenie wczytanych danych
    print_wheelbarrow_data();

    // Planowanie pracy robotów dla każdej kopalni
    for (size_t i = 0; i < robot_count.size(); ++i) {
        std::cout << "\nPlan for mine " << mine_names[i] << ":\n\n";
        plan_robot_work(robot_count[i]);  // Zaplanuj pracę robotów w tej kopalni
    }

    return 0;  // Zakończ program
}
