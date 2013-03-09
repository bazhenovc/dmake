
#include "codeblocksgenerator.hh"
#include "target.hh"
#include "parser.hh"

// At the moment - only linux custom makefile project
void CodeBlocksGenerator::generate(Parser& parser)
{
    std::string projectName = parser.fileName.substr(0,
                                                     parser.fileName.find_last_of("."));
	std::ofstream ofs(projectName + "-makefile.cbp");
	if (ofs.is_open()) {
		ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>" << std::endl
			<< "<CodeBlocks_project_file>" << std::endl
			<< "<FileVersion major=\"1\" minor=\"6\" />" << std::endl
			<< "<Project>" << std::endl
			<< "<Option title=\""<< projectName << "\" />" << std::endl
			<< "<Option makefile_is_custom=\"1\" />" << std::endl
			<< "<Build>" << std::endl
			<< "<Target title=\"all\"></Target>" << std::endl
			<< "</Build>" << std::endl;
		foreach (target, parser.getTargets()) {
			foreach (unit, (*target)->contents["src_files"]) {
				ofs << "<Unit filename=\"" << *unit << "\" />" << std::endl;
			}
			foreach (unit, (*target)->contents["hdr_files"]) {
				ofs << "<Unit filename=\"" << *unit << "\" />" << std::endl;
			}
			foreach (unit, (*target)->contents["platforms.linux.src_files"]) {
				ofs << "<Unit filename=\"" << *unit << "\" />" << std::endl;
			}
			foreach (unit, (*target)->contents["platforms.linux.hdr_files"]) {
				ofs << "<Unit filename=\"" << *unit << "\" />" << std::endl;
			}
		}
		ofs << "<Extensions>" << std::endl
			<< "<code_completion />" << std::endl
			<< "<debugger />" << std::endl
			<< "</Extensions>" << std::endl
			<< "</Project>" << std::endl
			<< "</CodeBlocks_project_file>" << std::endl;

	}
}
