#include "PDA.h"


int main() {
    PDA pda("json/pda.json");
    pda.print();
    pda.toCFG().print();
    return 0;
}