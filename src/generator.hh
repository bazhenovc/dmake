
#pragma once

class Parser;

class Generator
{
public:
    virtual ~Generator() {}
    virtual void generate(Parser& parser) = 0;
};
