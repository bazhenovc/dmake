
#include "vs10generator.hh"
#include "parser.hh"
#include "target.hh"
#include "guid.hh"

// quick and dirty hack to check, whether the path is relative
inline bool isRelativePath(const std::string& path)
{
	return path[1] != ':' && path [0] != '/';
}

VS10Generator::VS10Generator(IDEType type)
	: mIDEType(type)
{}

void VS10Generator::generate(Parser& parser)
{
	// First generate .sln file
	generateSLNFile(parser);

	// Generate VC projects

	foreach(target, parser.getTargets()) {
		generateVCProject(*target, parser);
	}
}

void VS10Generator::generateSLNFile(Parser& parser)
{
	std::string path;
	if (VisualStudio == mIDEType) {
		path = "vs10/Project.sln";
	} else if (MonoDevelop == mIDEType) {
		path = "monodevelop/Project.sln";
	}
	std::ofstream ofs(path);

	if (ofs.is_open()) {

		// Write header
		ofs << "\nMicrosoft Visual Studio Solution File, Format Version 11.00" << std::endl
			<< "# Visual C++ Express 2010" << std::endl;

		// Dump all targets

		foreach(target, parser.getTargets())
		{
			std::string guid = makeGUID();
			std::string guid2 = makeGUID();

			guidMap[(*target)->name] = std::make_pair(guid, guid2);

			// Write project info
			std::string projectExt;
			if (VisualStudio == mIDEType) {
				projectExt = ".vcxproj";
			} else if (MonoDevelop == mIDEType) {
				projectExt = ".cproj";
			}
			ofs << "Project(\"{" << guid << "}\") = \"" << (*target)->name
				<< "\", \"" << (*target)->name << projectExt << "\", "
				<< "\"{" << guid2 << "}\"" << std::endl
				<< "EndProject" << std::endl;
		}

		std::string debugConfig;
		std::string releaseConfig;

		if (VisualStudio == mIDEType) {
			debugConfig = "\t\tDebug|Win32 = Debug|Win32";
			releaseConfig = "\t\tRelease|Win32 = Release|Win32";
		} else if (MonoDevelop == mIDEType) {
			debugConfig = "\t\tDebug|Any CPU = Debug|Any CPU";
			releaseConfig = "\t\tRelease|Any CPU = Release|Any CPU";
		}

		// Write global section
		ofs << "Global" << std::endl
			<< "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution" << std::endl
			<< debugConfig << std::endl
			<< releaseConfig << std::endl
			<< "\tEndGlobalSection" << std::endl
			<< "EndGlobal" << std::endl;
	}
}

void VS10Generator::generateVCProject(Target* target, Parser& parser)
{
	std::string projectPath;
	if (VisualStudio == mIDEType) {
		projectPath = "vs10/" + target->name + ".vcxproj";
	} else if (MonoDevelop == mIDEType) {
		projectPath = "monodevelop/" + target->name + ".cproj";
	}
	std::ofstream ofs(projectPath);

	if (ofs.is_open()) {

		std::string platform;
		if (VisualStudio == mIDEType) {
			platform = "Win32";
		} else {
			platform = "Any CPU";
		}

		// XML header
		ofs << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl
			<< "<Project DefaultTargets=\"Build\" ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">" << std::endl;

		if (VisualStudio == mIDEType) {
			ofs << "<ItemGroup Label=\"ProjectConfigurations\">" << std::endl
				<< "<ProjectConfiguration Include=\"Debug|" << platform << "\">" << std::endl
				<< "<Configuration>Debug</Configuration>" << std::endl
				<< "<Platform>Win32</Platform>" << std::endl
				<< "</ProjectConfiguration>" << std::endl
				<< "<ProjectConfiguration Include=\"Release|" << platform << "\">" << std::endl
				<< "<Configuration>Release</Configuration>" << std::endl
				<< "<Platform>" << platform << "</Platform>" << std::endl
				<< "</ProjectConfiguration>" << std::endl
				<< "</ItemGroup>" << std::endl

				<< "<PropertyGroup Label=\"Globals\">" << std::endl
				<< "<ProjectGuid>{" << guidMap[target->name].second << "}</ProjectGuid>" << std::endl
				<< "<RootNamespace>" << target->name << "</RootNamespace>" << std::endl
				<< "</PropertyGroup>" << std::endl;
		} else if (MonoDevelop == mIDEType) {
			ofs << "<PropertyGroup>" << std::endl
				<< "<Configuration Condition=\" '$(Configuration)' == '' \">Debug</Configuration>" << std::endl
				<< "<Platform Condition=\" '$(Platform)' == '' \">AnyCPU</Platform>" << std::endl
				<< "<ProductVersion>10.0.0</ProductVersion>" << std::endl
				<< "<SchemaVersion>2.0</SchemaVersion>" << std::endl
				<< "<ProjectGuid>{" << guidMap[target->name].second << "}</ProjectGuid>" << std::endl
				<< "<Target>Bin</Target>" << std::endl
				<< "<Language>CPP</Language>" << std::endl
				<< "<Compiler>" << std::endl
				<< "<Compiler ctype=\"GppCompiler\" />" << std::endl
				<< "</Compiler>" << std::endl
				<< "</PropertyGroup>" << std::endl;
		}

		if (VisualStudio == mIDEType)
			ofs << "<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />" << std::endl;

		ofs << "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|" << platform << "'\" Label=\"Configuration\">" << std::endl;
		
		std::string targetType;
		if (Target::Library == target->type)
			targetType = "DynamicLibrary";
		else {
			targetType = "Application";
			generateUserprefs(target, parser);
		}

		ofs << "<ConfigurationType>" << targetType << "</ConfigurationType>" << std::endl
			<< "<UseDebugLibraries>true</UseDebugLibraries>" << std::endl
			<< "<CharacterSet>MultiByte</CharacterSet>" << std::endl
			<< "</PropertyGroup>" << std::endl

			<< "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|" << platform << "'\" Label=\"Configuration\">" << std::endl
			<< "<ConfigurationType>" << targetType << "</ConfigurationType>" << std::endl
			<< "<UseDebugLibraries>false</UseDebugLibraries>" << std::endl
			<< "<WholeProgramOptimization>true</WholeProgramOptimization>" << std::endl
			<< "<CharacterSet>MultiByte</CharacterSet>" << std::endl
			<< "</PropertyGroup>" << std::endl;

		if (VisualStudio == mIDEType) {
			ofs << "<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />" << std::endl

				<< "<ImportGroup Label=\"ExtensionSettings\">" << std::endl
				<< "</ImportGroup>" << std::endl

				<< "<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">" << std::endl
				<< "<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />" << std::endl
				<< "</ImportGroup>" << std::endl

				<< "<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">" << std::endl
				<< "<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />" << std::endl
				<< "</ImportGroup>" << std::endl

				<< "<PropertyGroup Label=\"UserMacros\" />" << std::endl;
		}

		ofs << "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|" << platform << "'\">" << std::endl
			<< "<TargetName>" << target->name << "_d" << "</TargetName>" << std::endl
			<< "<LibraryPath>../;$(LibraryPath)</LibraryPath>" << std::endl
			<< "<OutDir>../</OutDir>" << std::endl;

		// Dump include paths
		std::string allIncludePaths;

		foreach(inc, target->contents["inc_paths"])
		{
			if (isRelativePath(*inc)) {
				allIncludePaths += "../" + *inc + ";";
			}
			else {
				allIncludePaths += *inc + ";";
			}
		}

		foreach(inc, target->contents["platforms.win32.inc_paths"])
		{
			if (isRelativePath(*inc)) {
				allIncludePaths += "../" + *inc + ";";
			}
			else {
				allIncludePaths += *inc + ";";
			}
		}

		ofs << "<IncludePath>$(IncludePath);" << allIncludePaths << "</IncludePath>" << std::endl
			<< "</PropertyGroup>" << std::endl

			<< "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|" << platform << "'\">" << std::endl
			<< "<TargetName>" << target->name << "</TargetName>" << std::endl
			<< "<LibraryPath>../;$(LibraryPath)</LibraryPath>" << std::endl
			<< "<OutDir>../</OutDir>" << std::endl
			<< "<IncludePath>$(IncludePath);" << allIncludePaths << "</IncludePath>" << std::endl
			<< "</PropertyGroup>" << std::endl

			<< "<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|" << platform << "'\">" << std::endl
			<< "<ClCompile>" << std::endl
			<< "<WarningLevel>Level3</WarningLevel>" << std::endl
			<< "<Optimization>Disabled</Optimization>" << std::endl
			<< "</ClCompile>" << std::endl
			<< "<Link>" << std::endl
			<< "<GenerateDebugInformation>true</GenerateDebugInformation>" << std::endl;
		// Dump dependencies for debug configuration
		ofs << "<AdditionalDependencies>%(AdditionalDependencies);";

		foreach(dep, target->contents["deps"])
		{
			// _d prefix for debug
			ofs << *dep << "_d.lib;";
		}
		if (VisualStudio == mIDEType) {
			foreach(lib, target->contents["platforms.win32.linker"])
			{
				ofs << *lib << ";";
			}
		} else if (MonoDevelop == mIDEType) {
			foreach(lib, target->contents["platforms.linux.linker"])
			{
				ofs << *lib << ";";
			}
		}
		ofs << "</AdditionalDependencies>" << std::endl;

		ofs << "</Link>" << std::endl
			<< "</ItemDefinitionGroup>" << std::endl

			<< "<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|" << platform << "'\">" << std::endl
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

		foreach(dep, target->contents["deps"])
		{
			ofs << *dep << ".lib;";
		}

		if (VisualStudio == mIDEType) {
			foreach(lib, target->contents["platforms.win32.linker"])
			{
				ofs << *lib << ";";
			}
		} else if (MonoDevelop == mIDEType) {
			foreach(lib, target->contents["platforms.linux.linker"])
			{
				ofs << *lib << ";";
			}
		}
		ofs << "</AdditionalDependencies>" << std::endl;

		ofs << "</Link>" << std::endl
			<< "</ItemDefinitionGroup>" << std::endl;

		// Write header files
		ofs << "<ItemGroup>" << std::endl;

		foreach(hdr, target->contents["hdr_files"])
		{
			ofs << "<ClInclude Include=\"../" << *hdr << "\">" << std::endl
				<< "<FileType>CppHeader</FileType>" << std::endl
				<< "</ClInclude>" << std::endl;
		}

		if (VisualStudio == mIDEType) {
			foreach(hdr, target->contents["platforms.win32.hdr_files"])
			{
				ofs << "<ClInclude Include=\"../" << *hdr << "\">" << std::endl
					<< "<FileType>CppHeader</FileType>" << std::endl
					<< "</ClInclude>" << std::endl;
			}
		} else if (MonoDevelop == mIDEType) {
			foreach(hdr, target->contents["platforms.linux.hdr_files"])
			{
				ofs << "<ClInclude Include=\"../" << *hdr << "\">" << std::endl
					<< "<FileType>CppHeader</FileType>" << std::endl
					<< "</ClInclude>" << std::endl;
			}
		}
		
		ofs << "</ItemGroup>" << std::endl;

		// Write source file list
		ofs << "<ItemGroup>" << std::endl;

		foreach(src, target->contents["src_files"])
		{
			ofs << "<ClCompile Include=\"../" << *src << "\" />" << std::endl;
		}

		if (VisualStudio == mIDEType) {
			foreach(src, target->contents["platforms.win32.src_files"])
			{
				ofs << "<ClInclude Include=\"../" << *src << "\">" << std::endl
					<< "<FileType>CppHeader</FileType>" << std::endl
					<< "</ClInclude>" << std::endl;
			}
		} else if (MonoDevelop == mIDEType) {
			foreach(src, target->contents["platforms.linux.src_files"])
			{
				ofs << "<ClInclude Include=\"../" << *src << "\">" << std::endl
					<< "<FileType>CppHeader</FileType>" << std::endl
					<< "</ClInclude>" << std::endl;
			}
		}
		ofs << "</ItemGroup>" << std::endl;

		// Finalize
		if (VisualStudio == mIDEType) {
			ofs << "<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />" << std::endl
				<< "<ImportGroup Label=\"ExtensionTargets\">" << std::endl
				<< "</ImportGroup>" << std::endl;
		}
		ofs << "</Project>";
	}
}

void VS10Generator::generateUserprefs(Target* target, Parser& parser)
{
	if (MonoDevelop == mIDEType) return;
	std::ofstream ofs("vs10/" + target->name + ".vcxproj.user");
	if (ofs.is_open()) {
		
		ofs << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl
			<< "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">" << std::endl
			<< "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">" << std::endl
			<< "<LocalDebuggerWorkingDirectory>../</LocalDebuggerWorkingDirectory>" << std::endl
			<< "<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>" << std::endl
			<< "</PropertyGroup>" << std::endl
			<< "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">" << std::endl
			<< "<LocalDebuggerWorkingDirectory>../</LocalDebuggerWorkingDirectory>" << std::endl
			<< "<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>" << std::endl
			<< "</PropertyGroup>" << std::endl
			<< "</Project>" << std::endl;
	}
}