
#pragma once

#include "common.hh"

struct Target;

class Parser
{
public:

    Parser()
    : mCurrentTarget(0)
    {}

    ~Parser();

    inline void parse(const char* filename)
    {
        std::ifstream inf(filename);
        fileName = filename;

        if (inf.is_open())
            parseRecursive(inf, "");
    }

    void dump() const;

    inline const std::list<std::string>& getSectionContents(const std::string& name)
    {
        return mContents[name];
    }

    inline const std::list<Target*>& getTargets() const
    {
        return mTargets;
    }

    std::string fileName;

private:

    std::list<Target*> mTargets;
    Target* mCurrentTarget;
    std::map<std::string, std::list<std::string> > mContents;

    void parseRecursive(std::istream& stream, const std::string& name);
};
