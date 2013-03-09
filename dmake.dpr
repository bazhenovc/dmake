
section compilers
{
    section gcc
    {
        section flags
        {
            -Wall
            -MMD
            -MD
        }
        section cpp_flags
        {
            -std=gnu++0x
        }
    }
}

executable dmake
{
    section hdr_files
    {
        src/common.hh
        src/generator.hh
        src/guid.hh
        src/makefilegenerator.hh
        src/parser.hh
        src/target.hh
        src/vs10generator.hh
	src/codeblocksgenerator.hh
    }
    section src_files
    {
        src/main.cc
        src/parser.cc
        src/makefilegenerator.cc
        src/vs10generator.cc
	src/codeblocksgenerator.cc
    }
    section platforms
    {
        section linux
        {
            section linker
            {
                -luuid
            }
        }
    }
}