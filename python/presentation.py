import curses as cs
from graph import Graph

EXIT_APP = "EXIT_APP"


def ctrl(x): x & 0x1F


class UiController():
    def __init__(self, stdscr: 'cs._CursesWindow', graph: Graph) -> None:
        self.stdscr = stdscr
        self.graph = graph
        cs.noecho()
        cs.cbreak()
        # cs.halfdelay(5)
        cs.start_color()
        cs.init_pair(1, cs.COLOR_WHITE, cs.COLOR_BLACK)
        cs.init_pair(2, 10, cs.COLOR_BLACK)
        cs.init_pair(3, 13, cs.COLOR_BLACK)
        cs.init_pair(4, 15, cs.COLOR_BLACK)
        cs.init_pair(5, cs.COLOR_RED, cs.COLOR_BLACK)
        self.stdscr.keypad(True)

    def clear(self) -> None: self.stdscr.clear()

    def refresh(self) -> None: self.stdscr.refresh()

    def endwin(self) -> None: cs.endwin()

    def clear_n_lines(self, start, end) -> None:
        for i in range(start, end+1):
            self.stdscr.move(i, 0)
            self.stdscr.clrtoeol()

    def wait_for_enter(self, start) -> None:
        cs.curs_set(0)
        self.stdscr.attron(cs.color_pair(3))
        self.stdscr.addstr(start, 0, "Нажмите ENTER чтобы продолжить")
        self.stdscr.attroff(cs.color_pair(3))
        self.stdscr.move(start + 1, 0)
        c = self.stdscr.getch()
        while c != 10:
            c = self.stdscr.getch()

    def validate_number_input(self, input: str) -> int:
        if input.isdigit():
            return int(input)
        else:
            return -1

    def validate_city_input(self, input) -> int:
        for _, value in self.graph.cities.items():
            if value.title == input:
                return value.id

        return -1

    def validate_transport_input(self, input) -> int:
        for _, value in self.graph.transports.items():
            if value.title == input:
                return value.id

        return -1

    def handle_int_input(self) -> str:
        cs.echo()
        self.stdscr.attron(cs.color_pair(4))
        result = self.stdscr.getstr()
        self.stdscr.attroff(cs.color_pair(4))
        cs.noecho()
        return result

    def handle_string_input(self, row) -> str:
        result_str = []
        new_typed = []
        i = 0
        c = self.stdscr.get_wch()
        while c != '\n':
            if c != -1:
                if ord(c) == 127 or ord(c) == cs.KEY_BACKSPACE:
                    if i > 0:
                        result_str.pop()
                        i -= 1
                        self.stdscr.move(row, i)

                    else:
                        i = 0

                    self.stdscr.clrtoeol()

                elif c == ctrl(27):
                    return EXIT_APP

                else:
                    self.stdscr.attron(cs.color_pair(4))
                    self.stdscr.addch(c)
                    self.stdscr.attroff(cs.color_pair(4))
                    result_str.append(c)
                    new_typed.append(c)
                    i += 1

            c = self.stdscr.get_wch()

        return ''.join(result_str)

    def handle_mode_input(self) -> int:
        cs.curs_set(0)
        chosen_mode = 0
        modes = [
            " 1. Среди кратчайших по времени путей между двумя городами найти путь минимальной стоимости",
            " 2. Среди путей между двумя городами найти путь минимальной стоимости",
            " 3. Среди путей между двумя городами найти путь минимальный по числу посещенных городов",
            " 4. Найти множество городов (и минимальных по стоимости путей к ним), достижимых из города отправления\n     не более чем за обозначенную сумму денег",
            " 5. Найти множество городов (и минимальных по времени путей к ним), достижимых из города отправления\n     не более чем за обозначенное время",
            "Завершить программу"
        ]
        user_choice = -2
        is_proccessing = True
        self.stdscr.addstr(0, 0, "Добро пожаловать! Выберите режим работы")

        while is_proccessing:
            for i in range(len(modes)):
                if chosen_mode == i:
                    self.stdscr.attron(cs.A_REVERSE)

                if i == 4:
                    self.stdscr.addstr(i + 3, 0, modes[i])

                elif i == 5:
                    self.stdscr.attron(cs.color_pair(5))
                    self.stdscr.addstr(i + 5, 0, modes[i])
                    self.stdscr.attroff(cs.color_pair(5))

                else:
                    self.stdscr.addstr(i + 2, 0, modes[i])

                self.stdscr.attroff(cs.A_REVERSE)

            user_choice = self.stdscr.getch()

            if user_choice == cs.KEY_UP:
                if chosen_mode == 0:
                    chosen_mode = len(modes) - 1
                else:
                    chosen_mode -= 1

            elif user_choice == cs.KEY_DOWN:
                if chosen_mode == len(modes) - 1:
                    chosen_mode = 0
                else:
                    chosen_mode += 1

            elif user_choice == 10:
                is_proccessing = False
                if chosen_mode == 5:
                    return -1
                else:
                    return chosen_mode

        return -1

    def handle_finish_app(self) -> str:
        cs.curs_set(0)
        chosen_mode = 0
        modes = [" 1. Завершить программу",
                 " 2. Вернутся на страницу выбора режима работы"]
        user_choice = -2
        is_proccessing = True
        self.stdscr.addstr(0, 0, "Выберите режим работы")

        while is_proccessing:
            for i in range(len(modes)):
                if chosen_mode == i:
                    self.stdscr.attron(cs.A_REVERSE)

                self.stdscr.addstr(i + 2, 0, modes[i])
                self.stdscr.attroff(cs.A_REVERSE)

            user_choice = self.stdscr.getch()

            if user_choice == cs.KEY_UP:
                if chosen_mode == 0:
                    chosen_mode = len(modes) - 1
                else:
                    chosen_mode -= 1

            elif user_choice == cs.KEY_DOWN:
                if chosen_mode == len(modes) - 1:
                    chosen_mode = 0
                else:
                    chosen_mode += 1

            elif user_choice == 10:
                return chosen_mode

        return chosen_mode

    def handle_pair_of_cities_input(self) -> tuple[int]:
        cs.curs_set(1)
        self.stdscr.addstr(0, 0, "Введите город отправления")
        self.stdscr.move(1, 0)
        self.stdscr.refresh()

        city_from = ''
        city_from_id = -1
        city_from = self.handle_string_input(1)

        if city_from == EXIT_APP:
            return -1, -1

        while city_from_id == -1:
            city_from_id = self.validate_city_input(city_from)

            if city_from_id == -1:
                self.clear_n_lines(3, 4)
                self.stdscr.addstr(
                    3, 0, "Введите корректный город отправления")
                self.stdscr.move(4, 0)
                city_from = self.handle_string_input(4)

                if city_from == EXIT_APP:
                    return -1, -1

        self.clear_n_lines(0, 4)

        self.stdscr.attron(cs.color_pair(3))
        self.stdscr.addstr(0, 0, "######//~~~~~~//######")
        self.stdscr.attroff(cs.color_pair(3))
        self.stdscr.addstr(2, 0, "Ваш город отправления: ")
        self.stdscr.attron(cs.color_pair(2))
        self.stdscr.addstr(2, 23, city_from)
        self.stdscr.attroff(cs.color_pair(2))
        self.stdscr.attron(cs.color_pair(3))
        self.stdscr.addstr(4, 0, "######//~~~~~~//######")
        self.stdscr.attroff(cs.color_pair(3))

        self.stdscr.addstr(6, 0, "Введите город прибытия")
        self.stdscr.move(7, 0)
        self.stdscr.refresh()

        city_to = ''
        city_to_id = -1
        city_to = self.handle_string_input(7)

        if city_from == EXIT_APP:
            return -1, -1

        while city_to_id == -1:
            city_to_id = self.validate_city_input(city_to)

            if city_to_id == -1:
                self.clear_n_lines(9, 10)
                self.stdscr.addstr(9, 0, "Введите корректный город прибытия")
                self.stdscr.move(10, 0)
                city_to = self.handle_string_input(9)

                if city_to == EXIT_APP:
                    return -1, -1

        self.clear_n_lines(6, 10)

        self.stdscr.attron(cs.color_pair(3))
        self.stdscr.addstr(6, 0, "######//~~~~~~//######")
        self.stdscr.attroff(cs.color_pair(3))
        self.stdscr.addstr(8, 0, "Ваш город прибытия: ")
        self.stdscr.attron(cs.color_pair(2))
        self.stdscr.addstr(8, 20, city_to)
        self.stdscr.attroff(cs.color_pair(2))
        self.stdscr.attron(cs.color_pair(3))
        self.stdscr.addstr(10, 0, "######//~~~~~~//######")
        self.stdscr.attroff(cs.color_pair(3))

        self.wait_for_enter(12)

        return city_from_id, city_to_id

    def handle_single_city_input(self) -> int:
        cs.curs_set(1)
        self.stdscr.addstr(0, 0, "Введите город отправления")
        self.stdscr.move(1, 0)
        self.stdscr.refresh()

        city_from = ''
        city_from_id = -1
        city_from = self.handle_string_input(1)

        while city_from_id == -1:
            city_from_id = self.validate_city_input(city_from)

            if city_from_id == -1:
                self.clear_n_lines(3, 4)
                self.stdscr.addstr(
                    3, 0, "Введите корректный город отправления")
                self.stdscr.move(4, 0)
                city_from = self.handle_string_input(4)

                if city_from == EXIT_APP:
                    return -1

        self.clear_n_lines(0, 4)

        self.stdscr.attron(cs.color_pair(3))
        self.stdscr.addstr(0, 0, "######//~~~~~~//######")
        self.stdscr.attroff(cs.color_pair(3))
        self.stdscr.addstr(2, 0, "Ваш город отправления: ")
        self.stdscr.attron(cs.color_pair(2))
        self.stdscr.addstr(2, 23, city_from)
        self.stdscr.attroff(cs.color_pair(2))
        self.stdscr.attron(cs.color_pair(3))
        self.stdscr.addstr(4, 0, "######//~~~~~~//######")
        self.stdscr.attroff(cs.color_pair(3))

        self.wait_for_enter(6)

        return city_from_id

    def handle_cost_limit_input(self) -> int:
        cs.curs_set(1)
        self.stdscr.addstr(0, 0, "Введите ограничение по стоимости")
        self.stdscr.move(1, 0)
        self.stdscr.refresh()

        cost_limit_input = ''
        cost_limit = -1
        cost_limit_input = self.handle_int_input()

        while cost_limit == -1:
            cost_limit = self.validate_number_input(cost_limit_input)

            if cost_limit == -1:
                self.clear_n_lines(3, 4)
                self.stdscr.addstr(
                    3, 0, "Число отрицательно или введено некорректно, повторите попытку")
                self.stdscr.move(4, 0)
                cost_limit_input = self.handle_int_input()
                if cost_limit_input == EXIT_APP:
                    return -1

        self.clear_n_lines(0, 4)

        self.stdscr.attron(cs.color_pair(3))
        self.stdscr.addstr(0, 0, "######//~~~~~~//######")
        self.stdscr.attroff(cs.color_pair(3))
        self.stdscr.addstr(2, 0, "Ограничение по стоимости: ")
        self.stdscr.attron(cs.color_pair(2))
        self.stdscr.addstr(2, 26, cost_limit_input)
        self.stdscr.addstr(2, 27 + len(cost_limit_input), "рублей")
        self.stdscr.attroff(cs.color_pair(2))
        self.stdscr.attron(cs.color_pair(3))
        self.stdscr.addstr(4, 0, "######//~~~~~~//######")
        self.stdscr.attroff(cs.color_pair(3))

        self.wait_for_enter(6)

        return cost_limit

    def handle_time_limit_input(self) -> int:
        cs.curs_set(1)
        self.stdscr.addstr(0, 0, "Введите ограничение по времени")
        self.stdscr.move(1, 0)
        self.stdscr.refresh()

        time_limit_input = ''
        time_limit = -1
        time_limit_input = self.handle_int_input()

        while time_limit == -1:
            time_limit = self.validate_number_input(time_limit_input)

            if time_limit == -1:
                self.clear_n_lines(3, 4)
                self.stdscr.addstr(
                    3, 0, "Число отрицательно или введено некорректно, повторите попытку")
                self.stdscr.move(4, 0)
                time_limit_input = self.handle_int_input()
                if time_limit_input == EXIT_APP:
                    return -1

        self.clear_n_lines(0, 4)

        self.stdscr.attron(cs.color_pair(3))
        self.stdscr.addstr(0, 0, "######//~~~~~~//######")
        self.stdscr.attroff(cs.color_pair(3))
        self.stdscr.addstr(2, 0, "Ограничение по времени: ")
        self.stdscr.attron(cs.color_pair(2))
        self.stdscr.addstr(2, 24, time_limit_input)
        self.stdscr.addstr(2, 25 + len(time_limit_input), "у. е.")
        self.stdscr.attroff(cs.color_pair(2))
        self.stdscr.attron(cs.color_pair(3))
        self.stdscr.addstr(4, 0, "######//~~~~~~//######")
        self.stdscr.attroff(cs.color_pair(3))

        self.wait_for_enter(6)

        return time_limit

    def handle_restricted_transport_list_input(self) -> set[int]:
        cs.curs_set(1)
        self.stdscr.addstr(
            0, 0, "Введите список транспорта нежелательного для передвижения в таком формате (автобус/самолет/.../)")

        input_is_valid = False
        row_number = 0
        valid_transport_id_list = set()

        while input_is_valid != True:
            self.stdscr.move(row_number + 1, 0)
            self.stdscr.clrtoeol()
            self.stdscr.refresh()

            invalid_transport_title_list = set()
            valid_transport_id_list.clear()
            current_transport = ''

            transport_list_str = self.handle_string_input(row_number + 1)
            for c in transport_list_str:
                if c == '/':
                    valid_transport_id = self.validate_transport_input(
                        current_transport)

                    if valid_transport_id != -1:
                        valid_transport_id_list.add(valid_transport_id)

                    else:
                        invalid_transport_title_list.add(current_transport)

                    current_transport = ''

                else:
                    current_transport += c

            if len(invalid_transport_title_list) != 0:
                self.stdscr.move(4, 0)
                self.stdscr.clrtoeol()
                self.stdscr.move(6, 0)
                self.stdscr.clrtoeol()
                self.stdscr.addstr(
                    4, 0, "Транспорт введен некорректно или не найден")
                invalid_input = ''

                for item in invalid_transport_title_list:
                    invalid_input += item + " "

                self.stdscr.attron(cs.color_pair(4))
                self.stdscr.addstr(6, 0, invalid_input)
                self.stdscr.attroff(cs.color_pair(4))
                self.stdscr.addstr(8, 0, "Введите список снова")
                row_number = 8

            else:
                self.clear_n_lines(0, 9)
                if len(valid_transport_id_list) == 0:
                    self.stdscr.addstr(0, 0, "Ограничений на транспорт нет")
                    self.wait_for_enter(2)

                else:
                    self.stdscr.attron(cs.color_pair(3))
                    self.stdscr.addstr(0, 0, "######//~~~~~~//######")
                    self.stdscr.attroff(cs.color_pair(3))
                    self.stdscr.addstr(2, 0, "Следующий транспорт ограничен")
                    valid_input = ''

                    for transport_id in valid_transport_id_list:
                        valid_input += self.graph.get_transport(
                            transport_id).title + " "

                    self.stdscr.attron(cs.color_pair(5))
                    self.stdscr.addstr(4, 0, valid_input)
                    self.stdscr.attroff(cs.color_pair(5))
                    self.stdscr.attron(cs.color_pair(3))
                    self.stdscr.addstr(6, 0, "######//~~~~~~//######")
                    self.stdscr.attroff(cs.color_pair(3))

                    self.wait_for_enter(8)

                input_is_valid = True

        return valid_transport_id_list

    def print_result(self, title: str, result: str) -> None:
        self.stdscr.clear()
        self.stdscr.refresh()
        self.stdscr.addstr(0, 0, title)
        self.stdscr.attron(cs.color_pair(4))
        try:
            self.stdscr.addstr(2, 0, result)
        except cs.error:
            pass
        self.stdscr.attroff(cs.color_pair(4))
