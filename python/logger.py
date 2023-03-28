from graph import Graph, Cruise
from prettytable import PrettyTable
from path import Path


class Logger:
    def __init__(self, graph: Graph) -> None:
        self.__graph = graph

    def cruise_to_str(self, cruise: Cruise) -> str:
        return str(self.__graph.get_city(cruise.from_city_id)) + ' -------> ' + \
            str(self.__graph.get_city(cruise.to_city_id)) + ' | Время: ' + \
            str(cruise.time) + 'у. е. | ' + str(cruise.fare) + '|'

    def path_to_string(self, path: Path) -> str:
        result = PrettyTable(
            ['Город отправления', 'Город прибытия', 'Время (у. е.)', 'Стоимость (₽)'])

        for i in range(path.total_length()):
            cruise = self.__graph.get_cruise(path[i])
            city_from_title = self.__graph.get_city(cruise.from_city_id).title
            city_to_title = self.__graph.get_city(cruise.to_city_id).title
            result.add_row([city_from_title, city_to_title,
                           cruise.time, cruise.fare])

        return str(result)
    
    def paths_to_cities_to_string(self, paths_to_cities : dict[int, Path]) -> str:
        result = ''

        for city_to_id, path in paths_to_cities.items():
            city_to = self.__graph.get_city(city_to_id)
            result += "До города " + city_to.title + "\n" + self.path_to_string(path) + "\n\n";

        return result