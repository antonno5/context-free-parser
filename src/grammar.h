#pragma once

#include <iostream>
#include <unordered_set>
#include <string>
#include <unordered_map>
#include <list>
#include <utility>

class Grammar {
private:
    static void inputSymbols(int countSymbols, std::unordered_set<char>& setSymbols, std::istream& stream = std::cin);

    void inputRules(int countRules, std::istream& stream = std::cin);

    void clear();

public:
    std::unordered_set<char> nonTerminals;
    std::unordered_set<char> terminals;
    std::unordered_map<char, std::list<std::string>> rules;
    char startingSymbol;

    bool isTerminal(char symbol);

    bool isNonTerminal(char symbol);

    void scan(std::istream& stream = std::cin);
};
