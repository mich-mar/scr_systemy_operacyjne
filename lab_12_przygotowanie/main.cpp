#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>
#include <cstring>

struct Wheelbarrow {
    int number;
    std::string stone_type;
    int weight;
    int quantity;
    int time_needed;
};

struct MinuteData {
    int minute;
    std::vector<Wheelbarrow> barrows;
};

void read_wheelbarrow_data(const std::string &file_path, std::vector<MinuteData> &data) {
    std::ifstream file(file_path);
    if (!file) {
        std::cerr << "Nie można otworzyć pliku" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int minute;
        iss >> minute;

        MinuteData md;
        md.minute = minute;

        while (!iss.eof()) {
            Wheelbarrow wb;
            iss >> wb.number >> wb.stone_type >> wb.weight >> wb.quantity;
            if (iss.fail()) break;
            wb.time_needed = wb.weight * wb.quantity;
            md.barrows.push_back(wb);
        }

        data.push_back(md);
    }
}

bool has_active_robot(const std::vector<int> &robot_states) {
    for (int state : robot_states) {
        if (state > 0) {
            return true;
        }
    }
    return false;
}

void plan_robot_work(const std::vector<MinuteData> &data, int num_robots) {
    std::vector<int> robot_states(num_robots, 0);
    std::vector<std::string> robot_tasks(num_robots, "");
    std::queue<Wheelbarrow> queue;

    int current_minute = 0;

    std::cout << num_robots << " robots in the mine\n\n";
    std::cout << "FCFS:\n";

    while (current_minute < static_cast<int>(data.size()) || !queue.empty() || has_active_robot(robot_states)) {
        // Dodaj nowe taczki do kolejki
        if (current_minute < static_cast<int>(data.size()) && data[current_minute].minute == current_minute) {
            for (const auto &wb : data[current_minute].barrows) {
                queue.push(wb);
            }

            for (const auto &wb : data[current_minute].barrows) {
                std::cout << "        wheelbarrow arrived <" << wb.number << " " << wb.stone_type
                          << " " << wb.weight << " " << wb.quantity << " [" << wb.time_needed << "]>\n";
            }
        }

        // Przypisz taczki do wolnych robotów
        for (int i = 0; i < num_robots; ++i) {
            if (robot_states[i] == 0 && !queue.empty()) {
                auto wb = queue.front();
                queue.pop();
                robot_states[i] = wb.time_needed;
                robot_tasks[i] = "[" + wb.stone_type + " " + std::to_string(wb.time_needed) + "]";
            }
        }

        // Wyświetl stan robotów w tej minucie
        std::cout << "Moment " << current_minute << "\n";
        for (int i = 0; i < num_robots; ++i) {
            std::cout << std::setw(16) << (robot_tasks[i].empty() ? "[             ]" : robot_tasks[i]);
        }
        std::cout << "\n";

        // Aktualizuj czas pracy robotów
        for (int i = 0; i < num_robots; ++i) {
            if (robot_states[i] > 0) {
                robot_states[i]--;
                if (robot_states[i] == 0) {
                    robot_tasks[i] = "";
                }
            }
        }

        current_minute++;
    }
}

int main() {
    const std::string file_path = "dane.txt";
    std::vector<MinuteData> data;

    read_wheelbarrow_data(file_path, data);

    std::vector<int> robot_configurations = {1, 2, 6};
    std::vector<std::string> mine_names = {"la Carotte", "la Betterave", "le Radis"};

    for (size_t i = 0; i < robot_configurations.size(); ++i) {
        std::cout << "\nPlan for mine " << mine_names[i] << ":\n\n";
        plan_robot_work(data, robot_configurations[i]);
    }

    return 0;
}
