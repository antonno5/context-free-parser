#include "grammar.h"

void Grammar::inputRules(int countRules, std::istream& stream) {
    for (int i = 0; i < countRules; ++i) {
        std::string line;
        stream >> line;

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

void Grammar::inputSymbols(int countSymbols, std::unordered_set<char>& setSymbols, std::istream& stream) {
    for (int i = 0; i < countSymbols; ++i) {
        char symbol;
        stream >> symbol;
        setSymbols.insert(symbol);
    }
}

void Grammar::scan(std::istream& stream) {
    clear();
    int countNonTerminals, countTerminals, countRules;
    stream >> countNonTerminals >> countTerminals >> countRules;

    inputSymbols(countNonTerminals, nonTerminals, stream);

    inputSymbols(countTerminals, terminals, stream);

    inputRules(countRules, stream);

    stream >> startingSymbol;
    
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

void Grammar::clear() {
    nonTerminals.clear();
    terminals.clear();
    rules.clear();
}
