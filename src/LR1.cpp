#include "LR1.h"

void LR1::calcFirst(std::unordered_map<char, std::unordered_set<char>>& first) {
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

void LR1::go(const std::vector<Situation>& inClosure, char symbol, std::vector<Situation>& result, std::unordered_map<char, std::unordered_set<char>>& first) {
    std::vector<Situation> vector;
    for (auto situation : inClosure) {
        if (situation.cursor == situation.string->size()) {
            continue;
        } 
        if ((*situation.string)[situation.cursor] == symbol) {
            vector.push_back(situation);
            ++vector.back().cursor;
        }
    }
    return closure(vector, result, first);
}

std::unordered_set<char> LR1::getFirst(const std::string& string, std::unordered_map<char, std::unordered_set<char>>& first) {
    std::unordered_set<char> result;
    bool have$ = true;
    for (auto symbol : string) {
        if (grammar.isTerminal(symbol)) {
            have$ = false;
            result.insert(symbol);
            break;
        }
        for (const auto& el : first[symbol]) {
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
    return result;
}

void LR1::closure(const std::vector<Situation>& vector, std::vector<Situation>& result, std::unordered_map<char, std::unordered_set<char>>& first) {
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
            auto symbols = ((result[i].cursor + 1 == result[i].string->size()) ? getFirst(std::string(1, result[i].terminal), first) : 
                    getFirst(result[i].string->substr(result[i].cursor + 1) + result[i].terminal, first));
            
            for (auto symbol : symbols) {
                auto situation = Situation(&rule, 0, nonTerminal, symbol);
                if (!history.count(situation)) {
                    history.insert(situation);
                    result.push_back(situation);
                }
            }
        }
    }
}

void LR1::calcC(std::vector<std::vector<Situation>>& closures, std::unordered_map<char, std::unordered_set<char>>& first) {
    std::unordered_set<std::vector<Situation>, HashVector> history;
    Situation startSituation = {&(*grammar.rules[grammar.startingSymbol].begin()), 0, grammar.startingSymbol, '$'};
    std::vector<Situation> result;
    closure({startSituation}, result, first);

    closures.push_back(result);
    
    auto symbols = grammar.terminals;
    symbols.insert(grammar.nonTerminals.begin(), grammar.nonTerminals.end());
    for (int i = 0; i < closures.size(); ++i) {
        for (auto symbol : symbols) {
            auto newClosure = std::vector<Situation>();
            go(closures[i], symbol, newClosure, first);
            if (newClosure.size() && !history.count(newClosure)) {
                closures.push_back(newClosure);
                history.insert(newClosure);
            }
        }
    }
}

void LR1::addCell(int i, char symbol, std::pair<char, int> pair) {
    if (!LRtable[i].count(symbol) || LRtable[i][symbol] == pair) {
        LRtable[i][symbol] = pair;
    }
    else {
        throw std::runtime_error("Grammar isn't LR(1)");
    }
}

int LR1::getRuleNumber(const Situation& situation) {
    int ind = 0;
    for (auto& i: grammar.rules) {
        for (auto& j : i.second) {
            if (situation.string == &j) {
                return ind;
            }
            ++ind;
        }
    }
    throw std::runtime_error("Invalid rule in LR table");
}

std::pair<char, std::string*> LR1::getRule(int number) {
    int ind = 0;
    for (auto& i: grammar.rules) {
        for (auto& j : i.second) {
            if (number == ind) {
                return std::make_pair(i.first, &j);
            }
            ++ind;
        }
    }
    throw std::runtime_error("Invalid rule number in LR table");
}

void LR1::fit(Grammar otherGrammar) {
    std::unordered_map<char, std::unordered_set<char>> first;

    grammar = std::move(otherGrammar);
    grammar.nonTerminals.insert('&');
    grammar.rules['&'].push_back(std::string(1, grammar.startingSymbol));
    grammar.startingSymbol = '&';
    grammar.terminals.insert('$');

    calcFirst(first);

    auto closures = std::vector<std::vector<Situation>>();
    calcC(closures, first);

    LRtable = std::vector<std::unordered_map<char, std::pair<char, int>>>(closures.size()
        , std::unordered_map<char, std::pair<char, int>>());

    std::unordered_map<std::vector<Situation>, int, HashVector> number;

    for (int i = 0; i < closures.size(); ++i) {
        sort(closures[i].begin(), closures[i].end());
        number[closures[i]] = i;
    }

    for (int i = 0; i < closures.size(); ++i) {
        auto symbols = grammar.terminals;
        symbols.insert(grammar.nonTerminals.begin(), grammar.nonTerminals.end());
        for (auto symbol : symbols) {
            std::vector<Situation> result;
            go(closures[i], symbol, result, first);
            sort(result.begin(), result.end());
            if (!number.count(result)) {
                continue;
            }
            addCell(i, symbol, std::make_pair('S', number[result]));
        }
    }

    for (int i = 0; i < closures.size(); ++i) {
        for (auto& situation : closures[i]) {
            if (situation.cursor != situation.string->size()) {
                continue;
            }
            if (situation.nonTerminal == '&') {
                LRtable[i]['$'] = {'A', 0};
                continue;
            }
            addCell(i, situation.terminal, std::make_pair('R', getRuleNumber(situation)));
        }
    }
}

bool LR1::predict(const std::string& String) {
    auto string = String + '$';
    
    std::vector<std::pair<int, char>> stack;
    stack.push_back({0,'\0'});

    int index = 0;
    while (true) {
        if (!LRtable[stack.back().first].count(string[index])) {
            return false;
        }
        auto pair = LRtable[stack.back().first][string[index]];
        if (pair.first == 'S') {
            stack.emplace_back(pair.second, string[index]);
            ++index;
        }
        else if (pair.first == 'A') {
            return true;
        }
        else if (pair.first == 'R') {
            auto rule = getRule(pair.second);
            for (int i = (int)rule.second->size() - 1; i >= 0; --i) {
                if (stack.back().second != (*rule.second)[i]) {
                    return false;
                }
                stack.pop_back();
            }
            if (!LRtable[stack.back().first].count(rule.first)) {
                return false;
            }
            auto newCell = LRtable[stack.back().first][rule.first];
            if (newCell.first != 'S') {
                return false;
            }
            stack.emplace_back(newCell.second, rule.first);
        }
    }
    return false;
}
