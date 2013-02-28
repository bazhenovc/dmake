
#pragma once

#include "common.hh"

struct Target
{
    enum Type
    {
        Library,
        Executable
    };
    
    Type type;
    std::map<std::string, std::list<std::string> > contents;
    
    std::string name;
    
    void dump() const
    {
        foreach (itr, contents) {
            std::cout << (*itr).first << std::endl;
            foreach (itr2, (*itr).second) {
                std::cout << "    " << (*itr2) << std::endl;
            }
        }
    }
};