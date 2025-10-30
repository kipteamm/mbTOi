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

    void print();
    // [[nodiscard]] bool accepts(const std::string& input);

    [[nodiscard]] CFG toCFG();

private:
    // std::vector<std::pair<const std::string&, const std::vector<std::string>&>> transition(const std::string& state, const std::string& input, const std::string& top);

    std::unordered_set<std::string> states;
    std::unordered_set<std::string> acceptingStates;
    std::unordered_set<std::string> alphabet;
    std::unordered_set<std::string> stackAlphabet;
    std::unordered_map<std::string, std::vector<Transition>> transitions;
    std::string startState;
    std::stack<std::string> stack;

    void generateRules(CFG& cfg, const std::string& from, const Transition& transition, std::vector<std::string> intermediates, const std::string& r_k, int depth);
};



#endif //PDA_H
