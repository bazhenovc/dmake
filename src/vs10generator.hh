
#pragma once

#include "common.hh"
#include "generator.hh"

#include <map>

class Parser;
class Target;

class VS10Generator : public Generator
{
public:
	
	virtual void generate(Parser& parser);
	
private:
	
	void generateSLNFile(Parser& parser);
	
	// Map targets to guids
	std::map<std::string, std::pair<std::string, std::string> > guidMap;
};