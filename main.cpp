#include "PDA.h"


int main() {
    CFG cfg("json/cfg.json");
    cfg.ll();
    return 0;
}