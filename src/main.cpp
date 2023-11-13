#include "grammar.h"
#include "Earley.h"

int main() {
    Grammar grammar;
    grammar.scan();

    auto algo = Earley();

    algo.fit(grammar);

    int countStrings;
    std::cin >> countStrings;
    for (int i = 0; i < countStrings; ++i) {
        std::string string;
        std::cin >> string;
        std::cout << (algo.predict(string) ? "Yes" : "No") << '\n';
    }
}