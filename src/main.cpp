#include "grammar.h"
#include "Earley.h"
#include "LR1.h"
#include "fstream"

int main(int argc, char* argv[]) {   
    Parser *algo = nullptr;

    if (argc > 1 && (!strcmp(argv[1],"-A") || !strcmp(argv[1], "--algorithm"))) {
        if (argc < 3) {
            throw std::runtime_error("You should choose algorithm");
        }
        if (!strcmp(argv[2], "Earley")) {
            algo = new Earley;
        }
        else if (!strcmp(argv[2], "LR1")) {
            algo = new LR1;
        }
        else {
            throw std::runtime_error("Invalid algorithm");
        }
    }
    else {
        algo = new Earley;
    }

    std::ifstream stream("../input.in");
 
    Grammar grammar;
    grammar.scan(stream);

    algo->fit(grammar);

    int countStrings;
    stream >> countStrings;
    for (int i = 0; i < countStrings; ++i) {
        std::string string;
        stream >> string;
        std::cout << (algo->predict(string) ? "Yes" : "No") << '\n';
    }

    delete algo;
    stream.close();
}
