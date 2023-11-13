#pragma once

#include "grammar.h"
#include <vector>

struct Situation {
    const std::string* string;
    int cursor;
    int i;
    char nonTerminal;

    Situation() = delete;

    Situation(const std::string& String, int I, char NonTerminal) : string(&String), cursor(0), i(I)
            , nonTerminal(NonTerminal) {}

    bool operator<(const Situation& that) const {
        return std::make_tuple(cursor, i, string) <
               std::make_tuple(that.cursor, that.i, that.string);
    }

    bool operator==(const Situation& that) const {
        return std::make_tuple(cursor, i, string) ==
               std::make_tuple(that.cursor, that.i, that.string);
    }
};

template<>
struct std::hash<Situation> {
    size_t operator()(const Situation& situation) const {
        size_t numbers[4] = {(size_t)situation.string, (size_t)situation.cursor
                , (size_t)situation.i, (size_t)situation.nonTerminal};
        size_t hash = 0;
        std::size_t seed = 1023;
        for(auto& i : numbers) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return hash;
    }
};

class Earley {
private:
    Grammar grammar;

    std::vector<std::vector<Situation>> changes;
    std::vector<std::unordered_set<Situation>> history;

    void clearD();

    void complete(const Situation&, int);

    void predict(const Situation&, int);

    void scan(const Situation&, char, int);

    void addSituation(int, const Situation&);

public:
    void fit(Grammar);

    bool predict(const std::string&);

};
