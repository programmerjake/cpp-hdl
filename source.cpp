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

#include "source.h"
#include <ostream>
#include <cstdio>
#include <algorithm>
#include <mutex>
#include <system_error>
#include <cerrno>

class Source::NullSource final : public Source
{
public:
    NullSource() noexcept : Source({})
    {
    }
    virtual void writeLocation(std::ostream &os, std::size_t offset) const override;
};

void Source::NullSource::writeLocation(std::ostream &os, std::size_t offset) const
{
    os << "<nullptr>:" << offset;
}

class Source::StringSource final : public Source
{
private:
    const std::string text;
    const std::string fileName;
    mutable std::vector<std::size_t> lineStartOffsets;
    mutable std::once_flag generateLineStartOffsetsOnceFlag;

public:
    StringSource(std::string text, std::string fileName) noexcept
        : Source(text),
          text(std::move(text)),
          fileName(std::move(fileName)),
          lineStartOffsets(),
          generateLineStartOffsetsOnceFlag()
    {
    }
    virtual void writeLocation(std::ostream &os, std::size_t offset) const override;

private:
    template <typename Fn>
    void generateLineStartOffsetsHelper(Fn fn) const
    {
        for(std::size_t offset = 0; offset < text.size(); offset++)
        {
            if(text[offset] == '\r')
            {
                if(offset + 1 < text.size() && text[offset + 1] == '\n')
                {
                    fn(offset + 2);
                    offset++;
                    continue;
                }
                fn(offset + 1);
            }
            else if(text[offset] == '\n')
            {
                fn(offset + 1);
            }
        }
    }
    void generateLineStartOffsets() const
    {
        std::size_t offsetCount = 0;
        generateLineStartOffsetsHelper([&](std::size_t)
                                       {
                                           offsetCount++;
                                       });
        lineStartOffsets.reserve(offsetCount);
        generateLineStartOffsetsHelper([&](std::size_t index)
                                       {
                                           lineStartOffsets.push_back(index);
                                       });
    }
    static constexpr std::size_t getColumnAfterTab(std::size_t columnBeforeTab,
                                                   std::size_t tabSize = 8) noexcept
    {
        return tabSize == 0 || columnBeforeTab == 0 ?
                   columnBeforeTab + 1 :
                   columnBeforeTab + (tabSize - (columnBeforeTab - 1) % tabSize);
    }
};

void Source::StringSource::writeLocation(std::ostream &os, std::size_t offset) const
{
    std::call_once(generateLineStartOffsetsOnceFlag, &StringSource::generateLineStartOffsets, this);
    std::size_t line =
        1 + lineStartOffsets.size()
        + (lineStartOffsets.rbegin() - std::lower_bound(lineStartOffsets.rbegin(),
                                                        lineStartOffsets.rend(),
                                                        offset,
                                                        std::greater<std::size_t>()));
    std::size_t lineStartOffset = 0;
    if(line > 1)
        lineStartOffset = lineStartOffsets[line - 2];
    std::size_t column = 1;
    for(std::size_t i = lineStartOffset; i < offset; i++)
    {
        if(text[i] == '\t')
            column = getColumnAfterTab(column);
        else
            column++;
    }
    os << fileName << ":" << line << ":" << column;
}

const Source *Source::getNullSource() noexcept
{
    static const NullSource retval;
    return &retval;
}

std::unique_ptr<Source> Source::makeSourceFromText(std::string text, std::string fileName)
{
    return std::make_unique<StringSource>(std::move(text), std::move(fileName));
}

std::unique_ptr<Source> Source::makeSourceFromFile(std::string fileName, bool checkForStdin)
{
    struct FileCloser
    {
        void operator()(std::FILE *file) noexcept
        {
            std::fclose(file);
        }
    };

    if(checkForStdin && fileName == "-")
        return makeSourceFromStdin();
    std::string text;
    {
        std::unique_ptr<FILE, FileCloser> file(std::fopen(fileName.c_str(), "r"));
        if(!file)
        {
            int error = errno;
            throw std::system_error(
                error, std::generic_category(), "opening " + fileName + " failed");
        }
        for(int ch = std::getc(file.get()); ch != EOF; ch = std::getc(file.get()))
            text += ch;
        if(std::ferror(file.get()))
        {
            int error = errno;
            throw std::system_error(
                error, std::generic_category(), "reading from " + fileName + " failed");
        }
    }
    return std::make_unique<StringSource>(std::move(text), std::move(fileName));
}

std::unique_ptr<Source> Source::makeSourceFromStdin()
{
    std::string text;
    for(int ch = std::getchar(); ch != EOF; ch = std::getchar())
        text += ch;
    if(std::ferror(stdin))
    {
        int error = errno;
        throw std::system_error(error, std::generic_category(), "getchar failed");
    }
    return std::make_unique<StringSource>(std::move(text), "-");
}
