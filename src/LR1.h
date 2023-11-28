#pragma once

#include "parser.h"
#include <vector>

class LR1 : public Parser {
private:
    struct Situation {
        const std::string* string;
        int cursor;
        char nonTerminal;
        char terminal;
        
        constexpr Situation(const std::string* String, int Cursor, char NonTerminal, char Terminal) : string(String), cursor(Cursor)
        , nonTerminal(NonTerminal), terminal(Terminal) {};

        bool operator<(const Situation& that) const {
            return std::make_tuple(string, cursor, nonTerminal, terminal) <
                   std::make_tuple(that.string, that.cursor, that.nonTerminal, that.terminal);
        }

        bool operator==(const Situation& that) const {
            return std::make_tuple(string, cursor, nonTerminal, terminal) ==
                   std::make_tuple(that.string, that.cursor, that.nonTerminal, that.terminal);
        }
    };

    struct HashSituation {
        size_t operator()(const Situation& situation) const {
            size_t numbers[4] = {(size_t)situation.string, (size_t)situation.cursor
                    , (size_t)situation.nonTerminal, (size_t)situation.terminal};
            std::size_t seed = 1023;
            for(auto& i : numbers) {
                seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    struct HashVector {
        size_t operator()(const std::vector<Situation>& situations) const {
            std::size_t seed = 1023;
            for(auto& i : situations) {
                seed ^= HashSituation()(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    std::unordered_map<char, std::unordered_set<char>> first;

    std::vector<std::vector<int>> automat;

    void calcFirst();

    void closure(const std::vector<Situation>&, std::vector<Situation>&);

    void go(const std::vector<Situation>&, char, std::vector<Situation>&);

    void calcC(std::vector<std::vector<Situation>>&);

    std::unordered_set<char> getFirst(const std::string&);

public:
    void fit(Grammar) final;

    bool predict(const std::string&) final;
};