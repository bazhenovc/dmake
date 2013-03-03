
#pragma once

#include "common.hh"
#include "generator.hh"

#include <map>

class Parser;
struct Target;

class VS10Generator : public Generator
{
public:
	
	virtual void generate(Parser& parser);
	
private:
	
	void generateSLNFile(Parser& parser);
    void generateVCProject(Target* target, Parser& parser);
    void generateUserprefs(Target* target, Parser& parser);
	
	// Map targets to guids
	std::map<std::string, std::pair<std::string, std::string> > guidMap;
};