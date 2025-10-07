//
// Created by PPetre on 30/09/2025.
//

#ifndef CFG_H
#define CFG_H
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

class CFG {
public:
    CFG();
    explicit CFG(const std::string& file);

    void print();

private:
    std::unordered_set<std::string> variables;
    std::unordered_set<std::string> terminals;
    // VARIABLE (HEAD) -> VECTOR OF PARSEABLE STRING(S) (BODY(IES))
    std::map<std::string, std::vector<std::vector<std::string>>> productions;
    std::string startSymbol;
};



#endif //CFG_H
