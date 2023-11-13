#include "grammar.h"

void Grammar::inputRules(int countRules) {
    for (int i = 0; i < countRules; ++i) {
        std::string line;
        std::cin >> line;

        std::string delimiter = "->";
        if (line.find(delimiter) == std::string::npos) {
            throw std::runtime_error("Invalid rule without delimiter");
        }

        auto nonTerminal = line.substr(0, line.find(delimiter));
        if (nonTerminal.size() != 1 || !isNonTerminal(nonTerminal[0])) {
            throw std::runtime_error("Left side of the rule should contain one non-terminal");
        }

        auto string = line.substr(line.find(delimiter) + delimiter.size(), line.size());
        for (auto symbol : string) {
            if (!isNonTerminal(symbol) && !isTerminal(symbol)) {
                throw std::runtime_error("Invalid symbol on the right side of the rule");
            }
        }

        rules[nonTerminal[0]].push_back(string);
    }
}

void Grammar::inputSymbols(int countSymbols, std::unordered_set<char>& setSymbols) {
    for (int i = 0; i < countSymbols; ++i) {
        char symbol;
        std::cin >> symbol;
        setSymbols.insert(symbol);
    }
}

void Grammar::scan() {
    int countNonTerminals, countTerminals, countRules;
    std::cin >> countNonTerminals >> countTerminals >> countRules;

    inputSymbols(countNonTerminals, nonTerminals);

    inputSymbols(countTerminals, terminals);

    inputRules(countTerminals);

    std::cin >> startingSymbol;
    if (!isNonTerminal(startingSymbol)) {
        throw std::runtime_error("Starting symbol should be non-terminal");
    }
}

bool Grammar::isTerminal(char symbol) {
    return terminals.find(symbol) != terminals.end();
}

bool Grammar::isNonTerminal(char symbol) {
    return nonTerminals.find(symbol) != terminals.end();
}
