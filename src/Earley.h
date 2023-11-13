#pragma once

#include "grammar.h"
#include <vector>
#include <set>

class Earley {
private:
    Grammar grammar;

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
    };

    std::vector<std::vector<Situation>> changes;
    std::vector<std::set<Situation>> history;

    void clearD();

    void complete(const Situation&, int);

    void predict(const Situation&, int);

    void scan(const Situation&, char, int);

    void addSituation(int, const Situation&);

public:
    void fit(Grammar);

    bool predict(const std::string&);

};
