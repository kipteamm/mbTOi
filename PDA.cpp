//
// Created by PPetre on 7/10/2025.
//

#include <algorithm>
#include <iostream>
#include <fstream>

#include "PDA.h"
#include "json.hpp"


PDA::PDA(const std::string &file) {
    std::ifstream input(file);
    nlohmann::json data = nlohmann::json::parse(input);

    for (const auto& state: data["States"]) {
        this->states.insert(state.get<std::string>());
    }

    for (const auto& symbol: data["Alphabet"]) {
        this->alphabet.insert(symbol.get<std::string>());
    }

    for (const auto& symbol: data["StackAlphabet"]) {
        this->stackAlphabet.insert(symbol.get<std::string>());
    }

    for (const auto& _transition: data["Transitions"]) {
        std::vector<std::string> replacements = _transition["replacement"].get<std::vector<std::string>>();
        if (replacements.empty()) replacements.push_back("");

        const Transition transition{
            _transition["input"].get<std::string>(),
            _transition["stacktop"].get<std::string>(),
            _transition["to"].get<std::string>(),
            replacements,
        };

        this->transitions[_transition["from"].get<std::string>()].push_back(transition);
    }

    this->startState = data["StartState"].get<std::string>();
    this->stack.push(data["StartStack"].get<std::string>());
}


// bool PDA::accepts(const std::string &input) {
//     auto ids = transition(this->startState, &input[0], this->stack.top());
// }


// std::vector<std::pair<const std::string&, const std::vector<std::string>&>> PDA::transition(const std::string &state, const std::string& input, const std::string &top) {
//     std::vector<std::pair<const std::string&, const std::vector<std::string>&>> ids;
//
//
//     for (const Transition &transition: this->transitions[state]) {
//         if (transition.input != input) continue;
//         if (transition.stackTop != top) continue;
//         ids.push_back({transition.toState, transition.replacements});
//     }
//
//     return ids;
// }


void printSortedPDA(const std::unordered_set<std::string>& s) {
    std::vector<std::string> v(s.begin(), s.end());
    std::sort(v.begin(), v.end(), [](const std::string& a, const std::string& b) {
       return a < b;
    });

    for (int i = 0; i < v.size(); i++) {
        std::cout << v[i];
        if (i + 1 < v.size()) std::cout << ", ";
    }
}


void PDA::print() {
    std::cout << "Q  = {"; printSortedPDA(this->states); std::cout << "}\n";
    std::cout << "A  = {"; printSortedPDA(this->alphabet); std::cout << "}\n";
    std::cout << "S  = {"; printSortedPDA(this->stackAlphabet); std::cout << "}\n";

    std::cout << "P = [\n";
    for (const auto& [from, transitions]: this->transitions) {
        for (const Transition& transition: transitions) {
            std::cout << "    (" << from << ", " << transition.input << ", " << transition.stackTop << ") = {";
            for (const std::string& replacement: transition.replacements) {
                std::cout << "(" << transition.toState << ", " << replacement << "), ";
            }
            std::cout << "}\n";
        }
    }
    std::cout << "]\n";

    std::cout << "q0 = " << this->startState << "\n";


    std::cout << "F  = {"; printSortedPDA(this->acceptingStates); std::cout << "}" << std::endl;
}


void cartesianProduct(const std::vector<std::string>& a, const std::vector<std::string>& b) {

}


CFG PDA::toCFG() {
    CFG cfg{};

    cfg.addVariable("S");
    for (const std::string& q: this->states) {
        for (const std::string& X: this->stackAlphabet) {
            for (const std::string& p: this->states) {
                const std::string variable = "[" + q + "," + X + "," + p + "]";
                if (cfg.hasVariable(variable)) continue;

                cfg.addVariable(variable);
            }
        }
    }

    for (const std::string& a: this->alphabet) {
        cfg.addTerminal(a);
    }

    for (const std::string p : this->states) {
        cfg.addProduction("S", {"[" + this->startState + "," + this->stack.top() + "," + p + "]"});
    }

    this->alphabet.insert("");
    for (const auto& [from, transitions]: this->transitions) {
        for (const Transition& transition: transitions) {
            if (transition.replacements.size() == 1 && transition.replacements[0] == "") {
                cfg.addProduction("[" + from + "," + transition.stackTop + "," + transition.toState + "]", {transition.input});
            }

        }
    }

    cfg.setStartSymbol("S");

    return cfg;
}

