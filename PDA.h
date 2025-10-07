//
// Created by PPetre on 7/10/2025.
//

#ifndef PDA_H
#define PDA_H
#include <stack>
#include <string>
#include <unordered_map>

#include "cfg.h"


struct Transition {
    std::string input;
    std::string stackTop;
    std::string toState;
    std::vector<std::string> replacements;
};


class PDA {
public:
    explicit PDA(const std::string& file);

    [[nodiscard]] CFG toCFG();

private:
    std::unordered_set<std::string> states;
    std::unordered_set<std::string> alphabet;
    std::unordered_set<std::string> stackAlphabet;
    std::unordered_map<std::string, std::vector<const Transition>> transitions;
    std::string startState;
    std::stack<std::string> stack;
};



#endif //PDA_H
