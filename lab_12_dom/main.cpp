#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>
#include <cstring>
#include <unistd.h>

// struktura do reprezentowania taczek
struct wheelbarrow {
    int number; // numer taczek
    std::string stone_type; // typ kamienia w taczkach
    int weight; // waga kamienia w taczkach
    int quantity; // ilosc kamienia
    int time_needed; // czas potrzebny na wykonanie zadania (weight * quantity)
    int ctr = 0;
};

// struktura do przechowywania taczek do obsłużenia
struct currState {
    int minute;
    std::vector<wheelbarrow> barrows; // lista taczek które przyjechały w danej minucie
};

// globalna zmienna przechowująca wczytane dane
std::vector<currState> data;

enum alg {
    RR,
    FCFS,
    SRTF
};


// funkcja wczytująca dane o taczkach z pliku
void readDataFromFile(std::string file_path) {
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
            iss >> temp_wheelbarrow.number >> temp_wheelbarrow.stone_type >> temp_wheelbarrow.weight >> temp_wheelbarrow
                    .quantity;

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
void printData() {
    std::cout << std::endl << "=================================================" << std::endl;
    std::cout << "wczytane dane z pliku:\n";

    for (int i = 0; i < data.size(); ++i) {
        currState temp_state = data[i];
        std::cout << std::endl << "minuta: " << temp_state.minute << "\n";

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
bool hasWorkingRobot(const std::vector<int> &robot_states) {
    for (int i = 0; i < robot_states.size(); ++i) {
        // przejdz po wszystkich robotach
        if (robot_states[i] > 0) {
            // jeśli stan robota jest większy niż 0, robot jest aktywny
            return true;
        }
    }
    return false;
}

// funkcja pomocnicza zeby sprawdzić czy jakis robot pracuje
bool hasWorkingRobot(const std::vector<wheelbarrow> &robots) {
    for (const auto &robot: robots) {
        if (robot.time_needed > 0) {
            return true;
        }
    }
    return false;
}

// funkcja planująca pracę robotów za pomocą algorytmu RR (Round Robin)
void planWorkRR(int num_robots, int quantum) {
    std::vector<wheelbarrow> robots(num_robots); // lista robotów
    std::queue<wheelbarrow> queue; // kolejka taczek
    int current_minute = 0;

    std::cout << "robots in the mine: " << num_robots << std::endl << std::endl;

    // pętla główna - trwa do momentu, gdy wszystkie taczki zostaną przydzielone
    while (current_minute < data.size() || !queue.empty() || hasWorkingRobot(robots)) {
        // symulacja trwania rozładunku
        sleep(1);

        std::cout << std::endl << "moment " << current_minute << ":" << std::endl;

        // dodaj nowe taczki do kolejki
        if (current_minute < data.size()) {
            for (int i = 0; i < data[current_minute].barrows.size(); ++i) {
                wheelbarrow temp_wheelbarrow = data[current_minute].barrows[i];
                queue.push(temp_wheelbarrow);
                std::cout << "      wheelbarrow arrived <" << temp_wheelbarrow.number << " "
                          << temp_wheelbarrow.stone_type << " " << temp_wheelbarrow.weight << " "
                          << temp_wheelbarrow.quantity << " [" << temp_wheelbarrow.time_needed << "]>\n";
            }
        }


        // przypisz taczki do wolnych robotów
        for (int i = 0; i < num_robots; ++i) {
            if (robots[i].time_needed == 0 && !queue.empty()) {
                // jeśli robot jest wolny, przypisz mu taczkę
                wheelbarrow temp_wheelbarrow = queue.front();
                queue.pop();
                robots[i] = temp_wheelbarrow;
            }
        }

        // wyświetl stan robotów w tej minucie
        std::cout << "              ";
        for (int i = 0; i < num_robots; ++i) {
            std::cout << " ";
            if (robots[i].time_needed == 0) {
                std::cout << "[             ]";
            } else {
                std::cout << "[" << std::setw(12) << std::left << robots[i].stone_type
                        << robots[i].time_needed << "]";
            }
        }

        std::cout << std::endl;

        // obsługuje robotów, którzy przekroczyli kwant czasu
        for (int i = 0; i < num_robots; ++i) {
            if (robots[i].time_needed > quantum) {
                // jeśli czas pracy robota przekroczył kwant, zatrzymaj robota
                robots[i].time_needed -= quantum; // zmniejsz czas robota o kwant
                wheelbarrow temp_robot = robots[i]; // zapisz robota
                robots[i].ctr = quantum; // ustaw kwant na quantum

                // zwróć robota do kolejki, aby kontynuował pracę
                queue.push(temp_robot);
                robots[i] = wheelbarrow(); // resetuj robota
            } else if (robots[i].time_needed > 0) {
                robots[i].time_needed--; // zmniejsz czas pracy robota
            }
        }

        current_minute++;
    }
}

// funkcja planująca pracę robotów za pomocą algorytmu FCFS (First-Come, First-Served)
void planWorkFCFS(int num_robots) {
    std::vector<int> robot_states(num_robots, 0); // lista stanów robotów (0 = wolny)
    std::vector<wheelbarrow> robot_tasks(num_robots); // lista zadań robotów w postaci struktur wheelbarrow
    std::queue<wheelbarrow> queue; // kolejka taczek
    int current_minute = 0;

    std::cout << "robots in the mine: " << num_robots << std::endl << std::endl;

    // pętla główna - trwa do momentu, gdy wszystkie taczki zostaną przydzielone
    while (current_minute < (data.size()) || !queue.empty() || hasWorkingRobot(robot_states)) {
        // symulacja trwania rozładunku
        sleep(1);

        std::cout << std::endl << "moment " << current_minute << ":" << std::endl;

        // dodaj nowe taczki do kolejki
        if (current_minute < (data.size()) && data[current_minute].minute == current_minute) {
            // jeśli w tej minucie przyjeżdżają nowe taczki, dodaj je do kolejki
            for (int i = 0; i < data[current_minute].barrows.size(); ++i) {
                wheelbarrow &temp_wheelbarrow = data[current_minute].barrows[i];
                queue.push(temp_wheelbarrow);
            }

            // wyświetl przyjeżdżające taczki
            for (int i = 0; i < data[current_minute].barrows.size(); ++i) {
                wheelbarrow &temp_wheelbarrow = data[current_minute].barrows[i];
                std::cout << "      wheelbarrow arrived <" << temp_wheelbarrow.number << " " << temp_wheelbarrow.
                        stone_type
                        << " " << temp_wheelbarrow.weight << " " << temp_wheelbarrow.quantity << " [" <<
                        temp_wheelbarrow.time_needed << "]>\n";
            }
        }

        // przypisz taczki do wolnych robotów
        for (int i = 0; i < num_robots; ++i) {
            if (robot_states[i] == 0 && !queue.empty()) {
                // jeśli robot jest wolny i są dostępne taczki, przypisz robotowi taczkę
                wheelbarrow temp_wheelbarrow = queue.front(); // pobierz taczkę z przodu kolejki
                queue.pop(); // usuń tę taczkę z kolejki
                robot_states[i] = temp_wheelbarrow.time_needed; // ustaw czas pracy robota
                robot_tasks[i] = temp_wheelbarrow; // przypisz robotowi kopię taczki
            }
        }

        std::cout << "              ";
        // wyświetl stan robotów w tej minucie
        for (int i = 0; i < num_robots; ++i) {
            std::cout << " ";
            if (robot_states[i] == 0) {
                // jeśli robot nie ma zadania, wyświetl pustą przestrzeń
                std::cout << "[             ]";
            } else {
                // jeśli robot ma zadanie, wyświetl szczegóły taczki z pozostałym czasem
                std::cout << "[" << std::setw(12) << std::left << robot_tasks[i].stone_type
                        << robot_states[i] << "]";
            }
        }

        std::cout << std::endl;

        // aktualizuj czas pracy robotów (zmniejsz o 1 minutę)
        for (int i = 0; i < num_robots; ++i) {
            if (robot_states[i] > 0) {
                robot_states[i]--; // zmniejsz czas pracy robota
                if (robot_states[i] == 0) {
                    // zakończ zadanie robota
                    robot_tasks[i] = wheelbarrow{}; // wyczyść dane taczki
                }
            }
        }

        current_minute++;
    }
}


int main(int argc, char *argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0]
                << " <number_of_robots> <quantum> <file_name> <algorithm_choice>" << std::endl;
        std::cerr << "Algorithm choices: 1 = RR, 2 = FCFS, 3 = SRTF" << std::endl;
        return 1;
    }

    int robot_count = atoi(argv[1]);
    int quantum = atoi(argv[2]);
    std::string file_name = argv[3];
    int choice = atoi(argv[4]);

    alg algorithm;

    switch (choice) {
        case 1:
            algorithm = RR;
            break;
        case 2:
            algorithm = FCFS;
            break;
        case 3:
            algorithm = SRTF;
            break;
        default:
            std::cerr << "Invalid algorithm choice. Must be 1 (RR), 2 (FCFS), or 3 (SRTF)." << std::endl;
            return 1;
    }

    readDataFromFile(file_name);
    printData();

    std::cout << std::endl << "=================================================" << std::endl << std::endl;
    std::cout << "Running simulation with " << robot_count << " robots, quantum: " << quantum
            << ", algorithm: ";

    switch (algorithm) {
        case RR:
            std::cout << "RR" << std::endl;
            break;

        case FCFS:
            std::cout << "FCFS" << std::endl;
            break;

        case SRTF:
            std::cout << "SRTF" << std::endl;
            break;
    }

    std::cout << std::endl;

    std::cout << "=================================================" << std::endl;
    if (algorithm == RR) {
        planWorkRR(robot_count, quantum);
    } else if (algorithm == FCFS) {
        planWorkFCFS(robot_count);
    } else {
        std::cerr << "Selected algorithm not yet implemented in this example." << std::endl;
    }

    return 0;
}
