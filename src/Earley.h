#pragma once

#include "parser.h"
#include <vector>

class Earley : public Parser {
private:
    struct Situation {
        const std::string* string;
        int cursor;
        int i;
        char nonTerminal;

        Situation() = delete;

        Situation(const std::string& String, int I, char NonTerminal) : string(&String), cursor(0), i(I)
                , nonTerminal(NonTerminal) {}

        bool operator<(const Situation& that) const {
            return std::make_tuple(string, cursor, i, nonTerminal) <
                   std::make_tuple(that.string, that.cursor, that.i, that.nonTerminal);
        }

        bool operator==(const Situation& that) const {
            return std::make_tuple(string, cursor, i, nonTerminal) ==
                   std::make_tuple(that.string, that.cursor, that.i, that.nonTerminal);
        }
    };

    struct HashSituation {
        size_t operator()(const Situation& situation) const {
            size_t numbers[4] = {(size_t)situation.string, (size_t)situation.cursor
                    , (size_t)situation.i, (size_t)situation.nonTerminal};
            std::size_t seed = 1023;
            for(auto& i : numbers) {
                seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    std::vector<std::vector<Situation>> changes;
    std::vector<std::unordered_set<Situation, HashSituation>> history;

    void clearD();

    void complete(const Situation&, int);

    void predict(const Situation&, int);

    void scan(const Situation&, char, int);

    void addSituation(int, const Situation&);

public:
    void fit(Grammar) final;

    bool predict(const std::string&) final;
};
