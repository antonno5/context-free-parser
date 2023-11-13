#include <gtest/gtest.h>
#include "../grammar.h"
#include "../Earley.h"

struct inputGrammar {
    std::string buffer;
    char startingSymbol;
    std::unordered_set<char> nonTerminals;
    std::unordered_set<char> terminals;
    std::unordered_map<char, std::list<std::string>> rules;
};

const int countGrammars = 4;

inputGrammar testingInputs[countGrammars] = {
        {"1 2 2\nS\nab\nS->aSbS\nS->\nS"
                ,'S', {'S'}, {'a', 'b'}
                , {{'S', {"aSbS", ""}}} }
        , {"1 2 2\nC\nab\nC->aCbC\nC->\nC"
                ,'C', {'C'}, {'a', 'b'}
                , {{'C', {"aCbC", ""}}} }
        , {"1 2 2\nS\nab\nS->aSb\nS->\nS"
                , 'S', {'S'}, {'a', 'b'}
                , {{'S', {"aSb", ""}}} }
        , {"3 6 6\nSAB\nabcdkz\nS->aSb\nS->AB\nA->c\nA->ddAk\nA->\nB->zzz\nS"
                , 'S', {'S', 'A', 'B'}, {'a', 'b', 'c', 'd', 'k', 'z'}
                , {{'S', {"aSb", "AB"}}, {'A', {"c", "ddAk", ""}}, {'B', {"zzz"}}}}
};

TEST(GrammarTests, Scan) {
    std::stringstream buffer;
    std::streambuf *sBuffer = std::cin.rdbuf();

    auto testInput = [](const inputGrammar& input, std::stringstream& buffer){
        buffer.clear();
        buffer << input.buffer;
        std::cin.rdbuf(buffer.rdbuf());
        Grammar grammar;
        EXPECT_NO_THROW(grammar.scan());
        EXPECT_EQ(grammar.startingSymbol, input.startingSymbol);
        EXPECT_EQ(grammar.terminals, input.terminals);
        EXPECT_EQ(grammar.nonTerminals, input.nonTerminals);
        EXPECT_EQ(grammar.rules, input.rules);
    };

    for (const auto & testingInput : testingInputs) {
        testInput(testingInput, buffer);
    }

    std::cin.rdbuf(sBuffer);
}

TEST(GrammarTests, ScanExceptions) {
    std::stringstream buffer;
    std::streambuf *sBuffer = std::cin.rdbuf();

    std::string buffers[5] = {"1 2 2\nS\nab\nS->aScS\nS->\nS"
    , "1 2 2\nS\nab\nS->aSbS\nS->\nB"
    , "1 2 2\nS\nab\nS->aSbS\nS\nS"
    , "1 2 2\nS\nab\nS->aSbS\nS->B\nS"
    , "1 2 2\nS\nab\nS->aSbS\nS->(\nS"};

    auto testException = [](const std::string& input, std::stringstream& buffer){
        buffer.clear();
        buffer << input;
        std::cin.rdbuf(buffer.rdbuf());
        Grammar grammar;
        EXPECT_THROW(grammar.scan(), std::runtime_error);
    };

    for (const auto & i : buffers) {
        testException(i, buffer);
    }

    std::cin.rdbuf(sBuffer);
}

TEST(EarleyTests, Tests) {
    std::vector<std::string> stringsInGrammar[countGrammars];
    std::vector<std::string> stringsOutGrammar[countGrammars];

    stringsInGrammar[0] = {"aababb", "aabb", ""};

    stringsOutGrammar[0] = {"aabbba"};

    stringsInGrammar[2] = {"", "ab", "aaaaaabbbbbb"};

    stringsOutGrammar[2] = {"a", "aaaa", "c", "aab", "bbb", "9()9"};

    stringsInGrammar[3] = {"zzz", "azzzb", "aazzzbb", "aaczzzbb", "czzz", "addddkkzzzb"};

    stringsOutGrammar[3] = {"", "a", "aaaczzzbb", "c"};

    for (int i = 0; i < countGrammars; ++i) {
        std::stringstream buffer;
        std::streambuf *sBuffer = std::cin.rdbuf();
        buffer.clear();
        buffer << testingInputs[i].buffer;
        std::cin.rdbuf(buffer.rdbuf());

        Grammar grammar;
        grammar.scan();

        Earley algo;
        algo.fit(grammar);

        for (const auto& string : stringsInGrammar[i]) {
            EXPECT_EQ(true, algo.predict(string));
        }

        for (const auto& string : stringsOutGrammar[i]) {
            EXPECT_EQ(false, algo.predict(string));
        }
    }
}