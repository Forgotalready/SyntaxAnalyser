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
        {"A", {"!", "B", "!"}},
        {"B", {"T"}},
        {"B", {"T", "+", "B"}},
        {"T", {"M"}},
        {"T", {"M", "*", "T"}},
        {"M", {"a"}},
        {"M", {"b"}},
        {"M", {"(", "B", ")"}}
};

std::vector<std::string> T ={
     "!", "+", "*", "(", ")", "a", "b"
};

State state;

bool is_terminal(const std::string& symbol){
    auto symbol_iterator = std::find(T.begin(), T.end(), symbol);
    return symbol_iterator != T.end();
}

bool parse(std::vector<std::string>& exp, std::vector<int>& res){
    std::stack<std::string> L1;
    std::stack<std::string> L2;
    state = NORMAL;
    L2.emplace("A");
    int pos = 0;

    while(state != TERMINATE){
        if(state == NORMAL){
            if(L2.empty() && pos == exp.size()){
                state = TERMINATE;
                res.clear();
                while(!L1.empty()){
                    std::string elem = L1.top();
                    if(!is_terminal(elem)) {
                        int ruleNum = elem[elem.size() - 1] - '0';
                        res.push_back(ruleNum);
                    }
                    L1.pop();
                }
                return true;
            }
            if(L2.empty() && pos != exp.size()){
                state = BACKTRACK;
                continue;
            }

            std::string top = L2.top();

            if(is_terminal(top)){
                if(top == exp[pos]){
                    L2.pop();
                    L1.push(top);
                    pos++;
                }else{
                    state = BACKTRACK;
                }
            }else{
                bool matched = false;
                for(size_t i = 0;i < Grammar.size();i++){
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
                if(!matched) state = BACKTRACK;

            }
        }else if(state == BACKTRACK){
            if(L1.empty()){
                state = TERMINATE;
                return false;
            }

            std::string top = L1.top();

            if(is_terminal(top)){
                L1.pop();
                pos--;
                L2.push(top);
            }
            else{
                int ruleIndex = top[top.size() - 1] - '0';
                std::string nonTerminal = top.substr(0, top.size() - 1);
                L1.pop();
                int lenPrevGram = Grammar[ruleIndex - 1].right_part.size();
                for(int j = 0;j < lenPrevGram;j++){
                    L2.pop();
                }
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
                if(!alternativeFound) L2.push(nonTerminal);
            }
        }
    }
    return false;
}

void parse(std::string& exp){
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
        parse(exp);
    }
}
