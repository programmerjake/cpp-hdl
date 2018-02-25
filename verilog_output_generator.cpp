/**
 * Copyright 2018 Jacob Lifshay
 *
 * This file is part of Cpp-HDL.
 *
 * Cpp-HDL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cpp-HDL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cpp-HDL.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "verilog_output_generator.h"
#include <sstream>

const std::string &VerilogOutputGenerator::getSymbolOutputName(const ast::Symbol *symbol)
{
    auto &retval = symbolToOutputNameMap[symbol];
    if(retval.empty())
    {
        std::size_t &nextOutputNameSuffix = outputNameToUsageCountMap[*symbol->name];
        nextOutputNameSuffix++;
        retval = *symbol->name;
        if(nextOutputNameSuffix != 1)
        {
            std::ostringstream ss;
            ss << "_" << nextOutputNameSuffix;
            retval += ss.str();
        }
        outputNameToSymbolMap[retval] = symbol;
    }
    return retval;
}

void VerilogOutputGenerator::findAllModules(const ast::Module *module)
{
    if(!std::get<1>(modulesSet.insert(module)))
        return;
    modulesList.push_back(module);
    for(auto *symbol : module->symbolTable->localSymbolsList)
        if(auto *submodule = dynamic_cast<const ast::Module *>(symbol))
            findAllModules(submodule);
}

void VerilogOutputGenerator::addReservedWords()
{
    static constexpr std::initializer_list<const char *> reservedWords = {
        "alias",
        "always",
        "always_comb",
        "always_ff",
        "always_latch",
        "and",
        "assert",
        "assign",
        "assume",
        "automatic",
        "before",
        "begin",
        "bind",
        "bins",
        "binsof",
        "bit",
        "break",
        "buf",
        "bufif0",
        "bufif1",
        "byte",
        "case",
        "casex",
        "casez",
        "cell",
        "chandle",
        "class",
        "clocking",
        "cmos",
        "config",
        "const",
        "constraint",
        "context",
        "continue",
        "cover",
        "covergroup",
        "coverpoint",
        "cross",
        "deassign",
        "default",
        "defparam",
        "design",
        "disable",
        "dist",
        "do",
        "edge",
        "else",
        "end",
        "endcase",
        "endclass",
        "endclocking",
        "endconfig",
        "endfunction",
        "endgenerate",
        "endgroup",
        "endinterface",
        "endmodule",
        "endpackage",
        "endprimitive",
        "endprogram",
        "endproperty",
        "endspecify",
        "endsequence",
        "endtable",
        "endtask",
        "enum",
        "event",
        "expect",
        "export",
        "extends",
        "extern",
        "final",
        "first_match",
        "for",
        "force",
        "foreach",
        "forever",
        "fork",
        "forkjoin",
        "function",
        "generate",
        "genvar",
        "highz0",
        "highz1",
        "if",
        "iff",
        "ifnone",
        "ignore_bins",
        "illegal_bins",
        "import",
        "incdir",
        "include",
        "initial",
        "inout",
        "input",
        "inside",
        "instance",
        "int",
        "integer",
        "interface",
        "intersect",
        "join",
        "join_any",
        "join_none",
        "large",
        "liblist",
        "library",
        "local",
        "localparam",
        "logic",
        "longint",
        "macromodule",
        "matches",
        "medium",
        "modport",
        "module",
        "nand",
        "negedge",
        "new",
        "nmos",
        "nor",
        "noshowcancelled",
        "not",
        "notif0",
        "notif1",
        "null",
        "or",
        "output",
        "package",
        "packed",
        "parameter",
        "pmos",
        "posedge",
        "primitive",
        "priority",
        "program",
        "property",
        "protected",
        "pull0",
        "pull1",
        "pulldown",
        "pullup",
        "pulsestyle_onevent",
        "pulsestyle_ondetect",
        "pure",
        "rand",
        "randc",
        "randcase",
        "randsequence",
        "rcmos",
        "real",
        "realtime",
        "ref",
        "reg",
        "release",
        "repeat",
        "return",
        "rnmos",
        "rpmos",
        "rtran",
        "rtranif0",
        "rtranif1",
        "scalared",
        "sequence",
        "shortint",
        "shortreal",
        "showcancelled",
        "signed",
        "small",
        "solve",
        "specify",
        "specparam",
        "static",
        "string",
        "strong0",
        "strong1",
        "struct",
        "super",
        "supply0",
        "supply1",
        "table",
        "tagged",
        "task",
        "this",
        "throughout",
        "time",
        "timeprecision",
        "timeunit",
        "tran",
        "tranif0",
        "tranif1",
        "tri",
        "tri0",
        "tri1",
        "triand",
        "trior",
        "trireg",
        "type",
        "typedef",
        "union",
        "unique",
        "unsigned",
        "use",
        "uwire",
        "var",
        "vectored",
        "virtual",
        "void",
        "wait",
        "wait_order",
        "wand",
        "weak0",
        "weak1",
        "while",
        "wildcard",
        "wire",
        "with",
        "within",
        "wor",
        "xnor",
        "xor",
    };
    for(auto &reservedWord : reservedWords)
        outputNameToUsageCountMap[static_cast<std::string>(reservedWord)]++;
}

std::unique_ptr<OutputGenerator> VerilogOutputGenerator::create(std::unique_ptr<Params> params)
{
    if(!params)
        params = createParams();
    return std::make_unique<VerilogOutputGenerator>(std::move(params));
}

std::unique_ptr<OutputGenerator::Params> VerilogOutputGenerator::createParams()
{
    return std::make_unique<VerilogParams>();
}

void VerilogOutputGenerator::run(const ast::Module *module)
{
    os << indent << "// automatically generated from " << module->locationRange.getNonnullSource()->getFileName() << "\n\n";
    addReservedWords();
    findAllModules(module);
    for(auto *module : modulesList)
        getSymbolOutputName(module); // assign module names first
    for(auto *module : modulesList)
    {
        auto &&outputName = getSymbolOutputName(module);
        os << indent << "module " << outputName << "();\n";
        {
            PushIndent pushIndent(this);
            // TODO: finish
        }
        os << indent << "endmodule // " << outputName << "\n\n";
    }
}
