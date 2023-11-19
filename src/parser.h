#pragma once

#include "grammar.h"

class Parser {
protected:
    Grammar grammar;

public:
    virtual void fit(Grammar) = 0;

    virtual bool predict(const std::string&) = 0;
};
