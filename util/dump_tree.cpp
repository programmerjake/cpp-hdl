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

#include "dump_tree.h"
#include <unordered_map>
#include <deque>
#include <tuple>
#include <algorithm>

namespace util
{
void DumpTree::writeJSON(std::ostream &os, const DumpTree *tree)
{
    struct Visitor
    {
        std::unordered_map<const DumpTree *, std::size_t> nodeToNameMap;
        std::ostream &os;
        explicit Visitor(std::ostream &os) noexcept : os(os)
        {
        }
        void indent(std::size_t indentLevel)
        {
            for(std::size_t i = 0; i < indentLevel; i++)
                os << "    ";
        }
        void writeString(string_view str)
        {
            os << '\"';
            for(char ch : str)
            {
                switch(ch)
                {
                case '\"':
                case '\\':
                    os << '\\' << ch;
                    break;
                case '\b':
                    os << "\\b";
                    break;
                case '\f':
                    os << "\\f";
                    break;
                case '\n':
                    os << "\\n";
                    break;
                case '\r':
                    os << "\\r";
                    break;
                case '\t':
                    os << "\\t";
                    break;
                default:
                {
                    auto uch = static_cast<unsigned char>(ch);
                    static constexpr char hexDigits[] = "0123456789ABCDEF";
                    if(uch < 0x20 || uch == 0x7F)
                        os << "\\u00" << hexDigits[(uch / 16) % 16] << hexDigits[uch % 16];
                    else
                        os << uch;
                }
                }
            }
            os << '\"';
        }
        void visit(const DumpTree *node, std::size_t indentLevel, bool needsTrailingComma)
        {
            if(!node)
            {
                indent(indentLevel);
                os << "null";
                if(needsTrailingComma)
                    os << ',';
                os << '\n';
                return;
            }
            decltype(nodeToNameMap)::iterator iter;
            bool inserted;
            std::tie(iter, inserted) = nodeToNameMap.emplace(node, nodeToNameMap.size());
            if(!inserted)
            {
                indent(indentLevel);
                os << "{ \"@ref\": " << std::get<1>(*iter) << " }";
                if(needsTrailingComma)
                    os << ',';
                os << '\n';
                return;
            }
            indent(indentLevel);
            os << "{\n";
            indentLevel++;
            indent(indentLevel);
            os << "\"@id\": " << std::get<1>(*iter) << ",\n";
            indent(indentLevel);
            os << "\"@name\": ";
            writeString(node->nodeName);
            std::vector<decltype(node->simpleVariables)::const_iterator> simpleVariables;
            simpleVariables.reserve(node->simpleVariables.size());
            for(auto i = node->simpleVariables.begin(), end = node->simpleVariables.end(); i != end;
                ++i)
                simpleVariables.push_back(i);
            std::sort(simpleVariables.begin(),
                      simpleVariables.end(),
                      [](auto &&a, auto &&b) -> bool
                      {
                          return compareNames(*std::get<0>(*a), *std::get<0>(*b)) < 0;
                      });
            for(auto &&i : simpleVariables)
            {
                os << ",\n";
                indent(indentLevel);
                writeString(*std::get<0>(*i));
                os << ": ";
                writeString(std::get<1>(*i));
            }
            std::vector<decltype(node->pointerVariables)::const_iterator> pointerVariables;
            pointerVariables.reserve(node->pointerVariables.size());
            for(auto i = node->pointerVariables.begin(), end = node->pointerVariables.end();
                i != end;
                ++i)
                pointerVariables.push_back(i);
            std::sort(pointerVariables.begin(),
                      pointerVariables.end(),
                      [](auto &&a, auto &&b) -> bool
                      {
                          return compareNames(*std::get<0>(*a), *std::get<0>(*b)) < 0;
                      });
            bool needNewline = true;
            for(std::size_t i = 0, end = pointerVariables.size(); i < end; i++)
            {
                if(needNewline)
                {
                    os << ",\n";
                    needNewline = false;
                }
                indent(indentLevel);
                writeString(*std::get<0>(*pointerVariables[i]));
                os << ":";
                if(std::get<1>(*pointerVariables[i]))
                {
                    os << "\n";
                    visit(std::get<1>(*pointerVariables[i]), indentLevel, i != end - 1);
                }
                else
                {
                    os << " null";
                    needNewline = true;
                }
            }
            if(needNewline)
                os << "\n";
            indent(indentLevel - 1);
            os << '}';
            if(needsTrailingComma)
                os << ',';
            os << '\n';
        }
    };
    Visitor(os).visit(tree, 0, false);
}
}
