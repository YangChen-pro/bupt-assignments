from collections import OrderedDict
from ordered_set import OrderedSet


class DFA:
    def __init__(self):
        self.states = {0: OrderedSet()}  # 所有状态的集合，key为状态编号，value为状态
        self.transitions = {}  # 转移边集，二维字典，key为状态编号，value为字典，字典的key为下一个状态编号，value为转移符号
        self.count_state = 0  # 状态个数

    # 计算first集，是计算LR1自动机闭包的一部分
    def get_first(self, grammar, s):
        if s[0] in grammar.terminals or s == "$":
            return s[0]
        right_list = grammar.productions[s[0]]
        first_str = None
        for right_str in right_list:
            if right_str[0] in grammar.terminals:
                first_str = right_str[0]
            elif right_str == "num":
                first_str = "num"
            elif right_str[0] == s[0]:
                continue
            else:
                first_str = self.get_first(grammar, right_str[0])
        return first_str

    # 计算LR1自动机的闭包
    def get_closure(self, grammar, state):
        closure = state
        for item in state:
            left = item[0][0]
            right_str = item[0][1]
            for ii in range(len(item[1])):
                look_ahead = str(item[1][ii])
                for i in range(len(right_str)):
                    if right_str[i] == "·" and i + 1 < len(right_str):
                        if right_str[i + 1] in grammar.non_terminals:  # ·后面是非终结符
                            for production in grammar.productions[right_str[i + 1]]:
                                if i + 1 == len(right_str) - 1:  # ·在倒数第二个位置
                                    str_ahead = look_ahead
                                else:
                                    str_ahead = right_str[i + 2 :] + look_ahead
                                tmp = (
                                    (right_str[i + 1], "·" + production),
                                    tuple(self.get_first(grammar, str_ahead)),
                                )
                                if tmp not in closure:
                                    closure.add(tmp)
        return closure

    # 对于相同的左部产生式，合并look_ahead
    def merge(self, state):
        lst = state
        merged_dict = {}

        for item in lst:
            key = item[0]
            value = list(item[1])

            if key in merged_dict:
                for i in range(len(value)):
                    if value[i] not in merged_dict[key]:
                        merged_dict[key].append(value[i])
            else:
                merged_dict[key] = value

        merged_list = [(key, value) for key, value in merged_dict.items()]
        new_states = OrderedSet()
        for i in range(len(merged_list)):
            tmp = (
                (merged_list[i][0][0], merged_list[i][0][1]),
                tuple(merged_list[i][1]),
            )
            new_states.add(tmp)
        return new_states

    # 打印状态number
    def print_state(self, number):
        for item in self.states[number]:
            print(f"{item[0][0]} -> {item[0][1]} , {'|'.join(list(item[1]))}")

    # 添加边，参数为状态编号，下一个状态编号，转移符号
    def add_edge(self, start_vertex, end_vertex, weight):
        if start_vertex not in self.transitions:
            self.transitions[start_vertex] = {}
        self.transitions[start_vertex][end_vertex] = weight

    # 打印整个自动机
    def show_dfa(self):
        for num in range(self.count_state):
            print("状态I{}:".format(num))
            self.print_state(num)
            if num in self.transitions:
                for neighbor in self.transitions[num]:
                    print(
                        "--- {} ---> 状态I{}".format(
                            self.transitions[num][neighbor], neighbor
                        )
                    )
            print()


class LR1Parser:
    def __init__(self):
        self.DFA = DFA()  # LR1自动机
        self.analysis_table = {}  # 分析表

    # 初始化I0状态
    def compute_first(self, grammar):
        right_str = "·" + grammar.production_count[0][grammar.start_symbol]
        tmp = ((grammar.start_symbol, right_str), tuple("$"))
        self.DFA.states[0].add(tmp)
        self.DFA.count_state += 1
        self.DFA.states[0] = self.DFA.get_closure(grammar, self.DFA.states[0])
        self.DFA.states[0] = self.DFA.merge(self.DFA.states[0])

    # 计算LR1自动机
    def compute_lr1_automaton(self, grammar, number=0):
        if number == 0:  # 初始化I0状态
            self.compute_first(grammar)
        if number == self.DFA.count_state:  # 计算完毕
            self.DFA.show_dfa()
            return
        state = self.DFA.states[number]
        classified_dict = OrderedDict()  # 按照·后面的字符分类
        for item in state:
            dot_index = item[0][1].find("·")
            if dot_index != -1 and dot_index + 1 < len(item[0][1]):
                key = item[0][1][dot_index + 1]
                if key == "n":
                    key = "num"
                if key not in classified_dict:
                    classified_dict[key] = OrderedSet()
                classified_dict[key].add(item)

        for k, v in classified_dict.items():
            tmp_state = OrderedSet()
            for tpl in v:
                left = tpl[0][0]
                right_str = tpl[0][1]
                dot_index = right_str.find("·")
                new_right_str = ""
                if right_str[dot_index + 1 :] == "num":
                    new_right_str = "num·"
                elif dot_index != -1 and dot_index + 1 < len(right_str):
                    new_right_str = (
                        right_str[:dot_index]
                        + right_str[dot_index + 1]
                        + "·"
                        + right_str[dot_index + 2 :]
                    )
                tmp = ((left, new_right_str), tpl[1])
                tmp_state.add(tmp)
            tmp_state = self.DFA.get_closure(grammar, tmp_state)
            tmp_state = self.DFA.merge(tmp_state)
            if tmp_state not in self.DFA.states.values():
                self.DFA.states[self.DFA.count_state] = tmp_state
                self.DFA.add_edge(number, self.DFA.count_state, k)
                self.DFA.count_state += 1
            else:
                for key, value in self.DFA.states.items():
                    if value == tmp_state:
                        self.DFA.add_edge(number, key, k)

        self.compute_lr1_automaton(grammar, number + 1)

    # 计算分析表，教材算法4.9
    def compute_analysis_table(self, grammar):
        # 初始化分析表
        for num in range(self.DFA.count_state):
            self.analysis_table[num] = {}
            for terminal in grammar.terminals:
                self.analysis_table[num][terminal] = None
            self.analysis_table[num]["$"] = None
            for non_terminal in grammar.non_terminals:
                self.analysis_table[num][non_terminal] = None

        for num in range(self.DFA.count_state):
            if num in self.DFA.transitions:  # 有出边
                for neighbor in self.DFA.transitions[num]:
                    if (
                        self.DFA.transitions[num][neighbor] in grammar.non_terminals
                    ):  # 边是非终结符，goto下一个状态
                        self.analysis_table[num][
                            self.DFA.transitions[num][neighbor]
                        ] = str(neighbor)
                    else:  # 边是终结符，action为Shift，并转移到下一个状态
                        self.analysis_table[num][
                            self.DFA.transitions[num][neighbor]
                        ] = "S" + str(neighbor)
            for item in self.DFA.states[num]:
                if item[0][1][-1] == "·":  # 产生式是规约项
                    if item[0][0] == grammar.start_symbol:  # 产生式是开始符号
                        self.analysis_table[num]["$"] = "ACC"
                    else:  # 产生式不是开始符号
                        for look_ahead in item[1]:
                            self.analysis_table[num][
                                look_ahead
                            ] = "R" + grammar.find_production(
                                item[0][0], item[0][1][0:-1]
                            )
        print("分析表：")
        print(
            "状态\t{}{}{}{}".format(
                "\t" * (int)(len(grammar.terminals) + 1),
                "action",
                "\t" * (int)(len(grammar.terminals) + 1),
                "GOTO",
            )
        )
        print("\t", end="")
        for terminal in grammar.terminals:
            print(terminal + "\t\t", end="")
        print("$\t\t", end="")
        for non_terminal in grammar.non_terminals:
            if non_terminal != grammar.start_symbol:
                print(non_terminal + "\t\t", end="")
        print()
        for num in range(self.DFA.count_state):
            print("{}\t".format(num), end="")
            for terminal in grammar.terminals:
                if self.analysis_table[num][terminal] is None:
                    print("\t\t", end="")
                else:
                    print(self.analysis_table[num][terminal] + "\t\t", end="")
            if self.analysis_table[num]["$"] is None:
                print("\t\t", end="")
            else:
                print(self.analysis_table[num]["$"] + "\t\t", end="")
            for non_terminal in grammar.non_terminals:
                if non_terminal != grammar.start_symbol:
                    if self.analysis_table[num][non_terminal] is None:
                        print("\t\t", end="")
                    else:
                        print(self.analysis_table[num][non_terminal] + "\t\t", end="")
            print()
        print()

    # 分析输入串，教材算法4.3
    def parse(self, grammar, input_string):
        print("{:50}".format("Stack"), "\t{:30}".format("Input"), "\tAction")
        stack = [[0, "~"]]
        ptr = 0
        while True:
            # print("{:30}".format("Stack"), "\t{:30}".format("Input") + "$")
            pit_state = " ".join(str(stack[i][0]) for i in range(len(stack)))
            pit_symbol = " ".join(
                "{:>2}".format(str(stack[i][1]))
                if stack[i][0] > 9
                else "{:>1}".format(str(stack[i][1]))
                for i in range(len(stack))
            )
            print("State:  {:42}".format(pit_state))
            print(
                "Symbol: {:42}".format(pit_symbol),
                "\t{:30}".format(input_string[ptr:] + "$"),
                end="",
            )
            now_string = ""
            if ptr == len(input_string):  # 输入串已经读完
                now_string = "$"
            elif input_string[ptr].isdigit():  # 输入串中的符号是数字
                now_string = "num"
            else:
                now_string = input_string[ptr]
            if self.analysis_table[stack[-1][0]][now_string] is None:
                print("\t\033[91m error(s) \033[0m")
                break
            elif self.analysis_table[stack[-1][0]][now_string] == "ACC":
                print("\t\033[92mAcc!\033[0m", end="")
                break
            elif self.analysis_table[stack[-1][0]][now_string][0] == "S":  # 移进Shift
                next_num = int(self.analysis_table[stack[-1][0]][now_string][1:])
                print("\tShift {}".format(next_num))
                stack.append([next_num, input_string[ptr]])
                ptr += 1
            elif self.analysis_table[stack[-1][0]][now_string][0] == "R":  # 规约Reduce
                production_num = int(
                    self.analysis_table[stack[-1][0]][now_string][1:]
                )  # 记录产生式编号，将产生式右部弹出，左部压入
                left = list(grammar.production_count[production_num].keys())[0]
                right = list(grammar.production_count[production_num].values())[0]
                if right == "num":
                    pop_num = 1
                else:
                    pop_num = len(right)
                print("\treduce by {} -> {}".format(left, right))
                for i in range(pop_num):
                    stack.pop()
                stack.append([int(self.analysis_table[stack[-1][0]][left]), left])
            print()


class Grammar:
    def __init__(self):
        self.non_terminals = OrderedSet()  # 非终结符
        self.terminals = OrderedSet()  # 终结符
        self.productions = OrderedDict()  # 产生式
        self.start_symbol = None  # 开始符号
        self.production_count = OrderedDict()  # 产生式编号
        self.count = 0  # 产生式个数

    # 添加产生式，参数为左部和右部
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

    # 拓广文法并打印
    def expand_grammar(self):
        new_non_terminals = OrderedSet()
        new_productions = OrderedDict()
        new_start_symbol = self.start_symbol + "'"
        new_non_terminals.add(new_start_symbol)
        new_productions[new_start_symbol] = [self.start_symbol]
        for non_terminal in self.non_terminals:
            new_non_terminals.add(non_terminal)
            new_productions[non_terminal] = self.productions[non_terminal]
        self.non_terminals = new_non_terminals
        self.productions = new_productions
        self.start_symbol = new_start_symbol
        for non_terminal in self.non_terminals:
            right_list = self.productions[non_terminal]
            for right_str in right_list:
                key = {non_terminal: right_str}
                self.production_count[self.count] = key
                self.count += 1
        for i in range(self.count):
            for key in self.production_count[i]:
                print("{:2} -> ".format(key), end="")
                print("{:5}".format(self.production_count[i][key]), end="")
            print("{:10}".format(i))
        print()

    # 打印文法
    def print_grammar(self):
        for non_terminal in self.productions:
            print(non_terminal + " -> " + " | ".join(self.productions[non_terminal]))
        print()

    # 根据产生式左部和右部查找产生式编号
    def find_production(self, left, right):
        for i in range(self.count):
            if (
                left in self.production_count[i]
                and right in self.production_count[i].values()
            ):
                return str(i)
        return None


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
    print("原文法")
    grammar.print_grammar()

    print("Non-terminals:", grammar.non_terminals)
    print("Terminals:", grammar.terminals)
    print("Start symbol:", grammar.start_symbol)
    print("Productions:", grammar.productions, "\n")

    print("拓广文法后")
    grammar.expand_grammar()

    print("Non-terminals:", grammar.non_terminals)
    print("Terminals:", grammar.terminals)
    print("Start symbol:", grammar.start_symbol)
    print("Productions:", grammar.productions, "\n")

    lr1_parser = LR1Parser()
    print("计算LR1自动机：")
    lr1_parser.compute_lr1_automaton(grammar)
    # 计算分析表
    lr1_parser.compute_analysis_table(grammar)
    input_string = ["1+2", "1+2*(3-(4/0))", "1+2*/(3-4/0))"]
    for s in input_string:
        print("输入串：\033[93m{}\033[0m".format(s))
        lr1_parser.parse(grammar, s)
        print('\n')
