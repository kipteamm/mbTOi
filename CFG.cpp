//
// Created by PPetre on 30/09/2025.
//

#include "json.hpp"
#include "CFG.h"

#include <algorithm>
#include <fstream>
#include <iostream>

using json = nlohmann::json;


// CFG::CFG() {
//     this->variables.insert("BINDIGIT");
//     this->variables.insert("S");
//
//     this->terminals.insert("0");
//     this->terminals.insert("1");
//     this->terminals.insert("a");
//     this->terminals.insert("b");
//
//     this->productions["BINDIGIT"].push_back("0");
//     this->productions["BINDIGIT"].push_back("1");
//     this->productions["S"].push_back("");
//     this->productions["S"].push_back("a S b BINDIGIT");
//
//     this->startSymbol = "S";
// }

// std::vector<std::string> vv = data["Variables"].get<std::vector<std::string>>;
// std::sort(vv.begin(), vv.end(), [](const std::string& a, const std::string& b) {
//     return a < b;
// });

CFG::CFG() {}

CFG::CFG(const std::string& file) {
    std::ifstream input(file);
    json data = json::parse(input);

    for (const auto& variable: data["Variables"]) {
        this->variables.insert(variable.get<std::string>());
    }

    for (const auto& terminal: data["Terminals"]) {
        this->terminals.insert(terminal.get<std::string>());
    }

    for (const auto& production: data["Productions"]) {
        auto productions = production["body"].get<std::vector<std::string>>();
        if (productions.empty()) productions.emplace_back("");

        this->productions[production["head"].get<std::string>()].push_back(productions);
    }

    this->startSymbol = data["Start"].get<std::string>();
}



void printSorted(const std::unordered_set<std::string>& s) {
    std::vector<std::string> v(s.begin(), s.end());
    std::sort(v.begin(), v.end(), [](const std::string& a, const std::string& b) {
       return a < b;
    });

    for (int i = 0; i < v.size(); i++) {
        std::cout << v[i];
        if (i + 1 < v.size()) std::cout << ", ";
    }
}


void CFG::print() {
    std::cout << "V = {"; printSorted(this->variables); std::cout << "}\n";
    std::cout << "T = {"; printSorted(this->terminals); std::cout << "}\n";

    std::cout << "P = {\n";
    // for (const auto& [key, productions] : this->productions) {
    for (auto& pair : this->productions) {
        const std::string& key = pair.first;
        std::vector<std::vector<std::string>>& productions = pair.second;

        std::sort(productions.begin(), productions.end(), [](const std::vector<std::string>& a, const std::vector<std::string>& b) {
            return a[0] < b[0];
        });

        for (const std::vector<std::string>& production: productions) {
            std::cout << "    " + key + " -> " + "`";

            for (int i = 0; i < production.size(); i++) {
                std::cout << production[i];
                if (i + 1 < production.size()) std::cout << " ";
            }

            std::cout << "`\n";
        }
    }
    std::cout << "}\n";

    std::cout << "S = " << this->startSymbol << std::endl;
}
