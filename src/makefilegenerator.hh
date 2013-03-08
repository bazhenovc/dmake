
#pragma once

#include "common.hh"
#include "generator.hh"

class Parser;

class MakefileGenerator : public Generator
{
public:
    
    std::map<std::string, std::string> compilerMap;
    std::map<std::string, std::string> flagMap;
    
    MakefileGenerator()
    {
        compilerMap[".c"] = "gcc";
        compilerMap[".cc"] = "g++";
        compilerMap[".cpp"] = "g++";
    }
    
    inline void generateCompilationList(std::ofstream& ofs,
            const std::list<std::string>& files,
            const std::string& flags,
            const std::string& name)
    {
        foreach(src, files) {
                size_t slashPos = (*src).find_last_of('/');
                
                std::string extractedSource = (*src).substr(slashPos, (*src).size());
                std::string ext = extractedSource.substr(extractedSource.find_last_of("."), extractedSource.size());
                std::string compiler = compilerMap[ext];
                
                ofs << "$(BUILD_DIR)/" << extractedSource << "_" << name << ".o: " << (*src) << std::endl
			<< "\techo \"compiling " << extractedSource << " -> " << extractedSource << "_" << name << ".o\"" << std::endl
                        << "\t" << compiler << " $(GLOBAL_FLAGS)" << flagMap[compiler] << " " << "$(" << flags << ") -c " << (*src) << " -o $(BUILD_DIR)/" << extractedSource << "_" << name << ".o" << std::endl;
            }        
    }
    
    inline void generateObjectList(std::ofstream& ofs, const std::list<std::string>& files, const std::string& name)
    {
        foreach (src, files) {
                
                size_t slashPos = (*src).find_last_of('/');
                
                std::string extractedSource = (*src).substr(slashPos, (*src).size()) + "_" + name;
                
                ofs << "$(BUILD_DIR)/" << extractedSource << ".o ";
            }
    }
    
    virtual void generate(Parser& parser);
};