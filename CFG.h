//
// Created by PPetre on 30/09/2025.
//

#ifndef CFG_H
#define CFG_H
#include <unordered_set>
#include <string>
#include <vector>
#include <map>


class CFG {
public:
    CFG();
    explicit CFG(const std::string& file);

    void addVariable(const std::string& variable);
    void addTerminal(const std::string& terminal);
    void addProduction(const std::string& variable, std::vector<std::string> production);
    void setStartSymbol(const std::string& startSymbol);

    [[nodiscard]] bool hasVariable(const std::string& variable) const;

    void print();

    void ll();

private:
    std::unordered_set<std::string> variables;
    std::unordered_set<std::string> terminals;
    // VARIABLE (HEAD) -> VECTOR OF PARSEABLE STRING(S) (BODY(IES))
    std::map<std::string, std::vector<std::vector<std::string>>> productions;
    std::string startSymbol;

    void getFirst(std::unordered_map<std::string, std::unordered_set<std::string>>& first) const;
    void getFollow(
        std::unordered_map<std::string, std::unordered_set<std::string>> &follow,
        std::unordered_map<std::string, std::unordered_set<std::string>> &first
    );

    std::string join(const std::vector<std::string>& vector);
    void buildTable(
        const std::vector<std::string>& columns,
        std::unordered_map<std::string, std::unordered_set<std::string>> &first,
        std::unordered_map<std::string, std::unordered_set<std::string>>& follow,
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& table
    );
};



#endif //CFG_H
