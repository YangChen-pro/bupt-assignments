class LL1Parser:
    def __init__(self):
        self.first = {}
        self.follow = {}
        self.predictions_table = {}

    # 计算first集
    def compute_first(self, grammar):
        for non_terminal in grammar.non_terminals:  # 初始化
            self.first[non_terminal] = set()
        for terminal in grammar.terminals:
            self.first[terminal] = set()

        for non_terminal in grammar.non_terminals:
            self.calculate_first(grammar, non_terminal)

        for non_terminal in grammar.non_terminals:  # 输出first集
            print("First({}) = {}".format(non_terminal, self.first[non_terminal]))
        print()

    # 计算单个非终结符的first集
    def calculate_first(self, grammar, non_terminal):
        if len(grammar.productions[non_terminal]) == 0:
            return

        for terminal in grammar.terminals:
            if terminal != "ε":
                self.first[terminal].add(terminal)

        for production in grammar.productions[non_terminal]:
            if production == "num":
                first_symbol = "num"
            elif len(production) > 1 and production[1] == "'":
                first_symbol = production[0:2]
            else:
                first_symbol = production[0]

            if first_symbol in grammar.terminals:
                self.first[non_terminal].add(first_symbol)
            elif first_symbol in grammar.non_terminals:
                self.calculate_first(grammar, first_symbol)
                self.first[non_terminal].update(self.first[first_symbol])

            if (
                first_symbol in grammar.non_terminals
                and "ε" in grammar.productions[first_symbol]
            ):
                if len(production) > 1:
                    if first_symbol == production[0]:
                        second_symbol = production[1]
                    elif len(production) > 2 and first_symbol == production[0:2]:
                        second_symbol = production[2]
                        if len(production) > 3 and production[3] == "'":
                            second_symbol += "'"
                    else:  # 右侧只有一个ε
                        continue
                    self.calculate_first(grammar, second_symbol)
                    self.first[non_terminal].update(self.first[second_symbol])

    # 计算follow集
    def compute_follow(self, grammar):
        start_symbol = grammar.start_symbol

        for non_terminal in grammar.non_terminals:  # 初始化
            self.follow[non_terminal] = set()

        self.follow[start_symbol].add("$")

        while True:
            changed = False

            for non_terminal in grammar.non_terminals:
                for production in grammar.productions[non_terminal]:
                    symbols = production

                    for i in range(len(symbols)):
                        symbol = symbols[i]
                        if i != len(symbols) - 1 and symbols[i + 1] == "'":
                            symbol += "'"
                            i += 1

                        if symbol in grammar.non_terminals:  # 当前符号是非终结符
                            if i == len(symbols) - 1:  # 最后一个符号
                                original_set = self.follow[symbol].copy()
                                self.follow[symbol].update(
                                    self.follow[non_terminal]
                                )  # follow[symbol] = follow[symbol] + follow[non_terminal]
                                changed = changed or (
                                    original_set != self.follow[symbol]
                                )

                            else:  # 不是最后一个符号
                                next_symbol = symbols[i + 1]
                                if i + 1 != len(symbols) - 1 and symbols[i + 2] == "'":
                                    next_symbol += "'"

                                if next_symbol in grammar.terminals:  # 下一个符号是终结符,直接加入
                                    if next_symbol not in self.follow[symbol]:
                                        original_set = self.follow[symbol].copy()
                                        self.follow[symbol].add(next_symbol)
                                        changed = changed or (
                                            original_set != self.follow[symbol]
                                        )

                                else:  # 下一个符号是非终结符
                                    if (
                                        "ε" in self.first[next_symbol]
                                    ):  # next_symbol的first集中有ε
                                        original_set = self.follow[symbol].copy()
                                        self.follow[symbol].update(
                                            self.follow[non_terminal]
                                        )  # follow[symbol] = follow[symbol] + follow[non_terminal]
                                        changed = changed or (
                                            original_set != self.follow[symbol]
                                        )

                                    original_set = self.follow[symbol].copy()
                                    self.follow[symbol].update(
                                        self.first[next_symbol] - {"ε"}
                                    )  # follow[symbol] = follow[symbol] + first[next_symbol] - {ε}
                                    changed = changed or (
                                        original_set != self.follow[symbol]
                                    )

            if not changed:
                break

        for non_terminal in grammar.non_terminals:  # 输出follow集
            print("Follow({}) = {}".format(non_terminal, self.follow[non_terminal]))
        print()

    # 教材算法4.2
    def compute_predictions_table(self, grammar):
        for production in grammar.productions:
            left_element = production
            for right_list in grammar.productions[production]:
                if right_list[0] in grammar.non_terminals:  # 右侧第一个符号是非终结符
                    for a in self.first[right_list[0]]:  # 把A->a加入M[A,a]
                        self.predictions_table[left_element, a] = right_list
                    if "ε" in self.first[right_list[0]]:
                        for b in self.follow[left_element]:  # 把A->a加入M[A,b]
                            self.predictions_table[left_element, b] = right_list
                else:  # 右侧第一个符号是终结符
                    if right_list == "num":
                        self.predictions_table[left_element, "num"] = right_list
                    else:
                        self.predictions_table[left_element, right_list[0]] = right_list
                    if "ε" == right_list[0]:  # 把A->a加入M[A,a]
                        for b in self.follow[left_element]:  # 把A->a加入M[A,b]
                            self.predictions_table[left_element, b] = right_list

        # 错误处理加入分析表
        for non_terminal in grammar.non_terminals:
            for symbol in self.follow[non_terminal]:
                if (non_terminal, symbol) not in self.predictions_table:
                    self.predictions_table[non_terminal, symbol] = "synch"

        # 输出预测分析表
        for terminal in grammar.terminals:
            if terminal == "ε":
                terminal = "$"
            print("\t{:10}".format(terminal), end="")
        print()
        for non_terminal in grammar.non_terminals:
            print(non_terminal, end="")
            for i in grammar.terminals:
                if i == "ε":
                    i = "$"
                if (non_terminal, i) in self.predictions_table:
                    print(
                        "\t{:10}".format(
                            non_terminal
                            + " -> "
                            + self.predictions_table[non_terminal, i]
                        ),
                        end="",
                    )
                else:
                    print("\t{:10}".format(" "), end="")
            print()
        print()

    # 检查是否是LL1文法
    def is_LL1(self, grammar):
        for non_terminal, production in grammar.productions.items():
            if len(production) > 1:  # 一个非终结符对应多个产生式
                for p1 in production:  # First集合不相交
                    for p2 in production:
                        if p1 != p2:
                            head1 = p1[0]
                            head2 = p2[0]
                            if head1 == "n":
                                head1 = "num"
                            if head2 == "n":
                                head2 = "num"
                            if (
                                len(self.first[head1].intersection(self.first[head2]))
                                > 0
                            ):
                                print(
                                    "First集合相交: First{} = {} and First{} = {}".format(
                                        head1,
                                        self.first[head1],
                                        head2,
                                        self.first[head2],
                                    )
                                )
                                return False
                epsilon_index = []
                for index in range(len(production)):
                    head = production[index][0]
                    if head == "n":
                        head = "num"
                    if "ε" in self.first[head]:
                        epsilon_index.append(index)
                if len(epsilon_index) != 0:  # First含ε，其他First集合和左部符号Follow集合不相交
                    for index in range(len(production)):
                        if index not in epsilon_index:
                            head = production[index][0]
                            if head == "n":
                                head = "num"
                            if (
                                len(
                                    self.first[head].intersection(
                                        self.follow[non_terminal]
                                    )
                                )
                                > 0
                            ):
                                print(
                                    "First集合和Follow集合相交: First{} = {} and Follow{} = {}".format(
                                        head,
                                        self.first[head],
                                        non_terminal,
                                        self.follow[non_terminal],
                                    )
                                )
                                return False
        return True

    # 将生成式反转
    def pre_reversed(self, production):
        tmp = production[::-1]
        result = ""
        i = 0
        while i < len(tmp):
            if tmp[i] == "'":
                result += tmp[i + 1] + "'"
                i += 2
            elif tmp[i] == "m":
                result += "num"
                i += 3
            else:
                result += tmp[i]
                i += 1
        return result

    # 教材算法4.1
    def parse(self, grammar, input_string):
        print("{:30}".format("Stack"), "\t{:30}".format("Input"), "\tOutput")
        stack = ["$", grammar.start_symbol]
        ptr = 0
        error_count = 0
        while True:
            print(
                "{:30}".format("".join(stack)),
                "\t{:30}".format(input_string[ptr:] + "$"),
                end="",
            )

            if ptr == len(input_string):  # 输入串已经读完
                now_string = "$"
            elif input_string[ptr].isdigit():  # 输入串中的符号是数字
                now_string = "num"
            else:
                now_string = input_string[ptr]

            if ptr == len(input_string) and stack[-1] == "$":  # 分析完毕，退出
                print("\t\n\033[93mComplete!\033[0m", end="")
                if error_count == 0:
                    print("\t\033[92mNo error\033[0m\n")
                else:
                    print("\t\033[91m{} error(s)\033[0m\n".format(error_count))
                break
            element = stack[-1]  # 栈顶元素
            if element in grammar.terminals:  # 栈顶元素是终结符
                if (element == "num" and now_string.isdigit()) or (
                    element == now_string
                ):  # 栈顶元素和输入串中的符号相同
                    stack.pop()
                    ptr += 1
                    print("\t")
                else:  # 栈顶元素和输入串中的符号不同
                    print(
                        "\t\033[91mError\033[0m: {} != {}".format(element, now_string)
                    )
                    error_count += 1
                    stack.pop()
            else:
                if (element, now_string) in self.predictions_table:
                    stack.pop()
                    production = self.predictions_table[element, now_string]
                    if production == "synch":  # 预测分析表中对应的产生式是synch
                        print(
                            "\t\033[91mError\033[0m: predictions_table[{}, {}] is synch".format(
                                element, now_string
                            )
                        )
                        error_count += 1
                    else:  # 预测分析表中有对应的产生式
                        print("\t{} -> {}".format(element, production))
                        if production != "ε":
                            re_production = self.pre_reversed(production)  # 反转产生式
                            i = 0
                            while i < len(re_production):  # 将产生式反序入栈
                                if (
                                    i < len(re_production) - 1
                                    and re_production[i + 1] == "'"
                                ):
                                    stack.append(re_production[i : i + 2])
                                    i += 2
                                elif re_production[i : i + 3] == "num":
                                    stack.append("num")
                                    i += 3
                                else:
                                    stack.append(re_production[i])
                                    i += 1
                else:  # 预测分析表中没有对应的产生式
                    print(
                        "\t\033[91mError\033[0m: predictions_table[{}, {}] is empty".format(
                            element, now_string
                        )
                    )
                    error_count += 1
                    ptr += 1


class Grammar:
    def __init__(self):
        self.non_terminals = set()
        self.terminals = set()
        self.productions = {}
        self.start_symbol = None

    # 添加产生式，non_terminal是非终结符，production是产生式
    def add_production(self, non_terminal, production):
        if non_terminal not in self.productions:
            self.productions[non_terminal] = []

        self.productions[non_terminal].append(production)
        self.non_terminals.add(non_terminal)

        if production == "num":
            self.terminals.add("num")
        else:
            for symbol in production:
                if symbol.isalpha() and not symbol.islower():  # 是大写字母
                    self.non_terminals.add(symbol)
                else:
                    self.terminals.add(symbol)

    # 消除左递归
    def eliminate_left_recursion(self):
        non_terminals = list(self.non_terminals)

        for left_element in non_terminals:
            remove_list = []
            for right_list in self.productions[left_element]:
                if right_list[0] == left_element:
                    remove_list.append(right_list)
                    if left_element + "'" not in self.non_terminals:
                        self.non_terminals.add(left_element + "'")
                        self.productions[left_element + "'"] = []
            if remove_list:  # 如果remove_list不为空
                for right_str in remove_list:
                    self.productions[left_element].remove(right_str)
                    self.productions[left_element + "'"].append(
                        right_str[1:] + left_element + "'"
                    )
                self.productions[left_element + "'"].append("ε")
                self.terminals.add("ε")
                self.productions[left_element] = [
                    value + left_element + "'"
                    for value in self.productions[left_element]
                ]

    # 消除左公因子
    def eliminate_left_factor(self):
        new_non_terminals = self.non_terminals  # 新的非终结符
        new_productions = {}  # 新的产生式
        while True:
            tmp_productions = {}  # 用于存储新的产生式
            for left_element in self.productions:  # 遍历产生式
                right_list = self.productions[left_element]  # 右侧产生式列表
                right_head = []  # 右侧产生式的首符号列表
                for right_str in right_list:
                    right_head.append(right_str[0])
                for i in range(len(right_head)):  # 遍历右侧产生式的首符号
                    for j in range(i + 1, len(right_head)):  # 遍历右侧产生式的首符号
                        if (
                            right_head[i] in self.non_terminals
                            and right_head[j] in self.non_terminals
                        ):  # 都是非终结符
                            continue
                        if right_head[i] == right_head[j]:  # 首符号相同,消除左公因子
                            new_non_terminals.add(left_element + "'")
                            if left_element + "'" not in tmp_productions:
                                tmp_productions[left_element + "'"] = set()
                            self.productions[left_element].remove(right_list[i])
                            self.productions[left_element].remove(right_list[j])
                            tmp_productions[left_element + "'"].add(right_list[i][1:])
                            tmp_productions[left_element + "'"].add(right_list[j][1:])
                            self.productions[left_element].add(
                                right_head[i] + left_element + "'"
                            )
            new_productions.update(self.productions)  # 更新产生式
            if tmp_productions == {}:  # 如果没有新的产生式，退出循环
                break
        self.productions = new_productions.copy()  # 更新产生式

    # 输出文法
    def print_grammar(self):
        for non_terminal in self.productions:
            print(non_terminal + " -> " + " | ".join(self.productions[non_terminal]))
        print()


if __name__ == "__main__":
    grammar = Grammar()
    grammar.add_production("E", "E+T")
    grammar.add_production("E", "E-T")
    grammar.add_production("E", "T")
    grammar.add_production("T", "T*F")
    grammar.add_production("T", "T/F")
    grammar.add_production("T", "F")
    grammar.add_production("F", "(E)")
    grammar.add_production("F", "num")
    grammar.start_symbol = "E"

    grammar.print_grammar()

    print("Non-terminals:", grammar.non_terminals)
    print("Terminals:", grammar.terminals)
    print("Start symbol:", grammar.start_symbol)
    print("Productions:", grammar.productions, "\n")

    print("消除左递归和消除左公因子后")
    grammar.eliminate_left_recursion()
    grammar.eliminate_left_factor()

    grammar.print_grammar()

    print("Non-terminals:", grammar.non_terminals)
    print("Terminals:", grammar.terminals)
    print("Start symbol:", grammar.start_symbol)
    print("Productions:", grammar.productions, "\n")

    ll1_parser = LL1Parser()
    ll1_parser.compute_first(grammar)
    ll1_parser.compute_follow(grammar)
    if ll1_parser.is_LL1(grammar):
        print("经检查，该文法是LL1文法\n")
        ll1_parser.compute_predictions_table(grammar)
        input_string = ["1+2", "1+2*(3-(4/0))", "1+2*/(3-4/0))"]
        for s in input_string:
            ll1_parser.parse(grammar, s)
    else:
        print("经检查，该文法不是LL1文法\n")
