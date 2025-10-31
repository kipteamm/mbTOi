//
// Created by PPetre on 30/09/2025.
//

#include "json.hpp"
#include "CFG.h"

#include <algorithm>
#include <iostream>
#include <fstream>

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
        addVariable(variable.get<std::string>());
    }

    for (const auto& terminal: data["Terminals"]) {
        addTerminal(terminal.get<std::string>());
    }

    for (const auto& production: data["Productions"]) {
        addProduction(
            production["head"].get<std::string>(),
            production["body"].get<std::vector<std::string>>()
            );
    }

    this->startSymbol = data["Start"].get<std::string>();
}


void CFG::addVariable(const std::string &variable) {
    this->variables.insert(variable);
}

void CFG::addTerminal(const std::string &terminal) {
    this->terminals.insert(terminal);
}

void CFG::addProduction(const std::string &variable, std::vector<std::string> production) {
    if (production.empty()) production.push_back("");
    this->productions[variable].push_back(production);
}

void CFG::setStartSymbol(const std::string &startSymbol) {
    this->startSymbol = startSymbol;
}


bool CFG::hasVariable(const std::string &variable) const {
    return this->variables.count(variable);
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
            return a < b;
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


void CFG::ll() {
    std::cout << ">>> Building LL(1) Table\n";

    std::unordered_map<std::string, std::unordered_set<std::string>> first;
    for (const std::string& terminal: this->terminals) {
        first[terminal] = {terminal};
    }
    for (const std::string& variable: this->variables) {
        first[variable] = {};
    }

    getFirst(first);
    std::cout << " >> FIRST:\n";

    std::vector<std::string> f(this->variables.begin(), this->variables.end());
    std::sort(f.begin(), f.end(), [](const std::string& a, const std::string& b) {
       return a < b;
    });

    for (const std::string& variable: f) {
        std::vector<std::string> t(first[variable].begin(), first[variable].end());
        std::sort(t.begin(), t.end(), [](const std::string& a, const std::string& b) {
            if (a.empty()) return false;
            if (b.empty()) return true;
            return a < b;
        });

        std::cout << "    " << variable << ": " << "{" << t[0];
        for (int i = 1; i < t.size(); i++) std::cout << ", " << t[i];
        std::cout << "}\n";
    }

    std::unordered_map<std::string, std::unordered_set<std::string>> follow;
    for (const std::string& variable: this->variables) {
        follow[variable] = {};
    }
    follow[this->startSymbol].insert("<EOS>");

    getFollow(follow, first);
    std::cout << " >> FOLLOW:\n";
    for (const auto& entry: follow) {
        if (entry.first.empty()) continue;

        std::vector<std::string> t(entry.second.begin(), entry.second.end());
        std::sort(t.begin(), t.end(), [](const std::string& a, const std::string& b) {
            return a < b;
        });

        std::cout << "    " << entry.first << ": " << "{" << t[0];
        for (int i = 1; i < t.size(); i++) std::cout << ", " << t[i];
        std::cout << "}\n";
    }

    std::vector<std::string> columns(terminals.begin(), terminals.end());
    std::sort(columns.begin(), columns.end(), [](const std::string& a, const std::string& b) {
        return a < b;
    });
    columns.emplace_back("<EOS>");

    std::vector<std::string> rows(variables.begin(), variables.end());
    std::sort(rows.begin(), rows.end(), [](const std::string& a, const std::string& b) {
        return a < b;
    });

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> table;
    buildTable(columns, first, follow, table);

    std::unordered_map<std::string, int> widths;
    for (const std::string& row: rows) {
        for (const std::string& column: columns) {
            std::string& value = table[row][column];
            if (value == "``") value = "";

            const int size = static_cast<int>(std::max(value.size(), column.size()));

            if (size <= widths[column]) continue;
            widths[column] = size;
        }
    }

    std::cout << ">>> Table is built.\n";
    std::cout << "\n-------------------------------------\n\n";

    std::cout << "     |";
    for (const std::string& column: columns) {
        std::cout << " " << column;
        for (int s = 0; s < widths[column] - column.size() + 2; s++) std::cout << " ";
        std::cout << "|";
    }

    std::cout << "\n|----|";
    for (const std::string& column: columns) {
        for (int s = 0; s < widths[column] + 3; s++) std::cout << "-";
        std::cout << "|";
    }

    for (const std::string& row: rows) {
        std::cout << "\n| ";
        std::cout << row;
        std::cout << "  |";

        for (const std::string& column: columns) {
            std::cout << " ";

            std::string& value = table[row][column];
            if (value == "``") value = "";
            std::cout << value;

            for (int s = 0; s < widths[column] - value.size() + 2; s++) std::cout << " ";
            std::cout << "|";
        }
    }

    std::cout << "\n|----|";
    for (const std::string& column: columns) {
        for (int s = 0; s < widths[column] + 3; s++) std::cout << "-";
        std::cout << "|";
    }
}


void CFG::getFirst(std::unordered_map<std::string, std::unordered_set<std::string>> &first) const {
    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& production: this->productions) {
            for (const auto& body: production.second) {
                std::unordered_set<std::string> firstBody;

                if (body.empty() || (body.size() == 1 && body[0] == "")) firstBody.insert("");
                else {
                    bool hasEpsilon = true;
                    for (auto& symbol : body) {
                        for (auto& s : first[symbol]) {
                            if (s != "") firstBody.insert(s);
                        }
                        if (first[symbol].count("") == 0) {
                            hasEpsilon = false;
                            break;
                        }
                    }
                    if (hasEpsilon) firstBody.insert("");
                }

                const size_t before = first[production.first].size();
                first[production.first].insert(firstBody.begin(), firstBody.end());

                if (first[production.first].size() != before) changed = true;
            }
        }
    }
}


void CFG::getFollow(
    std::unordered_map<std::string, std::unordered_set<std::string>> &follow,
    std::unordered_map<std::string, std::unordered_set<std::string>> &first
) {
    bool changed = true;
    while (changed) {
        changed = false;

        for (auto& [head, bodies] : productions) {
            for (auto& body : bodies) {
                for (size_t i = 0; i < body.size(); ++i) {

                    std::string B = body[i];
                    if (!hasVariable(B)) continue;

                    std::unordered_set<std::string> trailer;
                    bool epsilonIn = true;

                    for (size_t j = i+1; j < body.size(); ++j) {
                        std::string sym = body[j];

                        for (auto& s : first[sym]) {
                            if (s != "") trailer.insert(s);
                        }
                        if (first[sym].count("") == 0) {
                            epsilonIn = false;
                            break;
                        }
                    }

                    if (epsilonIn) for (auto& s : follow[head]) trailer.insert(s);

                    const size_t before = follow[B].size();
                    follow[B].insert(trailer.begin(), trailer.end());
                    if (follow[B].size() != before) changed = true;
                }
            }
        }
    }
}


void CFG::buildTable(
    const std::vector<std::string>& columns,
    std::unordered_map<std::string, std::unordered_set<std::string>> &first,
    std::unordered_map<std::string, std::unordered_set<std::string>>& follow,
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& table
) {
    for (auto& v : variables) {
        for (auto& c : columns) {
            table[v][c] = "";
        }
    }

    for (auto& [head, bodies] : productions) {
        for (auto& body : bodies) {
            std::unordered_set<std::string> firstBody;
            if (body.empty() || (body.size() == 1 && body[0] == "")) firstBody.insert("");
            else {
                bool hasEpsilon = true;
                for (auto& sym : body) {
                    for (auto& s : first[sym]) {
                        if (s != "") firstBody.insert(s);
                    }
                    if (first[sym].count("") == 0) {
                        hasEpsilon = false;
                        break;
                    }
                }
                if (hasEpsilon) firstBody.insert("");
            }

            for (auto& term : firstBody) {
                if (term != "") table[head][term] = join(body);
            }
            if (firstBody.count("")) {
                for (auto& term : follow[head]) {
                    table[head][term] = join(body);
                }
            }
        }
    }

    for (auto& v : variables) {
        for (auto& c : columns) {
            if (table[v][c].empty()) {
                table[v][c] = "<ERR>";
            }
        }
    }
}


std::string CFG::join(const std::vector<std::string>& vector) {
    if (vector.empty()) return "";

    std::string s = "`" + vector[0];
    for (size_t i = 1; i < vector.size(); ++i) s += " " + vector[i];
    s += "`";
    return s;
}
