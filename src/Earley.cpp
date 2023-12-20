#include "Earley.h"

void Earley::fit(Grammar otherGrammar) {
    grammar = std::move(otherGrammar);
    grammar.nonTerminals.insert('&');
    grammar.rules['&'].push_back(std::string(1, grammar.startingSymbol));
    grammar.startingSymbol = '&';
}

bool Earley::predict(const std::string& string) {
    changes.resize(string.size() + 1);
    history.resize(string.size() + 1);

    Situation startSituation(*grammar.rules[grammar.startingSymbol].begin(), 0, grammar.startingSymbol);
    changes[0].push_back(startSituation);
    history[0].insert(startSituation);

    for (int i = 0; i < changes[0].size(); ++i) {
        complete(changes[0][i], 0);
        predict(changes[0][i], 0);
    }

    for (int i = 0; i < string.size(); ++i) {
        for (const auto& situation : changes[i]) {
            scan(situation, string[i], i);
        }
        for (int j = 0; j < changes[i + 1].size(); ++j) {
            complete(changes[i + 1][j], i + 1);
            predict(changes[i + 1][j], i + 1);
        }
    }

    auto finishSituation = startSituation;
    ++finishSituation.cursor;
    bool ans = history[string.size()].find(finishSituation) != history[string.size()].end();

    clearD();

    return ans;
}

void Earley::clearD() {
    changes.clear();
    history.clear();
}

void Earley::complete(const Situation& situation, int j) {
    if (situation.cursor == situation.string->size()) {
        for (const auto& parentSituation : changes[situation.i]) {
            if ((*parentSituation.string)[parentSituation.cursor] == situation.nonTerminal) {
                Situation newSituation(parentSituation);
                ++newSituation.cursor;
                addSituation(j, newSituation);
            }
        }
    }
}

void Earley::predict(const Situation& situation, int j) {
    auto symbol = (*situation.string)[situation.cursor];
    if (grammar.isNonTerminal(symbol)) {
        for (auto & string : grammar.rules[symbol]) {
            Situation newSituation(string, j, symbol);
            addSituation(j, newSituation);
        }
    }
}

void Earley::scan(const Situation& situation, char letter, int j) {
    if ((*situation.string)[situation.cursor] == letter) {
        auto newSituation = situation;
        ++newSituation.cursor;
        addSituation(j + 1, newSituation);
    }
}

void Earley::addSituation(int j, const Situation& situation) {
    if (history[j].find(situation) == history[j].end()) {
        changes[j].push_back(situation);
        history[j].insert(situation);
    }
}
