
#pragma once

class Parser;

class Generator
{
public:
    
    virtual void generate(Parser& parser) = 0;
};