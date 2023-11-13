#include <gtest/gtest.h>
#include "grammar.h"
#include "Earley.h"

TEST(GrammarTests, Scan) {
    Grammar grammar;
    grammar.scan();
}