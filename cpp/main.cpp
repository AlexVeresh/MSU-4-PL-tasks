#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <set>
#include <sys/time.h>
#include <sys/resource.h>
#include <ncurses.h>
#include <locale>
#include <chrono>
#include <iomanip>
#include <ctime>
#include "graph.hpp"
#include "graph_search.hpp"
#include "logger.hpp"
#include "path.hpp"

using namespace std;
using msu_tasks_cpp::Graph;
using msu_tasks_cpp::GraphSearch;
using msu_tasks_cpp::Logger;
using msu_tasks_cpp::Path;
using CityId = msu_tasks_cpp::Graph::CityId;
using City = msu_tasks_cpp::Graph::City;
using TransportId = msu_tasks_cpp::Graph::TransportId;
using Transport = msu_tasks_cpp::Graph::Transport;

#define ctrl(x) (x & 0x1F)

const string EXIT_APP = "EXIT_APP";

string dash_divider()
{
    string result = "";
    for (int i = 0; i < 30; i++)
    {
        result += "— ";
    }
    return result;
}

string high_divider()
{
    string result = "";
    for (int i = 0; i < 120; i++)
    {
        result += "•";
    }
    return result;
}

void clear_n_lines(int start, int end)
{
    for (int i = start; i <= end; i++)
    {
        move(i, 0);
        clrtoeol();
    }
}

void log_paths_and_data(
    const string mode_title,
    const string &result,
    float estimated_time,
    const string city_from_title,
    const string city_to_title)
{
    string previous_input = "";
    string current_line = "";
    fstream file;
    file.open("../logs.txt");
    while (getline(file, current_line))
    {
        previous_input += current_line + '\n';
    }
    file.close();

    struct rusage usage;
    ofstream myfile("../logs.txt");
    string line;
    if (myfile.is_open())
    {
        if (previous_input.length() != 0)
        {
            myfile << previous_input;
            myfile << "\n";
            myfile << high_divider();
            myfile << "\n";
            myfile << high_divider();
            myfile << "\n\n";
        }

        myfile << "Язык: C++\n";
        const auto t = std::time(nullptr);
        const auto tm = *std::localtime(&t);
        myfile << "Дата: " << std::put_time(&tm, "%Y-%m-%d") << std::endl;
        myfile << mode_title;

        if (city_from_title.length() != 0)
        {
            myfile << "\n";
            myfile << city_from_title;
            myfile << "\n";
            myfile << city_to_title;
        }
        myfile << "\n";
        myfile << dash_divider();
        myfile << "\n";
        myfile << result;
        myfile << "\n";
        myfile << dash_divider();
        myfile << "\n";
        myfile << "Время выполнения алгоритма поиска: " << estimated_time << " секунд";
        if (0 == getrusage(RUSAGE_SELF, &usage))
        {
            myfile << '\n'
                   << "Памяти использовано: " << usage.ru_maxrss << endl;
        }
        else
        {
            myfile << '\n'
                   << "Памяти использовано: " << '0' << endl;
        }
        myfile.close();
    }
}

float estimate_time(const std::chrono::steady_clock::duration &start_time_duration)
{
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time = end_time.time_since_epoch() - start_time_duration;
    return std::chrono::duration<float>(time).count();
}

string format_string(const string &input)
{

    string result = "";
    for (const auto &c : input)
    {
        if (c != '"')
        {
            result += c;
        }
    }
    return result;
}

void wait_for_enter(int start)
{

    curs_set(0);
    attron(COLOR_PAIR(3));
    mvprintw(start, 0, "Нажмите ENTER чтобы продолжить");
    attroff(COLOR_PAIR(3));
    move(start + 1, 0);
    char c;
    while ((c = getch()) != 10)
    {
    }
}

int validate_number_input(const string &input)
{
    for (int i = 0; i < input.length(); i++)
    {
        if (isdigit(input[i]) == false)
        {
            return -1;
        }
    }
    return stoi(input);
}

int validate_city_input(const unordered_map<CityId, City> &cities, const string &input)
{
    for (const auto &[key, value] : cities)
    {
        if (value.title == input)
        {
            return value.id;
        }
    }
    return -1;
}

int validate_transport_input(const unordered_map<TransportId, Transport> &transports, const string &input)
{
    for (const auto &[key, value] : transports)
    {
        if (value.title == input)
        {
            return value.id;
        }
    }
    return -1;
}

void handle_file_input(Graph &graph, const char *filename)
{
    string result;

    ifstream input(filename);

    while (getline(input, result))
    {
        string item;
        string cityPart;
        stringstream ss(result);
        int iter = 0;

        Graph::CityId from_city_id;
        Graph::CityId to_city_id;
        Graph::TransportId transport_id;
        unsigned int time;
        unsigned int fare;

        while (getline(ss, item, ' '))
        {
            if (item != "\n")
            {

                if (item[0] == '"' && item.back() != '"')
                {
                    cityPart = item;
                    continue;
                }
                if (cityPart != "")
                {
                    item = cityPart + ' ' + item;
                    cityPart = "";
                }
                // const auto arr = item.c_str();
                // item.erase(remove(arr.begin(), arr.end(), '"'), arr.end());
                item = format_string(item);

                switch (iter)
                {
                case 0:
                    from_city_id = graph.add_city(item);
                    break;
                case 1:
                    to_city_id = graph.add_city(item);
                    break;
                case 2:
                    transport_id = graph.add_transport(item);
                    break;
                case 3:
                    time = stoi(item);
                    break;
                case 4:
                    fare = stoi(item);
                    auto new_cruise = Graph::Cruise(from_city_id, to_city_id, transport_id, time, fare);
                    graph.add_cruise(new_cruise);
                    break;
                }
                iter++;
            }
        }
    }

    input.close();
}

int handle_mode_input()
{
    curs_set(0);
    Graph::TransportId chosen_mode = 0;
    const unsigned int modes_count = 6;
    const string modes[modes_count] = {
        " 1. Среди кратчайших по времени путей между двумя городами найти путь минимальной стоимости",
        " 2. Среди путей между двумя городами найти путь минимальной стоимости",
        " 3. Среди путей между двумя городами найти путь минимальный по числу посещенных городов",
        " 4. Найти множество городов (и минимальных по стоимости путей к ним), достижимых из города отправления\n     не более чем за обозначенную сумму денег",
        " 5. Найти множество городов (и минимальных по времени путей к ним), достижимых из города отправления\n     не более чем за обозначенное время",
        "Завершить программу"};
    int user_choice;
    bool is_proccessing = true;

    // clear();
    // refresh();

    mvprintw(0, 0, "Добро пожаловать! Выберите режим работы");

    while (is_proccessing)
    {

        for (int i = 0; i < modes_count; i++)
        {
            if (chosen_mode == i)
            {
                attron(A_REVERSE);
            }
            if (i == 4)
            {
                mvprintw(i + 3, 0, modes[i].c_str());
            }
            else if (i == 5)
            {
                attron(COLOR_PAIR(5));
                mvprintw(i + 5, 0, modes[i].c_str());
                attroff(COLOR_PAIR(5));
            }
            else
            {
                mvprintw(i + 2, 0, modes[i].c_str());
            }
            attroff(A_REVERSE);
        }
        user_choice = getch();

        switch (user_choice)
        {
        case KEY_UP:
            if (chosen_mode == 0)
            {
                chosen_mode = modes_count - 1;
            }
            else
            {
                chosen_mode--;
            }
            break;
        case KEY_DOWN:
            if (chosen_mode == modes_count - 1)
            {
                chosen_mode = 0;
            }
            else
            {
                chosen_mode++;
            }
            break;
        default:
            break;
        }

        // if (user_choice == 27)
        // {
        //     is_proccessing = false;
        //     return -1;
        // }
        if (user_choice == 10)
        {
            is_proccessing = false;
            if (chosen_mode == 5)
            {
                return -1;
            }
            else
            {
                return chosen_mode;
            }
        }
    }
    return -1;
}

int handle_finish_app()
{
    curs_set(0);
    const unsigned int modes_count = 2;
    int chosen_mode = 0;
    const string modes[modes_count] = {
        " 1. Завершить программу",
        " 2. Вернутся на страницу выбора режима работы"};
    int user_choice;
    bool is_proccessing = true;

    mvprintw(0, 0, "Выберите режим работы");

    while (is_proccessing)
    {

        for (int i = 0; i < modes_count; i++)
        {
            if (chosen_mode == i)
            {
                attron(A_REVERSE);
            }
            mvprintw(i + 2, 0, modes[i].c_str());
            attroff(A_REVERSE);
        }
        user_choice = getch();

        switch (user_choice)
        {
        case KEY_UP:
            if (chosen_mode == 0)
            {
                chosen_mode = modes_count - 1;
            }
            else
            {
                chosen_mode--;
            }
            break;
        case KEY_DOWN:
            if (chosen_mode == modes_count - 1)
            {
                chosen_mode = 0;
            }
            else
            {
                chosen_mode++;
            }
            break;
        default:
            break;
        }

        if (user_choice == 10)
        {
            return chosen_mode;
        }
    }

    return chosen_mode;
}

string handle_int_input()
{
    echo();
    char input[200];
    attron(COLOR_PAIR(4));
    getnstr(input, 200);
    attroff(COLOR_PAIR(4));
    std::string str(input);
    noecho();
    return str;
}

string handle_string_input(int row)
{
    int c;
    string str = "";
    string new_typed = "";
    int i = 0;
    bool backspace_tap_after_input = true;
    while ((c = getch()) != 10)
    {
        if (c != -1 && (c < 258 || c > 261))
        {
            if (c == 127 || c == KEY_BACKSPACE)
            {
                if (backspace_tap_after_input)
                {
                    i -= ((new_typed.length() / 2));
                    backspace_tap_after_input = false;
                    new_typed.clear();
                }

                if (i > 0)
                {
                    if (str.back() < '0')
                    {
                        str.pop_back();
                        str.pop_back();
                    }
                    else
                    {
                        str.pop_back();
                    }
                    i -= 1;
                    move(row, i);
                }
                else
                {
                    i = 0;
                }
                clrtoeol();
            }
            else if (c == ctrl(27))
            {
                return EXIT_APP;
            }
            else if (char(c) < '0')
            {
                backspace_tap_after_input = true;
                attron(COLOR_PAIR(4));
                printw("%c", c);
                attroff(COLOR_PAIR(4));
                str.push_back(char(c));
                new_typed.push_back(char(c));
                i++;
            }
            else
            {
                backspace_tap_after_input = true;
                attron(COLOR_PAIR(4));
                printw("%c", c);
                attroff(COLOR_PAIR(4));
                str.push_back(char(c));
                new_typed.push_back(char(c));
                new_typed.push_back(char(c));
                i += 2;
            }
        }
    }
    return str;
    // echo();
    // char input[200];
    // attron(COLOR_PAIR(4));
    // getnstr(input, 200);
    // attroff(COLOR_PAIR(4));
    // std::string str(input);
    // noecho();
    // return input;
}

pair<CityId, CityId> handle_pair_of_cities_input(Graph &graph)
{
    curs_set(1);
    printw("Введите город отправления");

    move(1, 0);

    refresh();

    string city_from;
    int city_from_id = -1;
    city_from = handle_string_input(1);
    if (city_from == EXIT_APP)
    {
        return make_pair(-1, -1);
    }

    while (city_from_id == -1)
    {
        city_from_id = validate_city_input(graph.get_cities(), city_from);
        if (city_from_id == -1)
        {
            clear_n_lines(3, 4);
            mvprintw(3, 0, "Введите корректный город отправления");
            move(4, 0);
            city_from = handle_string_input(4);
            if (city_from == EXIT_APP)
            {
                return make_pair(-1, -1);
            }
        }
    }

    clear_n_lines(0, 4);

    attron(COLOR_PAIR(3));
    mvprintw(0, 0, "######//~~~~~~//######");
    attroff(COLOR_PAIR(3));
    mvprintw(2, 0, "Ваш город отправления: ");
    attron(COLOR_PAIR(2));
    mvprintw(2, 23, city_from.c_str());
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    mvprintw(4, 0, "######//~~~~~~//######");
    attroff(COLOR_PAIR(3));

    move(6, 0);

    printw("Введите город прибытия");

    move(7, 0);

    refresh();

    string city_to;
    int city_to_id = -1;
    city_to = handle_string_input(7);

    while (city_to_id == -1)
    {
        city_to_id = validate_city_input(graph.get_cities(), city_to);
        if (city_to_id == -1)
        {
            clear_n_lines(9, 10);
            mvprintw(9, 0, "Введите корректный город прибытия");
            move(10, 0);
            city_to = handle_string_input(10);
            if (city_to == EXIT_APP)
            {
                return make_pair(-1, -1);
            }
        }
    }

    clear_n_lines(6, 10);

    attron(COLOR_PAIR(3));
    mvprintw(6, 0, "######//~~~~~~//######");
    attroff(COLOR_PAIR(3));
    mvprintw(8, 0, "Ваш город прибытия: ");
    attron(COLOR_PAIR(2));
    mvprintw(8, 20, city_to.c_str());
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    mvprintw(10, 0, "######//~~~~~~//######");
    attroff(COLOR_PAIR(3));

    wait_for_enter(12);

    return make_pair(city_from_id, city_to_id);
}

CityId handle_single_city_input(Graph &graph)
{
    curs_set(1);
    printw("Введите город отправления");

    move(1, 0);

    refresh();

    string city_from;
    int city_from_id = -1;
    city_from = handle_string_input(1);

    while (city_from_id == -1)
    {
        city_from_id = validate_city_input(graph.get_cities(), city_from);
        if (city_from_id == -1)
        {
            clear_n_lines(3, 4);
            mvprintw(3, 0, "Введите корректный город отправления");
            move(4, 0);
            city_from = handle_string_input(4);
            if (city_from == EXIT_APP)
            {
                return -1;
            }
        }
    }

    clear_n_lines(0, 4);

    attron(COLOR_PAIR(3));
    mvprintw(0, 0, "######//~~~~~~//######");
    attroff(COLOR_PAIR(3));
    mvprintw(2, 0, "Ваш город отправления: ");
    attron(COLOR_PAIR(2));
    mvprintw(2, 23, city_from.c_str());
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    mvprintw(4, 0, "######//~~~~~~//######");
    attroff(COLOR_PAIR(3));

    wait_for_enter(6);

    return city_from_id;
}

long handle_cost_limit_input()
{
    curs_set(1);
    printw("Введите ограничение по стоимости");

    move(1, 0);

    refresh();

    string cost_limit_input;
    long cost_limit = -1;
    cost_limit_input = handle_int_input();

    while (cost_limit == -1)
    {
        cost_limit = validate_number_input(cost_limit_input);
        if (cost_limit == -1)
        {
            clear_n_lines(3, 4);
            mvprintw(3, 0, "Число отрицательно или введено некорректно, повторите попытку");
            move(4, 0);
            cost_limit_input = handle_int_input();
            if (cost_limit_input == EXIT_APP)
            {
                return -1;
            }
        }
    }

    clear_n_lines(0, 4);

    attron(COLOR_PAIR(3));
    mvprintw(0, 0, "######//~~~~~~//######");
    attroff(COLOR_PAIR(3));
    mvprintw(2, 0, "Ограничение по стоимости: ");
    attron(COLOR_PAIR(2));
    mvprintw(2, 26, cost_limit_input.c_str());
    mvprintw(2, 27 + cost_limit_input.length(), "рублей");
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    mvprintw(4, 0, "######//~~~~~~//######");
    attroff(COLOR_PAIR(3));

    wait_for_enter(6);

    return cost_limit;
}

long handle_time_limit_input()
{
    curs_set(1);
    printw("Введите ограничение по времени");

    move(1, 0);

    refresh();

    string time_limit_input;
    long time_limit = -1;
    time_limit_input = handle_int_input();

    while (time_limit == -1)
    {
        time_limit = validate_number_input(time_limit_input);
        if (time_limit == -1)
        {
            clear_n_lines(3, 4);
            mvprintw(3, 0, "Число отрицательно или введено некорректно, повторите попытку");
            move(4, 0);
            time_limit_input = handle_int_input();
            if (time_limit_input == EXIT_APP)
            {
                return -1;
            }
        }
    }

    clear_n_lines(0, 4);

    attron(COLOR_PAIR(3));
    mvprintw(0, 0, "######//~~~~~~//######");
    attroff(COLOR_PAIR(3));
    mvprintw(2, 0, "Ограничение по времени: ");
    attron(COLOR_PAIR(2));
    mvprintw(2, 24, time_limit_input.c_str());
    mvprintw(2, 25 + time_limit_input.length(), "у. е.");
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    mvprintw(4, 0, "######//~~~~~~//######");
    attroff(COLOR_PAIR(3));

    wait_for_enter(6);

    return time_limit;
}

unordered_set<TransportId> handle_restricted_transport_list_input(Graph &graph)
{
    curs_set(1);
    printw("Введите список транспорта нежелательного для передвижения в таком формате (автобус/самолет/.../)");

    bool input_is_valid = false;

    int row_number = 0;
    unordered_set<TransportId> valid_transport_id_list;
    while (!input_is_valid)
    {

        move(row_number + 1, 0);
        clrtoeol();

        refresh();

        unordered_set<string> invalid_transport_title_list;
        valid_transport_id_list.clear();
        string current_transport;

        string transport_list_str = handle_string_input(row_number + 1);
        for (const auto &c : transport_list_str)
        {
            if (c == '/')
            {
                const auto valid_transport_id = validate_transport_input(graph.get_transports(), current_transport);
                if (valid_transport_id != -1)
                {
                    valid_transport_id_list.insert(valid_transport_id);
                }
                else
                {
                    invalid_transport_title_list.insert(current_transport);
                }
                current_transport.clear();
            }
            else
            {
                current_transport.push_back(c);
            }
        }

        if (!invalid_transport_title_list.empty())
        {
            move(4, 0);
            clrtoeol();
            move(6, 0);
            clrtoeol();

            mvprintw(4, 0, "Транспорт введен некорректно или не найден");

            string invalid_input;
            for (const auto &item : invalid_transport_title_list)
            {
                invalid_input += item + " ";
            }

            attron(COLOR_PAIR(4));
            mvprintw(6, 0, invalid_input.c_str());
            attroff(COLOR_PAIR(4));

            mvprintw(8, 0, "Введите список снова");
            row_number = 8;
        }
        else
        {
            clear_n_lines(0, 9);

            if (valid_transport_id_list.empty())
            {
                mvprintw(0, 0, "Ограничений на транспорт нет");
                wait_for_enter(2);
            }
            else
            {
                attron(COLOR_PAIR(3));
                mvprintw(0, 0, "######//~~~~~~//######");
                attroff(COLOR_PAIR(3));
                mvprintw(2, 0, "Следующий транспорт ограничен");

                string valid_input;
                for (const auto &transport_id : valid_transport_id_list)
                {
                    valid_input += graph.get_transport(transport_id).title + " ";
                }

                attron(COLOR_PAIR(5));
                mvprintw(4, 0, valid_input.c_str());
                attroff(COLOR_PAIR(5));
                attron(COLOR_PAIR(3));
                mvprintw(6, 0, "######//~~~~~~//######");
                attroff(COLOR_PAIR(3));
                wait_for_enter(8);
            }
            input_is_valid = true;
        }
    }

    return valid_transport_id_list;
}

// int main(int argc, char **argv)
// {
//     setlocale(LC_ALL, "ru_RU.UTF-8");
//     auto graph = Graph();
//     auto logger = Logger(graph);
//     if (argc < 2)
//     {
//         handle_file_input(graph, "../input.txt");
//     }
//     else
//     {
//         handle_file_input(graph, argv[1]);
//     }
//     const auto graphSearch = GraphSearch(graph, {});
//     const auto path = graphSearch.find_min_by_fare_among_shortest_path(0, 1);
//     for (const auto &[key, city] : graph.get_cities())
//     {
//         cout << city.title << endl;
//     }
// } // test

int main(int argc, char **argv)
{
    float total_time;
    // long long total_memory;

    setlocale(LC_ALL, "ru_RU.UTF-8");

    auto graph = Graph();
    auto logger = Logger(graph);

    if (argc < 2)
    {
        handle_file_input(graph, "../input.txt");
    }
    else
    {
        handle_file_input(graph, argv[1]);
    }

    initscr();
    noecho();
    cbreak();
    halfdelay(5);

    // scrollok(stdscr, true);
    start_color();

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, 10, COLOR_BLACK);
    init_pair(3, 13, COLOR_BLACK);
    init_pair(4, 15, COLOR_BLACK);
    init_pair(5, COLOR_RED, COLOR_BLACK);

    keypad(stdscr, true);

    bool proccess_in_interaction = true;

    while (proccess_in_interaction)
    {

        const auto result = handle_mode_input();

        clear();
        refresh();

        if (result != -1)
        {
            if (result < 3)
            {
                const auto city_pair = handle_pair_of_cities_input(graph);
                if (city_pair.first == -1)
                {
                    break;
                }
                else
                {
                    clear();
                    refresh();
                    const auto restricted_transports = handle_restricted_transport_list_input(graph);
                    const auto graphSearch = GraphSearch(graph, restricted_transports);
                    clear();
                    refresh();

                    switch (result)
                    {
                    case 0:
                    {
                        const auto start_time = std::chrono::high_resolution_clock::now();
                        const auto path = graphSearch.find_min_by_fare_among_shortest_path(city_pair.first, city_pair.second);
                        total_time = estimate_time(start_time.time_since_epoch());

                        const auto path_string = logger.path_to_string(path);

                        printw("Искомый путь: ");
                        attron(COLOR_PAIR(4));
                        mvprintw(2, 0, path_string.c_str());
                        attroff(COLOR_PAIR(4));

                        const auto log_title = "Путь минимальной стоимости среди кратчайших по времени";
                        const auto city_from_title = "Город отправления: " + graph.get_city(city_pair.first).title;
                        const auto city_to_title = "Город прибытия: " + graph.get_city(city_pair.second).title;
                        log_paths_and_data(log_title, path_string, total_time, city_from_title, city_to_title);

                        break;
                    }
                    case 1:
                    {
                        const auto start_time = std::chrono::high_resolution_clock::now();
                        const auto path = graphSearch.find_min_by_fare_path(city_pair.first, city_pair.second);
                        total_time = estimate_time(start_time.time_since_epoch());

                        const auto path_string = logger.path_to_string(path);

                        printw("Искомый путь: ");
                        attron(COLOR_PAIR(4));
                        mvprintw(2, 0, path_string.c_str());
                        attroff(COLOR_PAIR(4));

                        const auto log_title = "Путь минимальной стоимости";
                        const auto city_from_title = "Город отправления: " + graph.get_city(city_pair.first).title;
                        const auto city_to_title = "Город прибытия: " + graph.get_city(city_pair.second).title;
                        log_paths_and_data(log_title, path_string, total_time, city_from_title, city_to_title);

                        break;
                    }
                    case 2:
                    {
                        const auto start_time = std::chrono::high_resolution_clock::now();
                        const auto path = graphSearch.find_min_by_cities_path(city_pair.first, city_pair.second);
                        total_time = estimate_time(start_time.time_since_epoch());

                        const auto path_string = logger.path_to_string(path);

                        printw("Искомый путь: ");
                        attron(COLOR_PAIR(4));
                        mvprintw(2, 0, path_string.c_str());
                        attroff(COLOR_PAIR(4));

                        const auto log_title = "Путь, минимальный по числу посещенных городов";
                        const auto city_from_title = "Город отправления: " + graph.get_city(city_pair.first).title;
                        const auto city_to_title = "Город прибытия: " + graph.get_city(city_pair.second).title;
                        log_paths_and_data(log_title, path_string, total_time, city_from_title, city_to_title);

                        break;
                    }
                    default:
                        break;
                    }
                }
            }
            else
            {
                const auto city_from_id = handle_single_city_input(graph);
                clear();
                refresh();
                long limit;
                if (result == 3)
                {
                    limit = handle_cost_limit_input();
                }
                else
                {
                    limit = handle_time_limit_input();
                }
                clear();
                refresh();
                const auto restricted_transports = handle_restricted_transport_list_input(graph);
                const auto graphSearch = GraphSearch(graph, restricted_transports);
                clear();
                refresh();

                switch (result)
                {
                case 3:
                {
                    const auto start_time = std::chrono::high_resolution_clock::now();
                    const auto paths_to_cities = graphSearch.find_cities_in_limit_cost(city_from_id, limit);
                    total_time = estimate_time(start_time.time_since_epoch());

                    printw("Искомые пути: ");
                    const auto paths_to_cities_string = logger.paths_to_cities_to_string(paths_to_cities);
                    attron(COLOR_PAIR(4));
                    mvprintw(2, 0, paths_to_cities_string.c_str());
                    attroff(COLOR_PAIR(4));

                    string log_title = "Множество городов (и минимальных по стоимости путей к ним), достижимых из ";
                    log_title += graph.get_city(city_from_id).title + " не более чем за " + to_string(limit);
                    log_paths_and_data(log_title, paths_to_cities_string, total_time, "", "");

                    break;
                }
                case 4:
                {
                    const auto start_time = std::chrono::high_resolution_clock::now();
                    const auto paths_to_cities = graphSearch.find_cities_in_limit_time(city_from_id, limit);
                    total_time = estimate_time(start_time.time_since_epoch());

                    printw("Искомые пути: ");
                    const auto paths_to_cities_string = logger.paths_to_cities_to_string(paths_to_cities);
                    attron(COLOR_PAIR(4));
                    mvprintw(2, 0, paths_to_cities_string.c_str());
                    attroff(COLOR_PAIR(4));

                    string log_title = "Множество городов (и минимальных по времени путей к ним), достижимых из ";
                    log_title += graph.get_city(city_from_id).title + " не более чем за " + to_string(limit);
                    log_paths_and_data(log_title, paths_to_cities_string, total_time, "", "");

                    break;
                }
                default:
                    break;
                }
            }
            // refresh();
        }
        else
        {
            break;
        }

        char c;

        while ((c = getch()) != 10)
        {
            // break;
        }
        // wait_for_enter();

        clear();
        refresh();

        const auto chosen_mode = handle_finish_app();
        if (chosen_mode == 0)
        {
            proccess_in_interaction = false;
        }
    }
    // curs_set(0);
    endwin();
}