
#include <vector>

#include "common.hh"
#include "target.hh"
#include "parser.hh"
#include "generator.hh"
#include "makefilegenerator.hh"
#include "vs10generator.hh"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage: dmake <.dpr file>" << std::endl;
        exit(1);
    }
    Parser parser;
    parser.parse(argv[1]);
    //parser.dump();
    
    std::vector<Generator*> generators;
    generators.push_back(new MakefileGenerator);
	generators.push_back(new VS10Generator);
    
    foreach (g, generators) {
        (*g)->generate(parser);
        delete (*g);
    }
    return 0;
}
