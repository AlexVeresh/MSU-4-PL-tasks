from graph import Graph, Cruise
from path import Path


class Logger:
    def __init__(self, graph: Graph) -> None:
        self.__graph = graph

    def cruise_to_str(self, cruise: Cruise) -> str:
        return str(self.__graph.get_city(cruise.from_city_id)) + ' ------> ' + \
            str(self.__graph.get_city(cruise.to_city_id)) + ' (Время: ' + \
            str(cruise.time) + ' у. е. | Цена: ' + str(cruise.fare) + ' ₽)'

    def path_to_string(self, path: Path) -> str:
        result = ""

        for i in range(path.total_length()):
            cruise = self.__graph.get_cruise(path[i])
            result += self.cruise_to_str(cruise)

            if i != (path.total_length() - 1):
                result += '\n'

        return result

    def paths_to_cities_to_string(self, paths_to_cities: dict[int, Path]) -> str:
        result = ''

        for city_to_id, path in paths_to_cities.items():
            city_to = self.__graph.get_city(city_to_id)
            result += "До города " + city_to.title + \
                "\n" + self.path_to_string(path) + "\n\n"

        return result
