#include "LR1.h"

void LR1::calcFirst() {
    for (char terminal : grammar.terminals) {
        first[terminal].insert(terminal);
    }

    bool changes = true;
    while (changes) {
        changes = false;
        for (auto rule : grammar.rules) {
            char nonTerminal = rule.first;
            for (auto production : rule.second) {
                bool have$ = true;
                for (char symbol : production) {
                    if (first[symbol].count('$') == 0) {
                        for (auto i : first[symbol]) {
                            changes |= first[nonTerminal].insert(i).second;
                        }
                        have$ = false;
                        break;
                    }
                    for (auto i : first[symbol]) {
                        if (i == '$') {
                            continue;
                        }
                        changes |= first[nonTerminal].insert(i).second;
                    }
                }
                if (have$) {
                    changes |= first[nonTerminal].insert('$').second;
                }
            }
        }
    }
}

void LR1::go(const std::vector<Situation>& inClosure, char symbol, std::vector<Situation>& result) {
    std::vector<Situation> vector;
    for (auto situation : inClosure) {
        if (situation.cursor == situation.string->size()) {
            continue;
        } 
        if ((*situation.string)[situation.cursor] == symbol) {
            result.push_back(situation);
            ++result.back().cursor;
        }
    }
    return closure(vector, result);
}

std::unordered_set<char> LR1::getFirst(const std::string& string) {
    std::unordered_set<char> result;
    bool have$ = true;
    for (auto symbol : string) {
        if (grammar.isTerminal(symbol)) {
            have$ = false;
            result.insert(symbol);
            break;
        }
        for (auto el : first[symbol]) {
            if (el != '$') {
                result.insert(el);
            }
        }
        if (first[symbol].count('$') == 0) {
            have$ = false;
            break;
        }
    }
    if (have$) {
        result.insert('$');
    }
}

void LR1::closure(const std::vector<Situation>& vector, std::vector<Situation>& result) {
    result = vector;
    std::unordered_set<Situation, HashSituation> history;
    history.insert(result.begin(), result.end());
    for (int i = 0; i < result.size(); ++i) {
        if (result[i].cursor == result[i].string->size()) {
            continue;
        }
        char nonTerminal = (*result[i].string)[result[i].cursor];
        if (!grammar.isNonTerminal(nonTerminal)) {
            continue;
        }
        for (const auto& rule : grammar.rules[nonTerminal]) {
            for (auto symbol : getFirst(rule.substr(result[i].cursor + 1) + result[i].terminal)) {
                auto situation = Situation(&rule, 0, nonTerminal, symbol);
                if (history.count(situation)) {
                    history.insert(situation);
                    result.push_back(situation);
                }
            }
        }
    }
}

void LR1::calcC(std::vector<std::vector<Situation>>& closures) {
    std::unordered_set<std::vector<Situation>, HashVector> history;
    Situation startSituation = {&(*grammar.rules[grammar.startingSymbol].begin()), 0, grammar.startingSymbol, '$'};
    std::vector<Situation> result;
    closure({startSituation}, result);
    closures.push_back(result);
    for (int i = 0; i < closures.size(); ++i) {
        for (auto symbol : grammar.terminals) {
            auto newClosure = std::vector<Situation>();
            go(closures[i], symbol, newClosure);
            if (newClosure.size() && history.count(newClosure)) {
                closures.push_back(newClosure);
                history.insert(newClosure);
            }
        }
    }
}

void LR1::fit(Grammar otherGrammar) {
    grammar = std::move(otherGrammar);
    grammar.nonTerminals.insert('&');
    grammar.rules['&'].push_back(std::string(1, grammar.startingSymbol));
    grammar.startingSymbol = '&';
    grammar.terminals.insert('$');

    calcFirst();

    auto closures = std::vector<std::vector<Situation>>();
    calcC(closures);

    automat.resize(closures.size());
    for (int i = 0; i < automat.size(); ++i) {
        automat[i] = std::vector<int>(grammar.terminals.size() + grammar.nonTerminals.size(), 0);
    }

    std::unordered_map<Situation, int, HashSituation> number;

    for (int i = 0; i < closures.size(); ++i) {
        for (int j = 0; j < closures[i].size(); ++j) {
            number[closures[i][j]] = i;
        }
    }

    
}

bool LR1::predict(const std::string& string) {
    
}
