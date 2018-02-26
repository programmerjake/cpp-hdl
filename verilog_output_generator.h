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

#pragma once

#include "output_generator.h"
#include "util/string_view.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

class VerilogOutputGenerator final : public OutputGenerator
{
    friend class OutputGeneratorCreators;

private:
    struct Indent
    {
        std::size_t level = 0;
        friend std::ostream &operator<<(std::ostream &os, const Indent &indent)
        {
            for(std::size_t i = 0; i < indent.level; i++)
                os << "    ";
            return os;
        }
    };
    struct PushIndent
    {
        std::size_t &level;
        std::size_t previousLevel;
        explicit PushIndent(VerilogOutputGenerator *verilogOutputGenerator, int amount = 1) noexcept
            : level(verilogOutputGenerator->indent.level),
              previousLevel(level)
        {
            level += amount;
        }
        PushIndent(const PushIndent &) = delete;
        PushIndent &operator=(const PushIndent &) = delete;
        ~PushIndent()
        {
            level = previousLevel;
        }
    };

private:
    std::ostream &os;
    std::unique_ptr<Params> params;
    std::unordered_map<const ast::Symbol *, std::string> symbolToOutputNameMap;
    std::unordered_map<std::string, const ast::Symbol *> outputNameToSymbolMap;
    std::unordered_map<std::string, std::size_t> outputNameToUsageCountMap;
    std::unordered_set<const ast::Module *> modulesSet;
    std::vector<const ast::Module *> modulesList;
    Indent indent;

private:
    const std::string &getSymbolOutputName(const ast::Symbol *symbol);
    void findAllModules(const ast::Module *module);
    void addReservedWords();

public:
    struct VerilogParams final : public Params
    {
    };

public:
    explicit VerilogOutputGenerator(std::unique_ptr<Params> params) noexcept
        : os(*params->outputStream),
          params(std::move(params))
    {
    }

private:
    static std::unique_ptr<OutputGenerator> create(std::unique_ptr<Params> params);
    static std::unique_ptr<Params> createParams();

public:
    virtual void run(const ast::Module *module) override;
};
