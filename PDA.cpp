//
// Created by PPetre on 7/10/2025.
//

#include <fstream>

#include "PDA.h"
#include "json.hpp"


PDA::PDA(const std::string &file) {
    std::ifstream input(file);
    nlohmann::json data = nlohmann::json::parse(input);

    for (const auto& state: data["States"]) {
        this->states.insert(state.get<std::string>());
    }

    for (const auto& symbol: data["alphabet"]) {
        this->alphabet.insert(symbol.get<std::string>());
    }

    for (const auto& symbol: data["StackAlphabet"]) {
        this->stackAlphabet.insert(symbol.get<std::string>());
    }

    for (const auto& _transition: data["Transition"]) {
        const Transition transition{
            _transition["input"].get<std::string>(),
            _transition["stackTop"].get<std::string>(),
            _transition["to"].get<std::string>(),
            _transition["input"].get<std::vector<std::string>>(),
        };

        this->transitions[_transition["from"].get<std::string>()].push_back(transition);
    }

    this->startState = data["StartState"].get<std::string>();
    this->stack.push(data["StartSTack"].get<std::string>());
}


CFG PDA::toCFG() {
    CFG cfg{};

    cfg.addVariable("S");
    for (const std::string& q: this->states) {
        for (const std::string& X: this->stackAlphabet) {
            for (const std::string& p: this->states) {
                const std::string variable = "[" + q + X + p + "]";
                if (cfg.hasVariable(variable)) continue;

                cfg.addVariable(variable);
            }
        }
    }

    for (const std::string& a: this->alphabet) {
        cfg.addTerminal(a);
    }

    cfg.addProduction();

    cfg.setStartSymbol("S");

    return cfg;
}

