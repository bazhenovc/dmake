
#include "makefilegenerator.hh"
#include "target.hh"
#include "parser.hh"

void MakefileGenerator::generate(Parser& parser)
{
    std::ofstream ofs("Makefile");

    ofs << "# This makefile was generated automatically, please do not edit" << std::endl
            << "BUILD_DIR := build" << std::endl
            << "DEST_DIR := ." << std::endl;

    // Global compiler flags
    ofs << "GLOBAL_FLAGS := ";
    auto flagList = parser.getSectionContents("compilers.gcc.flags");

    foreach(flag, flagList) {
        ofs << (*flag) << " ";
    }
    ofs << std::endl;

    // Local flags
    auto cppFlagList = parser.getSectionContents("compilers.gcc.cpp_flags");
    std::string& cppFlags = flagMap["g++"];

    foreach(flag, cppFlagList) {
        cppFlags += " " + *flag;
    }

    ofs << "all: dirs";

    foreach(target, parser.getTargets()) {
        ofs << " " << (*target)->name;
    }
    ofs << std::endl;

    ofs << "dirs:\n\tmkdir -p $(BUILD_DIR)\n\n";

    ofs << "clean:\n\trm -f $(BUILD_DIR)/*" << std::endl;

    foreach(target, parser.getTargets()) {
        std::string name = (*target)->name;

        if (Target::Library == (*target)->type)
            name = "lib" + name + ".so";

        ofs << "\trm -f $(DEST_DIR)/" << name << std::endl;
    }
    ofs << std::endl;

    // Compiler-generated dependencies
    ofs << "-include $(BUILD_DIR)/*.d" << std::endl << std::endl;

    // Targets

    foreach(itr, parser.getTargets()) {

        ofs << "# target: " << (*itr)->name << std::endl;

        std::string targetName = (*itr)->name;
        std::string targetOut;
        if (Target::Library == (*itr)->type) {
            targetOut = "lib";
            targetOut += targetName;
            targetOut += ".so";
        } else {
            targetOut = (*itr)->name;
        }

        std::string targetFlags = (*itr)->name + "_FLAGS";
        std::string targetObjects = (*itr)->name + "_OBJECTS";

        // Build target dependencies
        std::string deps;
        std::string linkDeps;

        foreach(dep, (*itr)->contents["deps"]) {
            deps += (*dep) + " ";
            linkDeps += " -l" + (*dep);
        }

        // Build flags list
        ofs << targetFlags << " :=";

		foreach(inc, parser.getSectionContents("globals.inc_paths")) {
			ofs << " -I" << *inc;
		}
        foreach(inc, (*itr)->contents["inc_paths"]) {
            ofs << " -I" << *inc;
        }
        foreach(inc, (*itr)->contents["platforms.linux.inc_paths"]) {
            ofs << " -I" << *inc;
        }
        if (Target::Library == (*itr)->type) {
            ofs << " -DPIC -fPIC";
        }
        ofs << std::endl;

        // Build object list
        ofs << targetObjects << " :=";

        generateObjectList(ofs, (*itr)->contents["src_files"], targetName);
        generateObjectList(ofs, (*itr)->contents["platforms.linux.src_files"], targetName);
        ofs << std::endl << std::endl;

        // Compilation list
        generateCompilationList(ofs, (*itr)->contents["src_files"], targetFlags, targetName);
        ofs << "# linux src files" << std::endl;
        generateCompilationList(ofs, (*itr)->contents["platforms.linux.src_files"], targetFlags, targetName);

        ofs << std::endl;

        // Link target
        if (targetName != targetOut)
            ofs << targetName << ": " << targetOut << std::endl;

        ofs << targetOut << ": $(" << targetObjects << ") " << deps << std::endl
		<< "\techo \"linking " << targetOut << "\"" << std::endl
                << "\t$(CXX) -L$(DEST_DIR) -Wl,-rpath=$(DEST_DIR) $(" << targetObjects << ") -o $(DEST_DIR)/" << targetOut;

        if (Target::Library == (*itr)->type)
            ofs << " -shared ";

        foreach(link, (*itr)->contents["platforms.linux.linker"]) {
            ofs << " " << (*link);
        }
        ofs << " " << linkDeps;

        ofs << std::endl << std::endl;
    }
}
