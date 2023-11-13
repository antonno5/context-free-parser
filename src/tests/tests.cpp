#include <gtest/gtest.h>
#include "../grammar.h"
#include "../Earley.h"

TEST(GrammarTests, Scan) {
    struct inputScan {
        std::string buffer;
        char startingSymbol;
        std::unordered_set<char> nonTerminals;
        std::unordered_set<char> terminals;
        std::unordered_map<char, std::list<std::string>> rules;
    };

    inputScan testingInputs[2] = {
            {"1 2 2\nS\nab\nS->aSbS\nS->\nS"
                    ,'S', {'S'}, {'a', 'b'}
                    , {{'S', {"aSbS", ""}}} },
            {"1 2 2\nC\nab\nC->aCbC\nC->\nC"
                    ,'C', {'C'}, {'a', 'b'}
                    , {{'C', {"aCbC", ""}}} }
    };

    std::stringstream buffer;
    std::streambuf *sBuffer = std::cin.rdbuf();

    auto testInput = [](const inputScan& input, std::stringstream& buffer){
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

    std::string buffers[3] = {"1 2 2\nS\nab\nS->ac\nS->\nS"
    , "1 2 2\nS\nab\nS->ab\nS->\nB"
    , "1 2 2\nS\nab\nS->ac\nS\nS"};

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