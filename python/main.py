import sys
import curses as cs
from presentation import UiController
from graph import Graph, Cruise
from logger import Logger
from graph_search import GraphSearch
from time import time
from resource import getrusage, RUSAGE_SELF
from datetime import date
from platform import python_version


def high_divider() -> str: return "•"*120


def estimate_time(start_time):
    end_time = time()
    return end_time - start_time


def log_paths_and_data(mode_title, response, estimated_time, city_from_title, city_to_title) -> None:
    f = open("logs.txt", 'r')
    previous_content = f.read()
    result = ''
    f.close()

    f = open("logs.txt", 'w')

    if len(previous_content) != 0 and previous_content != '\n':
        result += previous_content
        result += '\n'
        result += high_divider()
        result += '\n'
        result += high_divider()
        result += '\n\n'

    result += 'Язык: Python(' + python_version() + ')\n'
    result += 'Дата: ' + str(date.today()) + '\n'
    result += 'Режим работы: ' + mode_title

    if len(city_from_title) != 0:
        result += '\n'
        result += city_from_title
        result += '\n'
        result += city_to_title

    result += "\n"
    result += response
    result += "\n"
    result += "Время выполнения алгоритма поиска: " + \
        str(estimated_time) + " секунд"
    rusage = getrusage(RUSAGE_SELF)
    result += '\n' + "Памяти использовано: " + str(rusage.ru_maxrss) + '\n'

    f.write(result)
    f.close()


def handle_file_input(graph: Graph, filename: str) -> None:
    lines = open(filename, 'r').readlines()

    for line in lines:
        iter = 0
        cityPart = ''
        from_city_id = 0
        to_city_id = 0
        transport_id = 0
        time = 0
        fare = 0

        for item in line.split(' '):
            if item != '\n':
                if item[0] == '"' and item[-1] != '"':
                    cityPart = item
                    continue

                if cityPart != "":
                    item = cityPart + ' ' + item
                    cityPart = ""

                item = item.replace('"', '')

                if iter == 0:
                    from_city_id = graph.add_city(item)

                elif iter == 1:
                    to_city_id = graph.add_city(item)

                elif iter == 2:
                    transport_id = graph.add_transport(item)

                elif iter == 3:
                    time = int(item)

                elif iter == 4:
                    fare = int(item)
                    new_cruise = Cruise(
                        from_city_id, to_city_id, transport_id, time, fare)
                    graph.add_cruise(new_cruise)

                iter += 1


def start(stdscr) -> str:
    res = ''
    graph = Graph()
    logger = Logger(graph)

    if len(sys.argv) < 2:
        return 'Введите название файла, содержащего города'

    else:
        handle_file_input(graph, sys.argv[1])

    uiCrontroller = UiController(stdscr=stdscr, graph=graph)
    proccess_in_interaction = True

    while proccess_in_interaction:
        result = uiCrontroller.handle_mode_input()

        uiCrontroller.clear()
        uiCrontroller.refresh()

        if result != -1:
            if result < 3:
                from_city_id, to_city_id = uiCrontroller.handle_pair_of_cities_input()
                if from_city_id == -1:
                    break

                else:
                    uiCrontroller.clear()
                    uiCrontroller.refresh()
                    restricted_transports = uiCrontroller.handle_restricted_transport_list_input()
                    graphSearch = GraphSearch(graph, restricted_transports)
                    uiCrontroller.clear()
                    uiCrontroller.refresh()

                    if result == 0:
                        start_time = time()
                        path = graphSearch.find_min_by_fare_among_shortest_path(
                            from_city_id, to_city_id)
                        total_time = estimate_time(start_time)

                        path_string = logger.path_to_string(path)
                        uiCrontroller.print_result(
                            "Искомый путь: ", path_string)

                        log_title = "Путь минимальной стоимости среди кратчайших по времени"
                        city_from_title = "Город отправления: " + \
                            graph.get_city(from_city_id).title
                        city_to_title = "Город прибытия: " + \
                            graph.get_city(to_city_id).title
                        log_paths_and_data(
                            log_title, path_string, total_time, city_from_title, city_to_title)

                    elif result == 1:
                        start_time = time()
                        path = graphSearch.find_min_by_fare_path(
                            from_city_id, to_city_id)
                        total_time = estimate_time(
                            start_time)

                        path_string = logger.path_to_string(path)
                        uiCrontroller.print_result(
                            "Искомый путь: ", path_string)

                        log_title = "Путь минимальной стоимости"
                        city_from_title = "Город отправления: " + \
                            graph.get_city(from_city_id).title
                        city_to_title = "Город прибытия: " + \
                            graph.get_city(to_city_id).title
                        log_paths_and_data(
                            log_title, path_string, total_time, city_from_title, city_to_title)

                    elif result == 2:
                        start_time = time()
                        path = graphSearch.find_min_by_cities_path(
                            from_city_id, to_city_id)
                        total_time = estimate_time(
                            start_time)

                        path_string = logger.path_to_string(path)
                        uiCrontroller.print_result(
                            "Искомый путь: ", path_string)

                        log_title = "Путь, минимальный по числу посещенных городов"
                        city_from_title = "Город отправления: " + \
                            graph.get_city(from_city_id).title
                        city_to_title = "Город прибытия: " + \
                            graph.get_city(to_city_id).title
                        log_paths_and_data(
                            log_title, path_string, total_time, city_from_title, city_to_title)

            else:
                city_from_id = uiCrontroller.handle_single_city_input()
                uiCrontroller.clear()
                uiCrontroller.refresh()
                limit = 0

                if result == 3:
                    limit = uiCrontroller.handle_cost_limit_input()

                else:
                    limit = uiCrontroller.handle_time_limit_input()

                uiCrontroller.clear()
                uiCrontroller.refresh()

                restricted_transports = uiCrontroller.handle_restricted_transport_list_input()
                graphSearch = GraphSearch(graph, restricted_transports)

                uiCrontroller.clear()
                uiCrontroller.refresh()

                if result == 3:
                    start_time = time()
                    paths_to_cities = graphSearch.find_cities_in_limit_cost(
                        city_from_id, limit)
                    total_time = estimate_time(start_time)

                    paths_to_cities_string = logger.paths_to_cities_to_string(
                        paths_to_cities)
                    uiCrontroller.print_result(
                        "Искомые пути: ", paths_to_cities_string)

                    log_title = "Множество городов (и минимальных по стоимости путей к ним), достижимых из "
                    log_title += graph.get_city(city_from_id).title + \
                        " не более чем за " + str(limit)
                    log_paths_and_data(
                        log_title, paths_to_cities_string, total_time, "", "")

                elif result == 4:
                    start_time = time()
                    paths_to_cities = graphSearch.find_cities_in_limit_time(
                        city_from_id, limit)
                    total_time = estimate_time(start_time)

                    paths_to_cities_string = logger.paths_to_cities_to_string(
                        paths_to_cities)
                    uiCrontroller.print_result(
                        "Искомые пути: ", paths_to_cities_string)

                    log_title = "Множество городов (и минимальных по времени путей к ним), достижимых из "
                    log_title += graph.get_city(city_from_id).title + \
                        " не более чем за " + str(limit)
                    log_paths_and_data(
                        log_title, paths_to_cities_string, total_time, "", "")

        else:
            break

        c = uiCrontroller.stdscr.getch()
        while c != 10:
            c = uiCrontroller.stdscr.getch()

        uiCrontroller.clear()
        uiCrontroller.refresh()

        chosen_mode = uiCrontroller.handle_finish_app()

        if chosen_mode == 0:
            proccess_in_interaction = False

    uiCrontroller.endwin()

    return res


def main():
    res = cs.wrapper(start)
    print(res)


if __name__ == '__main__':
    main()
