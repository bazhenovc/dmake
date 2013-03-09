#pragma once

#include "common.hh"
#include "generator.hh"

class Parser;

class CodeBlocksGenerator : public Generator
{
public:

	virtual void generate(Parser& parser);
};