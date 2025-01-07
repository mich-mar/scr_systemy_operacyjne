#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>
#include <cstring>

// struktura do reprezentowania taczek
struct wheelbarrow {
    int number; // numer taczek
    std::string stone_type; // typ kamienia w taczkach
    int weight; // waga kamienia w taczkach
    int quantity; // ilosc kamienia
    int time_needed; // czas potrzebny na wykonanie zadania (weight * quantity)
};

// struktura do przechowywania taczek do obsłużenia
struct currState {
    int minute;
    std::vector<wheelbarrow> barrows; // lista taczek które przyjechały w danej minucie
};

// globalna zmienna przechowująca dane w poszczególnych minutach
std::vector<currState> data; 


// funkcja wczytująca dane o taczkach z pliku
void readWheelbarrowData(std::string file_path) {
    std::ifstream file(file_path);
    std::string line;

    if (!file) {
        std::cerr << "nie mozna otworzyc pliku" << std::endl;
        exit(0);
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);

        int minute;
        iss >> minute;

        currState temp_state;
        temp_state.minute = minute;

        // petla do wczytywania wszystkich taczek w tej minucie
        while (!iss.eof()) {
            wheelbarrow temp_wheelbarrow; // utwórz obiekt wheelbarrow

            // wczytaj dane taczki
            iss >> temp_wheelbarrow.number >> temp_wheelbarrow.stone_type >> temp_wheelbarrow.weight >> temp_wheelbarrow.quantity;

            // jeśli napotkasz błąd w odczycie, zakończ wczytywanie
            if (iss.fail())
                break;

            // oblicz czas potrzebny do pracy
            temp_wheelbarrow.time_needed = temp_wheelbarrow.weight * temp_wheelbarrow.quantity;

            // dodaj tą taczkę do listy
            temp_state.barrows.push_back(temp_wheelbarrow);
        }

        // dodaj dane tej minuty do listy danych
        data.push_back(temp_state);
    }
}

// funkcja do wyswietlania wczytanych danych
void printWheelbarrowData() {
    std::cout << "wczytane dane z pliku:\n";

    for (int i = 0; i < data.size(); ++i) {
        currState temp_state = data[i];
        std::cout << "minuta: " << temp_state.minute << "\n";

        for (int j = 0; j < temp_state.barrows.size(); ++j) {
            wheelbarrow temp_wheelbarrow = temp_state.barrows[j];

            std::cout << "  taczka " << temp_wheelbarrow.number
                    << ", typ kamienia: " << temp_wheelbarrow.stone_type
                    << ", waga: " << temp_wheelbarrow.weight
                    << ", ilosc: " << temp_wheelbarrow.quantity
                    << ", czas pracy: " << temp_wheelbarrow.time_needed << std::endl;
        }
    }
    std::cout << std::endl;
}

// funkcja sprawdzajaca czy ktorys robot ma aktywne zadanie
bool hasActiveRobot(const std::vector<int> &robot_states) {
    for (int i = 0; i < robot_states.size(); ++i) {
        // przejdz po wszystkich robotach
        if (robot_states[i] > 0) {
            // jeśli stan robota jest większy niż 0, robot jest aktywny
            return true;
        }
    }
    return false;
}

// funkcja planujaca prace robotow
void planRobotWork(int num_robots) {
    std::vector<int> robot_states(num_robots, 0); // lista stanów robotów (0 = wolny)
    std::vector<std::string> robot_tasks(num_robots, ""); // lista zadan robotów (początkowo "")
    std::queue<wheelbarrow> queue; // kolejka taczek
    int current_minute = 0;

    std::cout << "robots in the mine: "<< num_robots << std::endl << std::endl;

    // petla glowna - trwa do momentu, gdy wszystkie taczki zostaną przydzielone
    while (current_minute < (data.size()) || !queue.empty() || hasActiveRobot(robot_states)) {
        std::cout << "moment " << current_minute << ":" << std::endl;

        // dodaj nowe taczki do kolejki
        if (current_minute < (data.size()) && data[current_minute].minute == current_minute) {
            // jeśli w tej minucie przyjeżdżają nowe taczki, dodaj je do kolejki
            for (int i = 0; i < data[current_minute].barrows.size(); ++i) {
                wheelbarrow &wb = data[current_minute].barrows[i];
                queue.push(wb);
            }

            // wyswietl przyjezdzajace taczki
            for (int i = 0; i < data[current_minute].barrows.size(); ++i) {
                wheelbarrow &wb = data[current_minute].barrows[i];
                std::cout << "        wheelbarrow arrived <" << wb.number << " " << wb.stone_type
                        << " " << wb.weight << " " << wb.quantity << " [" << wb.time_needed << "]>\n";
            }
        }

        // przypisz taczki do wolnych robotów
        for (int i = 0; i < num_robots; ++i) {
            if (robot_states[i] == 0 && !queue.empty()) {
                // jeśli robot jest wolny i są dostępne taczki, przypisz robotowi taczkę
                wheelbarrow wb = queue.front(); // pobierz taczkę z przodu kolejki
                queue.pop(); // usuń ta taczkę z kolejki
                robot_states[i] = wb.time_needed; // ustaw czas pracy robota
                robot_tasks[i] = "[" + wb.stone_type + " " + std::to_string(wb.time_needed) + "]"; // zapisz stan pracy
            }
        }

        // wyswietl stan robotów w tej minucie
        for (int i = 0; i < num_robots; ++i) {
            if (robot_tasks[i].empty()) {
                // jeśli robot nie ma zadania, wyświetl pustą przestrzeń
                std::cout << "[ ___ ]";
            } else {
                // jeśli robot ma zadanie, wyświetl nazwę zadania z nawiasami kwadratowymi
                std::cout << robot_tasks[i];
            }
        }

        std::cout << std::endl;

        // aktualizuj czas pracy robotów (zmniejsz o 1 minutę)
        for (int i = 0; i < num_robots; ++i) {
            if (robot_states[i] > 0) {
                robot_states[i]--; // zmniejsz czas pracy robota
                if (robot_states[i] == 0) {
                    // jeśli robot zakończył zadanie
                    robot_tasks[i] = ""; // usuń zadanie robota
                } else {
                    // tworzymy nowy string z pozostałym potrzebnym czasem pracy robota
                    robot_tasks[i] = robot_tasks[i].substr(0, robot_tasks[i].find(" ")) + " " +
                                     std::to_string(robot_states[i]) + "]";
                }
            }
        }

        current_minute++;
    }
}

int main() {
    const std::string file_path = "dane.txt";
    std::vector<int> robot_count = {1, 2, 6};
    std::vector<std::string> mine_names = {"la carotte", "la betterave", "le radis"};

    // wczytaj dane o taczkach
    readWheelbarrowData(file_path);

    // wyswietlenie wczytanych danych
    printWheelbarrowData();

    // planowanie pracy robotów dla każdej kopalni
    for (size_t i = 0; i < robot_count.size(); ++i) {
        std::cout << "\nplan for mine: " << mine_names[i] << "\n\n";

        // zaplanuj prace robotów w tej kopalni
        planRobotWork(robot_count[i]);
    }

    return 0;
}
