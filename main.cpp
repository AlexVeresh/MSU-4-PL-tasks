#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <set>
#include <ncurses.h>
#include <locale>
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

void handle_file_input(Graph &graph)
{
    string result;

    ifstream input("input_large.txt");

    while (getline(input, result))
    {
        string item;
        string cityPart;
        stringstream ss(result);
        int iter = 0;

        Graph::CityId from_city_id;
        Graph::CityId to_city_id;
        string transport_title;
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
                item.erase(remove(item.begin(), item.end(), '"'), item.end());

                switch (iter)
                {
                case 0:
                    from_city_id = graph.add_city(item);
                    break;
                case 1:
                    to_city_id = graph.add_city(item);
                    break;
                case 2:
                    transport_title = item;
                    break;
                case 3:
                    time = stoi(item);
                    break;
                case 4:
                    fare = stoi(item);
                    graph.add_cruise(from_city_id, to_city_id, transport_title, time, fare);
                    break;
                }
                iter++;
            }
        }
    }

    input.close();
}

int handle_mode_input(WINDOW *w)
{
    curs_set(0);
    Graph::TransportId chosen_mode;
    const unsigned int modes_count = 5;
    const string modes[modes_count] = {
        " 1. Среди кратчайших по времени путей между двумя городами найти путь минимальной стоимости",
        " 2. Среди путей между двумя городами найти путь минимальной стоимости",
        " 3. Среди путей между двумя городами найти путь минимальный по числу посещенных городов",
        " 4. Найти множество городов (и минимальных по стоимости путей к ним), достижимых из города отправления\n     не более чем за обозначенную сумму денег",
        " 5. Найти множество городов (и минимальных по времени путей к ним), достижимых из города отправления\n     не более чем за обозначенное время"};
    int user_choice;
    bool is_proccessing = true;

    wclear(w);
    wrefresh(w);

    mvwprintw(w, 0, 0, "Добро пожаловать! Выберите режим работы");

    while (is_proccessing)
    {

        for (int i = 0; i < modes_count; i++)
        {
            if (chosen_mode == i)
            {
                wattron(w, A_REVERSE);
            }
            if (i == 4)
            {
                mvwprintw(w, i + 3, 0, modes[i].c_str());
            }
            else
            {
                mvwprintw(w, i + 2, 0, modes[i].c_str());
            }
            wattroff(w, A_REVERSE);
        }
        user_choice = wgetch(w);

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

        if (user_choice == 27)
        {
            is_proccessing = false;
            return -1;
        }
        if (user_choice == 10)
        {
            is_proccessing = false;
            return chosen_mode;
        }
    }
    return -1;
}

int handle_finish_app(WINDOW *w)
{
    curs_set(0);
    const unsigned int modes_count = 2;
    int chosen_mode = 0;
    const string modes[modes_count] = {
        " 1. Завершить программу",
        " 2. Вернутся на страницу выбора режима работы"};
    int user_choice;
    bool is_proccessing = true;

    mvwprintw(w, 0, 0, "Выберите режим работы");

    while (is_proccessing)
    {

        for (int i = 0; i < modes_count; i++)
        {
            if (chosen_mode == i)
            {
                wattron(w, A_REVERSE);
            }
            if (i == 4)
            {
                mvwprintw(w, i + 3, 0, modes[i].c_str());
            }
            else
            {
                mvwprintw(w, i + 2, 0, modes[i].c_str());
            }
            wattroff(w, A_REVERSE);
        }
        user_choice = wgetch(w);

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

string handle_string_input(int row)
{
    int c;
    string str = "";
    int i = 0;
    while ((c = getch()) != 10)
    {
        if (c != -1)
        {
            if (c == 127)
            {
                clrtoeol();

                if (i > 0)
                {
                    str.pop_back();
                    i--;
                    move(row, i);
                }
            }
            else
            {
                attron(COLOR_PAIR(4));
                printw("%c", c);
                attroff(COLOR_PAIR(4));
                str.push_back(char(c));
                i++;
            }
        }
    }
    return str;
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

    int margin_1 = 0;
    while (city_from_id == -1)
    {
        city_from_id = validate_city_input(graph.get_cities(), city_from);
        if (city_from_id == -1)
        {
            mvprintw(3 + margin_1, 0, "Введите корректный город отправления");
            move(4 + margin_1, 0);
            city_from = handle_string_input(4 + margin_1);
            margin_1 += 2;
        }
    }

    mvprintw(margin_1 + 3, 0, "######//~~~~~~//######");
    mvprintw(margin_1 + 5, 0, "Ваш город отправления: ");
    mvprintw(margin_1 + 5, 23, city_from.c_str());
    mvprintw(margin_1 + 7, 0, "######//~~~~~~//######");

    move(margin_1 + 9, 0);

    printw("Введите город прибытия");

    move(margin_1 + 10, 0);

    refresh();

    string city_to;
    int city_to_id = -1;
    city_to = handle_string_input(margin_1 + 10);

    int margin_2 = 0;
    while (city_to_id == -1)
    {
        city_to_id = validate_city_input(graph.get_cities(), city_to);
        if (city_to_id == -1)
        {
            mvprintw(12 + margin_1 + margin_2, 0, "Введите корректный город прибытия");
            move(13 + margin_1 + margin_2, 0);
            city_to = handle_string_input(13 + margin_1 + margin_2);
            margin_2 += 2;
        }
    }

    mvprintw(margin_1 + margin_2 + 12, 0, "######//~~~~~~//######");
    mvprintw(margin_1 + margin_2 + 14, 0, "Ваш город прибытия: ");
    mvprintw(margin_1 + margin_2 + 14, 20, city_to.c_str());
    mvprintw(margin_1 + margin_2 + 16, 0, "######//~~~~~~//######");

    move(margin_1 + margin_2 + 17, 0);

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
            move(3, 0);
            clrtoeol();
            move(4, 0);
            clrtoeol();
            mvprintw(3, 0, "Введите корректный город отправления");
            move(4, 0);
            city_from = handle_string_input(4);
        }
    }

    move(3, 0);
    clrtoeol();
    move(4, 0);
    clrtoeol();

    attron(COLOR_PAIR(3));
    mvprintw(3, 0, "######//~~~~~~//######");
    attroff(COLOR_PAIR(3));
    mvprintw(5, 0, "Ваш город отправления: ");
    attron(COLOR_PAIR(2));
    mvprintw(5, 23, city_from.c_str());
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    mvprintw(7, 0, "######//~~~~~~//######");
    attroff(COLOR_PAIR(3));

    move(9, 0);

    wait_for_enter(9);

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
    cost_limit_input = handle_string_input(1);

    int margin_1 = 0;
    while (cost_limit == -1)
    {
        cost_limit = validate_number_input(cost_limit_input);
        if (cost_limit == -1)
        {
            mvprintw(3 + margin_1, 0, "Число отрицательно или введено некорректно, повторите попытку");
            move(4 + margin_1, 0);
            cost_limit_input = handle_string_input(4 + margin_1);
            margin_1 += 2;
        }
    }

    mvprintw(margin_1 + 3, 0, "######//~~~~~~//######");
    mvprintw(margin_1 + 5, 0, "Ограничение по стоимости: ");
    mvprintw(margin_1 + 5, 24, cost_limit_input.c_str());
    mvprintw(margin_1 + 5, 25 + cost_limit_input.length(), "часов");
    mvprintw(margin_1 + 7, 0, "######//~~~~~~//######");

    move(margin_1 + 9, 0);

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
    time_limit_input = handle_string_input(1);

    int margin_1 = 0;
    while (time_limit == -1)
    {
        time_limit = validate_number_input(time_limit_input);
        if (time_limit == -1)
        {
            mvprintw(3 + margin_1, 0, "Число отрицательно или введено некорректно, повторите попытку");
            move(4 + margin_1, 0);
            time_limit_input = handle_string_input(4 + margin_1);
            margin_1 += 2;
        }
    }

    mvprintw(margin_1 + 3, 0, "######//~~~~~~//######");
    mvprintw(margin_1 + 5, 0, "Ограничение по времени: ");
    mvprintw(margin_1 + 5, 24, time_limit_input.c_str());
    mvprintw(margin_1 + 5, 25 + time_limit_input.length(), "часов");
    mvprintw(margin_1 + 7, 0, "######//~~~~~~//######");

    move(margin_1 + 9, 0);

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

            int padding = 0;
            for (const auto &item : invalid_transport_title_list)
            {
                mvprintw(6, padding, item.c_str());
                padding += item.length();
                mvprintw(6, padding + 1, " ");
                padding += 2;
            }

            mvprintw(8, 0, "Введите список снова");
            row_number = 8;
        }
        else
        {
            move(4, 0);
            clrtoeol();
            move(6, 0);
            clrtoeol();

            if (valid_transport_id_list.empty())
            {
                mvprintw(2, 0, "Ограничений на транспорт нет");
                mvprintw(4, 0, "Нажмите ENTER чтобы продолжить");
                move(5, 0);
            }
            else
            {
                mvprintw(3, 0, "######//~~~~~~//######");
                mvprintw(4, 0, "Следующий транспорт ограничен");

                int padding = 0;
                for (const auto &transport_id : valid_transport_id_list)
                {
                    mvprintw(6, padding, graph.get_transport(transport_id).title.c_str());
                    padding += graph.get_transport(transport_id).title.length();
                    mvprintw(6, padding + 1, " ");
                    padding += 2;
                }

                move(8, 0);
                clrtoeol();
                move(9, 0);
                clrtoeol();
                move(8, 0);

                mvprintw(8, 0, "######//~~~~~~//######");
                mvprintw(10, 0, "Нажмите ENTER чтобы продолжить");
                move(11, 0);
            }
            input_is_valid = true;
        }
    }

    // refresh();

    char c;
    while ((c = getch()) != 10)
    {
    }

    return valid_transport_id_list;
}

// int main()
// {
//     setlocale(LC_ALL, "ru_RU.UTF-8");
//     auto graph = Graph();
//     auto logger = Logger(graph);
//     handle_file_input(graph);
//     const auto graphSearch = GraphSearch(graph, {});
//     const auto path = graphSearch.find_min_by_fare_among_shortest_path(0, 1);
//     cout << logger.path_to_string(path) << endl;

// } // test

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8");

    auto graph = Graph();
    auto logger = Logger(graph);
    handle_file_input(graph);

    initscr();
    noecho();
    cbreak();
    halfdelay(5);

    int x_max, y_max;
    getmaxyx(stdscr, y_max, x_max);

    WINDOW *w = newwin(0, 0, 0, 0);

    scrollok(w, true);
    start_color();

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, 10, COLOR_BLACK);
    init_pair(3, 13, COLOR_BLACK);
    init_pair(4, 15, COLOR_BLACK);

    keypad(w, true);

    bool proccess_in_interaction = true;

    while (proccess_in_interaction)
    {

        const auto result = handle_mode_input(w);

        clear();
        refresh();

        if (result != -1)
        {
            if (result < 3)
            {
                const auto city_pair = handle_pair_of_cities_input(graph);
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
                    const auto path = graphSearch.find_min_by_fare_among_shortest_path(city_pair.first, city_pair.second);
                    const auto path_string = logger.path_to_string(path);

                    printw("Искомый путь: ");
                    mvprintw(2, 0, path_string.c_str());
                    break;
                }
                case 1:
                {
                    const auto path = graphSearch.find_min_by_fare_path(city_pair.first, city_pair.second);
                    const auto path_string = logger.path_to_string(path);

                    printw("Искомый путь: ");
                    mvprintw(2, 0, path_string.c_str());
                    break;
                }
                case 2:
                {
                    const auto path = graphSearch.find_min_by_cities_path(city_pair.first, city_pair.second);
                    const auto path_string = logger.path_to_string(path);

                    printw("Искомый путь: ");
                    mvprintw(2, 0, path_string.c_str());
                    break;
                }
                default:
                    break;
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
                    const auto paths_to_cities = graphSearch.find_cities_in_limit_cost(city_from_id, limit);

                    printw("Искомые пути: ");

                    const auto paths_to_cities_string = logger.paths_to_cities_to_string(paths_to_cities);
                    mvprintw(2, 0, paths_to_cities_string.c_str());

                    break;
                }
                case 4:
                {
                    const auto paths_to_cities = graphSearch.find_cities_in_limit_time(city_from_id, limit);

                    printw("Искомые пути: ");

                    const auto paths_to_cities_string = logger.paths_to_cities_to_string(paths_to_cities);
                    mvprintw(2, 0, paths_to_cities_string.c_str());

                    break;
                }
                default:
                    break;
                }
            }
            // refresh();
        }

        char c;

        while ((c = getch()) != 10)
        {
            // break;
        }

        clear();
        refresh();

        const auto chosen_mode = handle_finish_app(w);
        if (chosen_mode == 0)
        {
            proccess_in_interaction = false;
        }
    }
    // curs_set(0);
    endwin();
}