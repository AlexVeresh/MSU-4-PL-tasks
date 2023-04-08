from queue import Queue
from graph import Graph
from path import Path
from math import inf
import heapq as h


class GraphSearch:
    def __init__(self, graph: Graph, restricted_transports: set[int]) -> None:
        self.__graph = graph
        self.__restricted_transports = restricted_transports

    def find_min_by_fare_among_shortest_path(self, start_city_id, finish_city_id) -> Path:
        cities_count = len(self.__graph.cities)

        distancies_to_cities = [inf] * cities_count
        total_fares_to_cities = [inf] * cities_count
        distancies_to_cities[start_city_id] = 0
        total_fares_to_cities[start_city_id] = 0

        pathes_to_all_cities = [Path()] * cities_count

        current_city_ids = [
            (distancies_to_cities[start_city_id], start_city_id)]
        h.heapify(current_city_ids)

        while len(current_city_ids) != 0:
            current_item = h.heappop(current_city_ids)

            if current_item[0] > distancies_to_cities[current_item[1]]:
                continue

            from_city_id = current_item[1]

            for cruise_id in self.__graph.get_city(from_city_id).cruise_ids:
                to_city_id = self.__graph.get_cruise(cruise_id).to_city_id
                time = self.__graph.get_cruise(cruise_id).time
                fare = self.__graph.get_cruise(cruise_id).fare
                transport_type_id = self.__graph.get_cruise(
                    cruise_id).transport_type_id

                if self.__is_transport_valid(transport_type_id):
                    if distancies_to_cities[from_city_id] + time < distancies_to_cities[to_city_id]:
                        distancies_to_cities[to_city_id] = distancies_to_cities[from_city_id] + time
                        total_fares_to_cities[to_city_id] = total_fares_to_cities[from_city_id] + fare
                        pathes_to_all_cities[to_city_id] = pathes_to_all_cities[from_city_id] + \
                            self.__graph.get_cruise(cruise_id)
                        h.heappush(
                            current_city_ids, (distancies_to_cities[to_city_id], to_city_id))

                    elif distancies_to_cities[from_city_id] + time == distancies_to_cities[to_city_id]:
                        current_fare = total_fares_to_cities[to_city_id]
                        new_fare = total_fares_to_cities[from_city_id] + fare

                        if new_fare < current_fare:
                            distancies_to_cities[to_city_id] = distancies_to_cities[from_city_id] + time
                            total_fares_to_cities[to_city_id] = total_fares_to_cities[from_city_id] + fare
                            pathes_to_all_cities[to_city_id] = pathes_to_all_cities[from_city_id] + \
                                self.__graph.get_cruise(cruise_id)
                            h.heappush(
                                current_city_ids, (distancies_to_cities[to_city_id], to_city_id))

        return pathes_to_all_cities[finish_city_id]

    def find_min_by_fare_path(self, start_city_id, finish_city_id) -> Path:
        cities_count = len(self.__graph.cities)

        distancies_to_cities = [inf] * cities_count
        distancies_to_cities[start_city_id] = 0

        pathes_to_all_cities = [Path()] * cities_count

        current_city_ids = [
            (distancies_to_cities[start_city_id], start_city_id)]
        h.heapify(current_city_ids)

        while len(current_city_ids) != 0:
            current_item = h.heappop(current_city_ids)

            if current_item[0] > distancies_to_cities[current_item[1]]:
                continue

            from_city_id = current_item[1]

            for cruise_id in self.__graph.get_city(from_city_id).cruise_ids:
                to_city_id = self.__graph.get_cruise(cruise_id).to_city_id
                fare = self.__graph.get_cruise(cruise_id).fare
                transport_type_id = self.__graph.get_cruise(
                    cruise_id).transport_type_id

                if distancies_to_cities[from_city_id] + fare < distancies_to_cities[to_city_id] and self.__is_transport_valid(transport_type_id):
                    distancies_to_cities[to_city_id] = distancies_to_cities[from_city_id] + fare
                    pathes_to_all_cities[to_city_id] = pathes_to_all_cities[from_city_id] + \
                        self.__graph.get_cruise(cruise_id)
                    h.heappush(
                        current_city_ids, (distancies_to_cities[to_city_id], to_city_id))

        return pathes_to_all_cities[finish_city_id]

    def find_min_by_cities_path(self, start_city_id, finish_city_id) -> Path:
        cities_count = len(self.__graph.cities)

        distancies_to_cities = [inf] * cities_count
        distancies_to_cities[start_city_id] = 0

        pathes_to_all_cities = [Path()] * cities_count

        current_city_ids = Queue()
        current_city_ids.put(start_city_id)
        used = [False] * cities_count
        used[start_city_id] = True

        while not current_city_ids.empty():
            from_city_id = current_city_ids.get()

            for cruise_id in self.__graph.get_city(from_city_id).cruise_ids:
                to_city_id = self.__graph.get_cruise(cruise_id).to_city_id
                transport_type_id = self.__graph.get_cruise(
                    cruise_id).transport_type_id

                if not used[to_city_id] and self.__is_transport_valid(transport_type_id):
                    used[to_city_id] = True
                    current_city_ids.put(to_city_id)
                    distancies_to_cities[to_city_id] = distancies_to_cities[from_city_id] + 1
                    pathes_to_all_cities[to_city_id] = pathes_to_all_cities[from_city_id] + \
                        self.__graph.get_cruise(cruise_id)

        return pathes_to_all_cities[finish_city_id]

    def find_cities_in_limit_cost(self, start_city_id, limit_cost) -> dict[int, Path]:
        cities_count = len(self.__graph.cities)

        distancies_to_cities = [inf] * cities_count
        distancies_to_cities[start_city_id] = 0

        pathes_to_all_cities = [Path()] * cities_count

        current_city_ids = [
            (distancies_to_cities[start_city_id], start_city_id)]
        h.heapify(current_city_ids)

        while len(current_city_ids) != 0:
            current_item = h.heappop(current_city_ids)

            if current_item[0] > distancies_to_cities[current_item[1]]:
                continue

            from_city_id = current_item[1]

            for cruise_id in self.__graph.get_city(from_city_id).cruise_ids:
                to_city_id = self.__graph.get_cruise(cruise_id).to_city_id
                fare = self.__graph.get_cruise(cruise_id).fare
                transport_type_id = self.__graph.get_cruise(
                    cruise_id).transport_type_id

                if distancies_to_cities[from_city_id] + fare < distancies_to_cities[to_city_id] and self.__is_transport_valid(transport_type_id):
                    distancies_to_cities[to_city_id] = distancies_to_cities[from_city_id] + fare
                    pathes_to_all_cities[to_city_id] = pathes_to_all_cities[from_city_id] + \
                        self.__graph.get_cruise(cruise_id)
                    h.heappush(
                        current_city_ids, (distancies_to_cities[to_city_id], to_city_id))

        result = {}

        for i in range(len(distancies_to_cities)):
            total_fare = distancies_to_cities[i]

            if total_fare <= limit_cost and total_fare != 0:
                result[i] = pathes_to_all_cities[i]

        return result

    def find_cities_in_limit_time(self, start_city_id, limit_time) -> dict[int, Path]:
        cities_count = len(self.__graph.cities)

        distancies_to_cities = [inf] * cities_count
        distancies_to_cities[start_city_id] = 0

        pathes_to_all_cities = [Path()] * cities_count

        current_city_ids = [
            (distancies_to_cities[start_city_id], start_city_id)]
        h.heapify(current_city_ids)

        while len(current_city_ids) != 0:
            current_item = h.heappop(current_city_ids)

            if current_item[0] > distancies_to_cities[current_item[1]]:
                continue

            from_city_id = current_item[1]

            for cruise_id in self.__graph.get_city(from_city_id).cruise_ids:
                to_city_id = self.__graph.get_cruise(cruise_id).to_city_id
                time = self.__graph.get_cruise(cruise_id).time
                transport_type_id = self.__graph.get_cruise(
                    cruise_id).transport_type_id

                if distancies_to_cities[from_city_id] + time < distancies_to_cities[to_city_id] and self.__is_transport_valid(transport_type_id):
                    distancies_to_cities[to_city_id] = distancies_to_cities[from_city_id] + time
                    pathes_to_all_cities[to_city_id] = pathes_to_all_cities[from_city_id] + \
                        self.__graph.get_cruise(cruise_id)
                    h.heappush(
                        current_city_ids, (distancies_to_cities[to_city_id], to_city_id))

        result = {}

        for i in range(len(distancies_to_cities)):
            total_fare = distancies_to_cities[i]

            if total_fare <= limit_time and total_fare != 0:
                result[i] = pathes_to_all_cities[i]

        return result

    def __is_transport_valid(self, transport_id) -> bool:
        return not (transport_id in self.__restricted_transports)
