
#include "vs10generator.hh"
#include "parser.hh"
#include "target.hh"
#include "guid.hh"

// quick and dirty hack to check, whether the path is relative for win32 platform
// and again: only on WIN32 platform!
inline bool isRelativePath(const std::string& path)
{
	return path[1] != ':';
}

void VS10Generator::generate(Parser& parser)
{
	// First generate .sln file
	generateSLNFile(parser);
	
	// Generate VC projects
	foreach (target, parser.getTargets()) {
		
		std::ofstream ofs("vs10/" + (*target)->name + ".vcxproj");
		
		if (ofs.is_open()) {
			
			// XML header
			ofs << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl
				<< "<Project DefaultTargets=\"Build\" ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">" << std::endl
				
				<< "<ItemGroup Label=\"ProjectConfigurations\">" << std::endl
				<< "<ProjectConfiguration Include=\"Debug|Win32\">" << std::endl
				<< "<Configuration>Debug</Configuration>" << std::endl
				<< "<Platform>Win32</Platform>" << std::endl
				<< "</ProjectConfiguration>" << std::endl
				<< "<ProjectConfiguration Include=\"Release|Win32\">" << std::endl
				<< "<Configuration>Release</Configuration>" << std::endl
				<< "<Platform>Win32</Platform>" << std::endl
				<< "</ProjectConfiguration>" << std::endl
				<< "</ItemGroup>" << std::endl
				
				<< "<PropertyGroup Label=\"Globals\">" << std::endl
				<< "<ProjectGuid>{" << guidMap[(*target)->name].second << "}</ProjectGuid>" << std::endl
				<< "<RootNamespace>" << (*target)->name << "</RootNamespace>" << std::endl
				<< "</PropertyGroup>" << std::endl
				
				<< "<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />" << std::endl
				
				<< "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\" Label=\"Configuration\">" << std::endl;
			std::string targetType;
			if (Target::Library == (*target)->type)
				targetType = "DynamicLibrary";
			else
				targetType = "Application";
			
			ofs << "<ConfigurationType>" << targetType << "</ConfigurationType>" << std::endl
				<< "<UseDebugLibraries>true</UseDebugLibraries>" << std::endl
				<< "<CharacterSet>MultiByte</CharacterSet>" << std::endl
				<< "</PropertyGroup>" << std::endl
				
				<< "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\" Label=\"Configuration\">" << std::endl
				<< "<ConfigurationType>" << targetType << "</ConfigurationType>" << std::endl
				<< "<UseDebugLibraries>false</UseDebugLibraries>" << std::endl
				<< "<WholeProgramOptimization>true</WholeProgramOptimization>" << std::endl
				<< "<CharacterSet>MultiByte</CharacterSet>" << std::endl
				<< "</PropertyGroup>" << std::endl
				
				<< "<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />" << std::endl
				
				<< "<ImportGroup Label=\"ExtensionSettings\">" << std::endl
				<< "</ImportGroup>" << std::endl
				
				<< "<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">" << std::endl
				<< "<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />" << std::endl
				<< "</ImportGroup>" << std::endl
				
				<< "<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">" << std::endl
				<< "<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />" << std::endl
				<< "</ImportGroup>" << std::endl
				
				<< "<PropertyGroup Label=\"UserMacros\" />" << std::endl
				
				<< "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">" << std::endl
				<< "<TargetName>" << (*target)->name << "_d" << "</TargetName>" << std::endl
				<< "<LibraryPath>../;$(LibraryPath)</LibraryPath>" << std::endl
				<< "<OutDir>../</OutDir>" << std::endl;
			
				// Dump include paths
			std::string allIncludePaths;
			foreach(inc, (*target)->contents["inc_paths"]) {
				if (isRelativePath(*inc)) {
					allIncludePaths += "../" + *inc + ";";
				} else {
					allIncludePaths += *inc + ";";
				}
			}
			foreach(inc, (*target)->contents["platforms.win32.inc_paths"]) {
				if (isRelativePath(*inc)) {
					allIncludePaths += "../" + *inc + ";";
				} else {
					allIncludePaths += *inc + ";";
				}
			}
			
			ofs << "<IncludePath>$(IncludePath);" << allIncludePaths << "</IncludePath>" << std::endl
				<< "</PropertyGroup>" << std::endl
				
				<< "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">" << std::endl
				<< "<TargetName>" << (*target)->name << "</TargetName>" << std::endl
				<< "<LibraryPath>../;$(LibraryPath)</LibraryPath>" << std::endl
				<< "<OutDir>../</OutDir>" << std::endl
				<< "<IncludePath>$(IncludePath);" << allIncludePaths << "</IncludePath>" << std::endl
				<< "</PropertyGroup>" << std::endl
				
				<< "<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">" << std::endl
				<< "<ClCompile>" << std::endl
				<< "<WarningLevel>Level3</WarningLevel>" << std::endl
				<< "<Optimization>Disabled</Optimization>" << std::endl
				<< "</ClCompile>" << std::endl
				<< "<Link>" << std::endl
				<< "<GenerateDebugInformation>true</GenerateDebugInformation>" << std::endl;
			// Dump dependencies for debug configuration
			ofs << "<AdditionalDependencies>%(AdditionalDependencies);";
			foreach (dep, (*target)->contents["deps"]) {
				// _d prefix for debug
				ofs << *dep << "_d.lib;";
			}
			foreach (lib, (*target)->contents["platforms.win32.linker"]) {
				ofs << *lib << ";";
			}
			ofs << "</AdditionalDependencies>" << std::endl;
			
			ofs << "</Link>" << std::endl
				<< "</ItemDefinitionGroup>" << std::endl
  
				<< "<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">" << std::endl
				<< "<ClCompile>" << std::endl
				<< "<WarningLevel>Level3</WarningLevel>" << std::endl
				<< "<Optimization>MaxSpeed</Optimization>" << std::endl
				<< "<FunctionLevelLinking>true</FunctionLevelLinking>" << std::endl
				<< "<IntrinsicFunctions>true</IntrinsicFunctions>" << std::endl
				<< "</ClCompile>" << std::endl
				<< "<Link>" << std::endl
				<< "<GenerateDebugInformation>true</GenerateDebugInformation>" << std::endl
				<< "<EnableCOMDATFolding>true</EnableCOMDATFolding>" << std::endl
				<< "<OptimizeReferences>true</OptimizeReferences>" << std::endl;
			
			// Dump dependencies for release configuration
			ofs << "<AdditionalDependencies>%(AdditionalDependencies);";
			foreach (dep, (*target)->contents["deps"]) {
				ofs << *dep << ".lib;";
			}
			foreach (lib, (*target)->contents["platforms.win32.linker"]) {
				ofs << *lib << ";";
			}
			ofs << "</AdditionalDependencies>" << std::endl;
			
			ofs << "</Link>" << std::endl
				<< "</ItemDefinitionGroup>" << std::endl;
			
			// Write header files
			ofs << "<ItemGroup>" << std::endl;
			foreach (hdr, (*target)->contents["hdr_files"]) {
				ofs << "<ClInclude Include=\"../" << *hdr << "\">" << std::endl
					<< "<FileType>CppHeader</FileType>" << std::endl
					<< "</ClInclude>" << std::endl;
			}
			foreach (hdr, (*target)->contents["platforms.win32.hdr_files"]) {
				ofs << "<ClInclude Include=\"../" << *hdr << "\">" << std::endl
					<< "<FileType>CppHeader</FileType>" << std::endl
					<< "</ClInclude>" << std::endl;
			}
			ofs << "</ItemGroup>" << std::endl;
			
			// Write source file list
			ofs << "<ItemGroup>" << std::endl;
			foreach (src, (*target)->contents["src_files"]) {
				ofs << "<ClCompile Include=\"../" << *src << "\" />" << std::endl;
			}
			foreach (src, (*target)->contents["platforms.win32.src_files"]) {
				ofs << "<ClCompile Include=\"../" << *src << "\" />" << std::endl;
			}
			ofs << "</ItemGroup>" << std::endl;
		
			// Finalize
			ofs << "<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />" << std::endl
				<< "<ImportGroup Label=\"ExtensionTargets\">" << std::endl
				<< "</ImportGroup>" << std::endl
				<< "</Project>";
		}
	}
}

void VS10Generator::generateSLNFile(Parser& parser)
{
	std::ofstream ofs("vs10/Project.sln");
	
	if (ofs.is_open()) {
		
		// Write header
		ofs << "\nMicrosoft Visual Studio Solution File, Format Version 11.00" << std::endl
			<< "# Visual C++ Express 2010" << std::endl;
		
		// Dump all targets
		foreach (target, parser.getTargets()) {
			std::string guid = makeGUID();
			std::string guid2 = makeGUID();
			
			guidMap[(*target)->name] = std::make_pair(guid, guid2);
			
			// Write project info
			ofs << "Project(\"{" << guid << "}\") = \"" << (*target)->name
				<< "\", \"" << (*target)->name << ".vcxproj" << "\", "
				<< "\"{" << guid2 << "}\"" << std::endl
				<< "EndProject" << std::endl;
		}
		
		// Write global section
		ofs << "Global" << std::endl
			<< "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution" << std::endl
			<< "\t\tDebug|Win32 = Debug|Win32" << std::endl
			<< "\t\tRelease|Win32 = Release|Win32" << std::endl
			<< "\tEndGlobalSection" << std::endl
			<< "EndGlobal" << std::endl;
	}
}