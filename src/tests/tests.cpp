#include <gtest/gtest.h>
#include "../grammar.h"
#include "../Earley.h"
#include "../LR1.h"

struct inputGrammar {
    std::string stream;
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
    std::stringstream stream;

    auto testInput = [](const inputGrammar& input, std::stringstream& stream){
        stream << input.stream;
        Grammar grammar;
        EXPECT_NO_THROW(grammar.scan(stream));
        EXPECT_EQ(grammar.startingSymbol, input.startingSymbol);
        EXPECT_EQ(grammar.terminals, input.terminals);
        EXPECT_EQ(grammar.nonTerminals, input.nonTerminals);
        EXPECT_EQ(grammar.rules, input.rules);
    };

    for (const auto & testingInput : testingInputs) {
        testInput(testingInput, stream);
    }   
}

TEST(GrammarTests, ScanExceptions) {
    std::stringstream stream;

    std::string buffers[5] = {"1 2 2\nS\nab\nS->aScS\nS->\nS"
    , "1 2 2\nS\nab\nS->aSbS\nS->\nB"
    , "1 2 2\nS\nab\nS->aSbS\nS\nS"
    , "1 2 2\nS\nab\nS->aSbS\nS->B\nS"
    , "1 2 2\nS\nab\nS->aSbS\nS->(\nS"};

    auto testException = [](const std::string& input, std::stringstream& stream){
        stream << input;
        Grammar grammar;
        EXPECT_THROW(grammar.scan(stream), std::runtime_error);
    };

    for (const auto & i : buffers) {
        testException(i, stream);
    }
}

TEST(EarleyTests, Tests) {
    std::vector<std::string> stringsInGrammar[countGrammars];
    std::vector<std::string> stringsOutGrammar[countGrammars];

    stringsInGrammar[0] = std::vector<std::string>({"aababb", "aabb", ""});
    stringsOutGrammar[0] = std::vector<std::string>({"aabbba"});

    stringsInGrammar[2] = std::vector<std::string>({"", "ab", "aaaaaabbbbbb"});
    stringsOutGrammar[2] = std::vector<std::string>({"a", "aaaa", "c", "aab", "bbb", "9()9"});

    stringsInGrammar[3] = std::vector<std::string>({"zzz", "azzzb", "aazzzbb", "aaczzzbb", "czzz", "addddkkzzzb"});
    stringsOutGrammar[3] = std::vector<std::string>({"", "a", "aaaczzzbb", "c"});

    for (int i = 0; i < countGrammars; ++i) {
        std::stringstream stream;
        stream << testingInputs[i].stream;

        Grammar grammar;
        grammar.scan(stream);

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

TEST(LR1Tests, ExceptionTests) {
    std::string buffers[3] = {"2 1 2\nSD\na\nS->aaD\nD->D\nS"
        , "4 3 7\nSDGH\nabc\nS->DG\nD->abD\nD->G\nG->bGH\nG->c\nH->bH\nH->\nS"
        , "3 1 4\nSAB\na\nS->Aa\nS->Ba\nA->\nB->\nS"};

    for (int i = 0; i < 3; ++i) {
        std::stringstream stream;
        stream << buffers[i];
        Grammar grammar;
        grammar.scan(stream);

        LR1 parser;
        EXPECT_THROW(parser.fit(grammar), std::runtime_error);
    }
}

TEST(LR1Tests, Tests) {
    std::vector<std::string> stringsInGrammar[countGrammars];
    std::vector<std::string> stringsOutGrammar[countGrammars];

    stringsInGrammar[0] = std::vector<std::string>({"aababb", "aabb", ""});
    stringsOutGrammar[0] = std::vector<std::string>({"aabbba"});

    stringsInGrammar[2] = std::vector<std::string>({"", "ab", "aaaaaabbbbbb"});
    stringsOutGrammar[2] = std::vector<std::string>({"a", "aaaa", "c", "aab", "bbb", "9()9"});

    stringsInGrammar[3] = std::vector<std::string>({"zzz", "azzzb", "aazzzbb", "aaczzzbb", "czzz", "addddkkzzzb"});
    stringsOutGrammar[3] = std::vector<std::string>({"", "a", "aaaczzzbb", "c"});

    for (int i = 0; i < countGrammars; ++i) {
        std::stringstream stream;
        stream << testingInputs[i].stream;

        Grammar grammar;
        grammar.scan(stream);

        LR1 algo;
        algo.fit(grammar);

        for (const auto& string : stringsInGrammar[i]) {
            EXPECT_EQ(true, algo.predict(string));
        }

        for (const auto& string : stringsOutGrammar[i]) {
            EXPECT_EQ(false, algo.predict(string));
        }
    }
}