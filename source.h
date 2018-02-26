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

#include "util/string_view.h"
#include <iosfwd>
#include <memory>
#include <string>
#include <cassert>

class Source
{
private:
    util::string_view sourceText;

protected:
    Source(util::string_view sourceText) noexcept : sourceText(sourceText)
    {
    }

public:
    virtual ~Source() = default;
    typedef util::string_view::iterator iterator;
    typedef util::string_view::const_iterator const_iterator;
    const_iterator begin() const noexcept
    {
        return sourceText.begin();
    }
    const_iterator end() const noexcept
    {
        return sourceText.end();
    }
    const_iterator cbegin() const noexcept
    {
        return sourceText.begin();
    }
    const_iterator cend() const noexcept
    {
        return sourceText.end();
    }
    std::size_t size() const noexcept
    {
        return sourceText.size();
    }
    const char *data() const noexcept
    {
        return sourceText.data();
    }
    util::string_view text() const noexcept
    {
        return sourceText;
    }
    virtual void writeLocation(std::ostream &os, std::size_t offset) const = 0;
    virtual util::string_view getFileName() const noexcept = 0;

private:
    class NullSource;
    class StringSource;

public:
    static const Source *getNullSource() noexcept;
    static std::unique_ptr<Source> makeSourceFromText(std::string text, std::string fileName);
    static std::unique_ptr<Source> makeSourceFromFile(std::string fileName,
                                                      bool checkForStdin = false);
    static std::unique_ptr<Source> makeSourceFromStdin();
};

struct Location
{
    const Source *source;
    std::size_t offset;
    constexpr Location() noexcept : source(nullptr), offset(0)
    {
    }
    constexpr Location(const Source *source, std::size_t offset) noexcept : source(source),
                                                                            offset(offset)
    {
    }
    constexpr operator bool() const noexcept
    {
        return source != nullptr;
    }
    const Source *getNonnullSource() const noexcept
    {
        if(source)
            return source;
        return Source::getNullSource();
    }
    friend std::ostream &operator<<(std::ostream &os, Location location);
};

inline std::ostream &operator<<(std::ostream &os, Location location)
{
    if(!location.source)
        location = Location(Source::getNullSource(), 0);
    location.source->writeLocation(os, location.offset);
    return os;
}

struct LocationRange
{
    const Source *source;
    std::size_t offset;
    std::size_t size;
    constexpr LocationRange(const Source *source, std::size_t offset, std::size_t size) noexcept
        : source(source),
          offset(offset),
          size(size)
    {
    }
    constexpr LocationRange(Location location = {}, std::size_t size = 0) noexcept
        : source(location.source),
          offset(location.offset),
          size(size)
    {
    }
    constexpr LocationRange(Location begin, Location end) noexcept : source(begin.source),
                                                                     offset(begin.offset),
                                                                     size(end.offset - begin.offset)
    {
        assert(begin.source == end.source);
        assert(begin.offset <= end.offset);
    }
    constexpr operator bool() const noexcept
    {
        return source != nullptr;
    }
    constexpr Location begin() const noexcept
    {
        return Location(source, offset);
    }
    constexpr Location end() const noexcept
    {
        return Location(source, offset + size);
    }
    constexpr void setBegin(Location begin) noexcept
    {
        *this = LocationRange(begin, end());
    }
    constexpr void setEnd(Location end) noexcept
    {
        *this = LocationRange(begin(), end);
    }
    const Source *getNonnullSource() const noexcept
    {
        if(source)
            return source;
        return Source::getNullSource();
    }
    util::string_view getText() const noexcept
    {
        if(!*this)
            return {};
        return source->text().substr(offset, size);
    }
    friend std::ostream &operator<<(std::ostream &os, LocationRange locationRange);
};

inline std::ostream &operator<<(std::ostream &os, LocationRange locationRange)
{
    os << locationRange.begin();
    return os;
}
