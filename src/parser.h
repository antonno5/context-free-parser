#pragma once

#include "grammar.h"

class Parser {
protected:
    Grammar grammar;

public:
    virtual void fit(Grammar) {}

    virtual bool predict(const std::string&) {
        return false;
    }
};
