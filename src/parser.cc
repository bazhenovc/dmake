
#include "parser.hh"
#include "target.hh"

Parser::~Parser()
{
    foreach (itr, mTargets) delete (*itr);
}

void Parser::dump() const
{
    foreach (itr, mTargets)
        (*itr)->dump();
}

void Parser::parseRecursive(std::istream& stream, const std::string& name)
{
    std::string token;

    std::string sectionPrefix = name == "" ? "" : ".";

    stream >> token;

    if (token[0] == '#') {
        stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    do {

        if ("executable" == token) {
            std::string sectionName;
            std::string junk; // {
            stream >> sectionName;
            stream >> junk;

            Target* target = new Target;
            target->name = sectionName;

            target->type = Target::Executable;

            mTargets.push_back(target);
            mCurrentTarget = target;

            parseRecursive(stream, ""); // no nested targets

        } else if ("library" == token) {
            std::string sectionName;
            std::string junk; // {
            stream >> sectionName;
            stream >> junk;

            Target* target = new Target;
            target->name = sectionName;

            target->type = Target::Library;

            mTargets.push_back(target);
            mCurrentTarget = target;

            parseRecursive(stream, ""); // no nested targets

        } else if ("section" == token) {
            std::string sectionName;
            std::string junk; // {
            stream >> sectionName;
            stream >> junk;

            parseRecursive(stream, name + sectionPrefix + sectionName);
        } else {
            // Parse section contents
            if (mCurrentTarget)
                mCurrentTarget->contents[name].push_back(token);
            else
                mContents[name].push_back(token);
        }

        stream >> token;
    } while (!stream.eof() && "}" != token);
}