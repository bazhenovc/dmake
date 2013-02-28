
section compilers
{
    section gcc
    {
        section flags
        {
            -Wall
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
        src/makefilegenerator.hh
        src/parser.hh
        src/target.hh
    }
    section src_files
    {
        src/main.cc
        src/parser.cc
        src/makefilegenerator.cc
    }
}