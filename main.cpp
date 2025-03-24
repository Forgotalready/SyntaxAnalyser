#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <stack>
#include "Rule/Rule.h"

enum State : uint8_t{
    NORMAL,
    BACKTRACK,
    TERMINATE
};

std::vector<Rule> Grammar ={
        {"A", {"!", "B", "!"}}, //1
        {"B", {"T"}},//2
        {"B", {"T", "+", "B"}},//3
        {"T", {"M"}},//4
        {"T", {"M", "*", "T"}},//5
        {"M", {"a"}},//6
        {"M", {"b"}},//7
        {"M", {"(", "B", ")"}}//8
};

std::vector<std::string> T ={
     "!", "+", "*", "(", ")", "a", "b"
};

bool is_terminal(const std::string& symbol){
    auto symbol_iterator = std::find(T.begin(), T.end(), symbol);
    return symbol_iterator != T.end();
}

void form_result_array(std::stack<std::string> &L1, std::vector<int> &res) {
    res.clear();
    while(!L1.empty()){
        std::string elem = L1.top();
        if(!is_terminal(elem)) {
            int ruleNum = elem[elem.size() - 1] - '0';
            res.push_back(ruleNum);
        }
        L1.pop();
    }
}

int move_to_L2(std::stack<std::string> &L1, std::stack<std::string> &L2, int pos, const std::string &top) {
    L1.pop();
    pos--;
    L2.push(top);
    return pos;
}

bool find_next_alternative(std::stack<std::string> &L1, std::stack<std::string> &L2, int ruleIndex,
                           const std::string &nonTerminal, State &state) {
    bool alternativeFound = false;
    for (size_t i = ruleIndex; i < Grammar.size(); i++) {
        if (Grammar[i].left_part == nonTerminal) {
            L1.push(nonTerminal + std::to_string(i + 1));

            for (int j = Grammar[i].right_part.size() - 1; j >= 0; j--) {
                L2.push(Grammar[i].right_part[j]);
            }
            alternativeFound = true;
            state = NORMAL;
            break;
        }
    }
    return alternativeFound;
}

void delete_previous_alternative(std::stack<std::string> &L2, int ruleIndex) {
    int lenPrevGram = Grammar[ruleIndex - 1].right_part.size();
    for(int j = 0;j < lenPrevGram;j++){
        L2.pop();
    }
}

bool find_first_alternative(std::stack<std::string> &L1, std::stack<std::string> &L2, const std::string &top) {
    bool matched;
    for(size_t i = 0; i < Grammar.size(); i++){
        if(Grammar[i].left_part == top){
            L2.pop();
            L1.push(top + std::to_string(i + 1));
            for (int j = Grammar[i].right_part.size() - 1; j >= 0; j--) {
                L2.push(Grammar[i].right_part[j]);
            }
            matched = true;
            break;
        }
    }
    return matched;
}

int move_to_L1(std::stack<std::string> &L1, std::stack<std::string> &L2, int pos, const std::string &top) {
    L2.pop();
    L1.push(top);
    pos++;
    return pos;
}

bool check_stop_condition(std::stack<std::string>& L2, std::vector<std::string>& exp, int pos){
    return (L2.empty() && pos == exp.size());
}

bool parse(std::vector<std::string>& exp, std::vector<int>& res){
    std::stack<std::string> L1;
    std::stack<std::string> L2;
    State state = NORMAL;

    L2.emplace("A");
    int pos = 0;

    while(state != TERMINATE){
        if(state == NORMAL){
            /*
             * В нормальном состоянии:
             * 1) Проверить условие остановки алгоритма (шаг 3)
             * 2) Проверить не конец ли строки (шаг 4)
             * 3) Взять элемент с верхушки L2
             * 4) Провести анализ (если терминальный (шаг 2 или шаг 4) сравнить с элементом строки, если не терминальный расписать (шаг 1))
             * */
            if(check_stop_condition(L2, exp, pos)){
                state = TERMINATE;
                form_result_array(L1, res);
                return true;
            }
            if(L2.empty() && pos < exp.size()){
                state = BACKTRACK;
                continue;
            }
            if (!L2.empty() && pos == exp.size()) {
                state = BACKTRACK;
                continue;
            }

            std::string top = L2.top();

            if(is_terminal(top)){
                if(top == exp[pos]){
                    pos = move_to_L1(L1, L2, pos, top);
                }else{
                    state = BACKTRACK;
                }
            }
            else{
                bool matched = find_first_alternative(L1, L2, top);
                if(!matched) state = BACKTRACK;
            }
        }
        else if(state == BACKTRACK){
            /*
             * В состоянии возврата:
             * 1) Проверить условие остановки (шаг 6б)
             * 2) Взять элемент с верхушки L1
             * 3) Вернуть терминальный (шаг 5) или попробовать применить другую альтернативу для не терминального (шаг 6)
             * */
            if(L1.empty()){
                state = TERMINATE;
                return false;
            }

            std::string top = L1.top();

            if(is_terminal(top)){
                pos = move_to_L2(L1, L2, pos, top);
            }
            else{
                int ruleIndex = top[top.size() - 1] - '0';
                std::string nonTerminal = top.substr(0, top.size() - 1);
                L1.pop();
                delete_previous_alternative(L2, ruleIndex);
                bool alternativeFound = find_next_alternative(L1, L2, ruleIndex, nonTerminal, state);
                if(!alternativeFound) L2.push(nonTerminal);
            }
        }
    }
    return false;
}

void analiz(std::string& exp){
    std::vector<std::string> parsed_exp;
    for(auto x : exp){
        parsed_exp.emplace_back(1, x);
    }
    std::vector<int> res;
    std::cout << parse(parsed_exp, res) << "\n";

    if(res.empty()){
        std::cout << "Expression not include\n";
        return;
    }

    std::reverse(res.begin(), res.end());
    for(auto x : res){
        std::cout << x << " ";
    }
    std::cout << "\n";
}

int main() {
    std::string exp;
    while(true){
        std::cin >> exp;
        analiz(exp);
    }
}
