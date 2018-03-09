#include <iostream>
#include <fstream>
#include "parse/parse_error.h"
#include "parse/parser.h"
#include "parse/source.h"
#include "ast/context.h"
#include "util/dump_tree.h"
#include <string>

void help(const char *arg0)
{
    std::cerr << "usage: " << arg0 << " <filename.hdl>" << std::endl;
}

int main(int argc, char **argv)
{
    try
    {
        if(argc != 2)
        {
            help(argv[0]);
            return 1;
        }
        std::string arg = argv[1];
        if(arg == "-h" || arg == "--help")
        {
            help(argv[0]);
            return 0;
        }
        if(arg.empty() || (arg != "-" && arg[0] == '-'))
        {
            help(argv[0]);
            return 1;
        }
        auto source = parse::Source::makeSourceFromFile(std::move(arg), true);
        ast::Context context;
        try
        {
            auto *tree = parse::parseTopLevelModule(context, source.get());
            assert(tree);
            util::Arena dumpArena;
            util::DumpState dumpState(dumpArena, context.stringPool);
            auto *dumpTree = dumpState.getDumpNode(tree);
            std::ofstream os("out.gv");
            util::DumpTree::writeGraphvizDOT(os, dumpTree);
#warning finish
        }
        catch(parse::ParseError &e)
        {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }
    catch(parse::ParseError &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch(std::runtime_error &e)
    {
        std::cerr << "fatal error: " << e.what() << std::endl;
        return 1;
    }
}
