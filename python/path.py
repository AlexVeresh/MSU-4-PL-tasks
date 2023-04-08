from graph import Cruise


class Path:
    def __init__(self, cruise_ids=None) -> None:
        if not (cruise_ids is None):
            self.__cruise_ids = cruise_ids

    def total_length(self) -> int: return len(self.__cruise_ids)

    def __add__(self, obj: Cruise):
        new_ids = self.__cruise_ids.copy()
        new_ids.append(obj.id)
        path = Path(new_ids)
        return path

    def __getitem__(self, i): return self.__cruise_ids[i]

    def __str__(self) -> str: return str(self.__cruise_ids)

    __cruise_ids: list[int] = []
