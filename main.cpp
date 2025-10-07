#include "PDA.h"


int main() {
    PDA pda("pda.json");
    pda.toCFG().print();
    return 0;
}