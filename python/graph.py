class City:
    def __init__(self, id, title) -> None:
        self.id = id
        self.title = title

    def add_cruise_id(self, id) -> None:
        new_ids = self.__cruise_ids.copy()
        new_ids.append(id)
        self.__cruise_ids = new_ids

    def __str__(self) -> str: return self.title

    @property
    def cruise_ids(self) -> list[int]: return self.__cruise_ids

    __cruise_ids: list[int] = []


class Transport:
    def __init__(self, id, title) -> None:
        self.id = id
        self.title = title

    def __str__(self) -> str: return self.title


class Cruise:
    def __init__(self, from_city_id, to_city_id, transport_type_id, time, fare, id=0) -> None:
        self.id = id
        self.from_city_id = from_city_id
        self.to_city_id = to_city_id
        self.transport_type_id = transport_type_id
        self.time = time
        self.fare = fare

    def __str__(self) -> str: return '(' + str(self.id) + ', ' + \
        str(self.from_city_id) + ', ' + str(self.to_city_id) + ')'


class Graph:

    def add_city(self, title) -> int:
        city_id = -1

        for key, value in self.__cities.items():
            if value.title == title:
                city_id = key
                break

        if city_id == -1:
            city_id = self.__get_new_city_id()
            self.__cities[city_id] = City(city_id, title)

        return city_id

    def add_cruise(self, cruise: Cruise) -> None:
        new_cruise_id = self.__get_new_cruise_id()
        cruise.id = new_cruise_id
        self.__cruises[new_cruise_id] = cruise
        self.__cities[cruise.from_city_id].add_cruise_id(new_cruise_id)

    def add_transport(self, title) -> int:
        transport_id = -1
        for key, value in self.__transports.items():
            if value.title == title:
                transport_id = key
                break

        if transport_id == -1:
            transport_id = self.__get_new_transport_id()
            self.__transports[transport_id] = Transport(transport_id, title)

        return transport_id

    def get_city_cruises(
        self, city_id) -> list[int]: return self.__cities[city_id].cruise_ids()

    def get_city(self, id) -> City: return self.__cities[id]

    def get_cruise(self, id) -> Cruise: return self.__cruises[id]

    def get_transport(self, id) -> Transport: return self.__transports[id]

    @property
    def cities(self) -> dict[int, City]: return self.__cities

    @property
    def cruises(self) -> dict[int, Cruise]: return self.__cruises

    @property
    def transports(self) -> dict[int, Transport]: return self.__transports

    def get_cities_amount(self) -> int: return self.__cities.size()

    def get_cruises_amount(self) -> int: return self.__cruises.size()

    __cities: dict[int, City] = {}
    __cruises: dict[int, Cruise] = {}
    __transports: dict[int, Transport] = {}
    __city_id_counter: int = 0
    __cruise_id_counter: int = 0
    __transport_id_counter: int = 0

    def __get_new_city_id(self) -> int:
        result = self.__city_id_counter
        self.__city_id_counter += 1
        return result

    def __get_new_cruise_id(self) -> int:
        result = self.__cruise_id_counter
        self.__cruise_id_counter += 1
        return result

    def __get_new_transport_id(self) -> int:
        result = self.__transport_id_counter
        self.__transport_id_counter += 1
        return result
