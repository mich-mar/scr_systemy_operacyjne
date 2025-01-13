#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <algorithm>

// struktura do reprezentowania taczek
struct wheelbarrow {
    int number; // numer taczki
    std::string stone_type; // typ kamienia w taczce
    int weight; // waga kamienia w taczkach
    int quantity; // ilosc kamienia
    int time_needed; // czas potrzebny na wykonanie zadania (weight * quantity)
    int ctr = 0; // licznik przydatny do algorytmu RR
};

// struktura do przechowywania taczek do obsłużenia
struct currState {
    int minute;
    std::vector<wheelbarrow> barrows; // lista taczek które przyjechały w danej minucie
};

// globalna zmienna przechowująca wczytane dane
std::vector<currState> data;

// liczba robotów w kopalni
int num_robots, quantum;

// enum do wyboru argumentów
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
            wheelbarrow temp_wheelbarrow;

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


// funkcja pomocnicza zeby sprawdzić czy jakis robot pracuje
bool hasWorkingRobot(const std::vector<wheelbarrow> robots) {
    for (int i = 0; i < robots.size(); ++i) {
        if (robots[i].time_needed > 0) {
            return true;
        }
    }
    return false;
}


// funkcja planująca pracę robotów za pomocą algorytmu RR (Round Robin)
void planWorkRR() {
    std::vector<wheelbarrow> robots(num_robots); // lista robotów
    std::queue<wheelbarrow> queue; // kolejka taczek
    int current_minute = 0;

    std::cout << "robots in the mine: " << num_robots << std::endl;
    std::cout << "used alg - RR with quantum " << quantum << ": " << std::endl << std::endl;

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
void planWorkFCFS() {
    std::vector<wheelbarrow> robots(num_robots); // lista robotów (zainicjowana pustymi strukturami wheelbarrow)
    std::queue<wheelbarrow> queue; // kolejka taczek
    int current_minute = 0;

    std::cout << "robots in the mine: " << num_robots << std::endl;
    std::cout << "used alg - FCFS:" << std::endl << std::endl;

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

        // zmniejszaj czas pracy robotów
        for (int i = 0; i < num_robots; ++i) {
            if (robots[i].time_needed > 0) {
                robots[i].time_needed--; // zmniejsz czas pracy robota
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

    num_robots = atoi(argv[1]);
    quantum = atoi(argv[2]);
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

    // std::cout << std::endl << "=================================================" << std::endl << std::endl;
    // std::cout << "Running simulation with " << num_robots << " robots, quantum: " << quantum
    //         << ", algorithm: ";

    std::cout << std::endl;

    std::cout << "=================================================" << std::endl;
    if (algorithm == RR) {
        planWorkRR();
    } else if (algorithm == FCFS) {
        planWorkFCFS();
    } else if (algorithm == SRTF) {
        std::cerr << "Brak zaimplementowanego algorytmu SRTF :((" << std::endl;
    }

    return 0;
}
