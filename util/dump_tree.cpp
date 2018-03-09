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
#include <unordered_set>
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

void DumpTree::writeGraphvizDOT(std::ostream &os, const DumpTree *tree)
{
    struct Writer final
    {
        std::ostream &os;
        explicit Writer(std::ostream &os) : os(os)
        {
        }
        std::unordered_map<const DumpTree *, std::string> nodeToNameMap;
        const std::string &getNodeName(const DumpTree *node)
        {
            auto iter = nodeToNameMap.find(node);
            if(iter == nodeToNameMap.end())
            {
                std::ostringstream ss;
                ss << "id" << nodeToNameMap.size();
                iter = std::get<0>(nodeToNameMap.emplace(node, ss.str()));
            }
            return std::get<1>(*iter);
        }
        std::unordered_set<const DumpTree *> seenNodeSet;
        std::deque<const DumpTree *> workList;
        void addToWorkList(const DumpTree *node)
        {
            if(std::get<1>(seenNodeSet.insert(node)))
                workList.push_back(node);
        }
        void writeEscapedString(string_view str)
        {
            for(char ch : str)
            {
                switch(ch)
                {
                case '&':
                    os << "&amp;";
                    break;
                case '<':
                    os << "&lt;";
                    break;
                case '>':
                    os << "&gt;";
                    break;
                case '\n':
                    os << "<br align=\"left\"/>";
                    break;
                default:
                    os << ch;
                }
            }
        }
        void indent(std::size_t level)
        {
            for(std::size_t i = 0; i < level; i++)
                os << "    ";
        }
        void runTopLevel(const DumpTree *tree)
        {
            if(!tree)
            {
                os << "digraph\n";
                os << "{\n";
                indent(1);
                os << "nullptr [label=\"nullptr\"];\n";
                os << "}\n";
                return;
            }
            os << "digraph\n"
                  "{\n";
            indent(1);
            os << "node [shape=plaintext];\n";
            addToWorkList(tree);
            std::vector<decltype(tree->simpleVariables)::const_iterator> simpleVariables;
            std::vector<decltype(tree->pointerVariables)::const_iterator> pointerVariables;
            std::size_t nextNullNodeIndex = 0;
            while(!workList.empty())
            {
                auto *node = workList.front();
                workList.pop_front();
                auto &name = getNodeName(node);
                simpleVariables.clear();
                pointerVariables.clear();
                std::size_t indentLevel = 1;
                indent(indentLevel);
                os << name << " [label=<\n";
                indentLevel++;
                indent(indentLevel);
                os << "<table border=\"0\" cellborder=\"1\" "
                      "cellspacing=\"0\">\n";
                simpleVariables.reserve(node->simpleVariables.size());
                for(auto i = node->simpleVariables.begin(), end = node->simpleVariables.end();
                    i != end;
                    ++i)
                    simpleVariables.push_back(i);
                std::sort(simpleVariables.begin(),
                          simpleVariables.end(),
                          [](auto &&a, auto &&b) -> bool
                          {
                              return compareNames(*std::get<0>(*a), *std::get<0>(*b)) < 0;
                          });
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
                indentLevel++;
                indent(indentLevel);
                os << "<tr><td colspan=\"" << (pointerVariables.empty() && simpleVariables.empty() ?
                                                   1 :
                                                   pointerVariables.size() + simpleVariables.size())
                   << "\">";
                writeEscapedString(node->nodeName);
                os << "</td></tr>\n";
                indent(indentLevel);
                os << "<tr>\n";
                indentLevel++;
                for(auto &i : simpleVariables)
                {
                    indent(indentLevel);
                    os << "<td align=\"text\">";
                    writeEscapedString(*std::get<0>(*i));
                    os << "</td>\n";
                }
                for(auto &i : pointerVariables)
                {
                    indent(indentLevel);
                    os << "<td align=\"text\">";
                    writeEscapedString(*std::get<0>(*i));
                    os << "</td>\n";
                }
                if(simpleVariables.empty() && pointerVariables.empty())
                {
                    indent(indentLevel);
                    os << "<td align=\"text\">&nbsp;</td>\n";
                }
                indentLevel--;
                indent(indentLevel);
                os << "</tr>\n";
                indent(indentLevel);
                os << "<tr>\n";
                indentLevel++;
                for(auto &i : simpleVariables)
                {
                    indent(indentLevel);
                    os << "<td align=\"text\">";
                    writeEscapedString(std::get<1>(*i));
                    os << "</td>\n";
                }
                for(std::size_t i = 0; i < pointerVariables.size(); i++)
                {
                    indent(indentLevel);
                    os << "<td align=\"text\" port=\"" << i << "\">&nbsp;</td>\n";
                }
                if(simpleVariables.empty() && pointerVariables.empty())
                {
                    indent(indentLevel);
                    os << "<td align=\"text\">&nbsp;</td>\n";
                }
                indentLevel--;
                indent(indentLevel);
                os << "</tr>\n";
                indentLevel--;
                indent(indentLevel);
                os << "</table>\n";
                indentLevel--;
                indent(indentLevel);
                os << ">];\n";
                for(std::size_t index = 0; index < pointerVariables.size(); index++)
                {
                    auto &iter = pointerVariables[index];
                    if(std::get<1>(*iter))
                    {
                        addToWorkList(std::get<1>(*iter));
                        indent(indentLevel);
                        os << name << ":" << index << " -> " << getNodeName(std::get<1>(*iter))
                           << ";\n";
                    }
                    else
                    {
                        indent(indentLevel);
                        os << "nullptr" << nextNullNodeIndex << " [shape=point];\n";
                        indent(indentLevel);
                        os << name << ":" << index << " -> nullptr" << nextNullNodeIndex << ";\n";
                        nextNullNodeIndex++;
                    }
                }
#warning finish
            }
            os << "}\n";
        }
    };
    Writer(os).runTopLevel(tree);
}
}

