
#include "parser.hh"
#include "target.hh"

#include <sstream>

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

        if ("import" == token) {

            std::string line;

            char c;
            while (true) {
                stream.get(c);
                if (';' == c)
                    break;
                line += c;
            }

            // Trim
            std::stringstream trimmer;
            trimmer << line;
            line.clear();
            trimmer >> line;

            parse(line.c_str());
        }

        Target::Type type = mNameTargetMap[token];

        if (Target::InvalidType != type) { // Valid registered type
            std::string sectionName;
            std::string junk; // {
            stream >> sectionName;
            stream >> junk;

            Target* target = new Target;
            target->name = sectionName;

            target->type = type;

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
            std::string contents;
            if ("{" == token) {
                char c;
                // Read junk whitespace
                stream.get(c);
                while ('}' != c) {
                    stream.get(c);
                    contents.append(1, c);
                }
                // Remove junk ' }'
                contents.resize(contents.size() - 2);
            } else {
                contents = token;
            }
            if (mCurrentTarget)
                mCurrentTarget->contents[name].push_back(contents);
            else
                mContents[name].push_back(contents);
        }

        stream >> token;
    } while (!stream.eof() && "}" != token);
}
