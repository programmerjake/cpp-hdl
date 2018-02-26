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

#include <iostream>
#include "ast/module.h"
#include <memory>
#include "util/string_view.h"
#include <tuple>

class OutputGenerator
{
public:
    virtual ~OutputGenerator() = default;
    struct Params
    {
        std::ostream *outputStream;
        Params() noexcept : outputStream(&std::cout)
        {
        }
        virtual ~Params() = default;
    };
    virtual void run(const ast::Module *module) = 0;
};

class OutputGeneratorCreators;

class OutputGeneratorCreator
{
public:
    using Params = OutputGenerator::Params;
    typedef std::unique_ptr<OutputGenerator> (*CreateFn)(std::unique_ptr<Params> params);
    typedef std::unique_ptr<Params> (*CreateParamsFn)();
    util::string_view name;
    util::string_view outputFileExtension;
    CreateFn createFn;
    CreateParamsFn createParamsFn;
    std::unique_ptr<OutputGenerator> create(std::unique_ptr<Params> params) const
    {
        return createFn(std::move(params));
    }
    std::unique_ptr<Params> createParams() const
    {
        return createParamsFn();
    }
};

class OutputGeneratorCreators final
{
private:
    static std::pair<const OutputGeneratorCreator *, std::size_t>
        getOutputGeneratorCreators() noexcept;

public:
    typedef const OutputGeneratorCreator *const_iterator;
    typedef const_iterator iterator;
    const_iterator begin() const noexcept
    {
        return std::get<0>(getOutputGeneratorCreators());
    }
    std::size_t size() const noexcept
    {
        return std::get<1>(getOutputGeneratorCreators());
    }
    const_iterator end() const noexcept
    {
        auto creators = getOutputGeneratorCreators();
        return std::get<0>(creators) + std::get<1>(creators);
    }
};

constexpr OutputGeneratorCreators outputGeneratorCreators{};
