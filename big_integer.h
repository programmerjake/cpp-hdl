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

#include <cstdint>
#include <limits>
#include <initializer_list>
#include <utility>
#include <cassert>

class BigInteger
{
private:
#ifdef __SIZEOF_INT128__
    using udword = unsigned __int128;
    using sdword = __int128;
    using uword = std::uint64_t;
    using sword = std::int64_t;
    static constexpr int bitsPerWord = 64;
#else
    using udword = std::uint64_t;
    using sdword = std::int64_t;
    using uword = std::uint32_t;
    using sword = std::int32_t;
    static constexpr int bitsPerWord = 32;
#endif
    static constexpr udword unsignedAbs(sdword value) noexcept
    {
        if(value < 0)
            return -static_cast<udword>(value);
        return value;
    }
    static constexpr uword unsignedAbs(sword value) noexcept
    {
        if(value < 0)
            return -static_cast<uword>(value);
        return value;
    }
    static constexpr uword highWord(udword value) noexcept
    {
        return value >> bitsPerWord;
    }
    static constexpr sword highWord(sdword value) noexcept
    {
        return value >> bitsPerWord;
    }
    static constexpr uword lowWord(udword value) noexcept
    {
        return value;
    }
    static constexpr uword lowWord(sdword value) noexcept
    {
        return value;
    }
    static constexpr udword makeDWord(uword high, uword low) noexcept
    {
        return (static_cast<udword>(high) << bitsPerWord) + low;
    }
    static constexpr sdword makeDWord(sword high, uword low) noexcept
    {
        return makeDWord(static_cast<uword>(high), low);
    }
    template <bool NeedDWord, bool IsSigned>
    struct NeedDWordTag
    {
    };
    template <typename T>
    using UnsignedNeedDWordTag = NeedDWordTag<(static_cast<T>(-1) > static_cast<uword>(-1)), false>;
    template <typename T>
    static constexpr T signedMax() noexcept
    {
        return static_cast<std::make_unsigned_t<T>>(-1) >> 1;
    }
    template <typename T>
    static constexpr T signedMin() noexcept
    {
        return -signedMax<T>() - 1;
    }
    template <typename T>
    using SignedNeedDWordTag =
        NeedDWordTag<(signedMax<T>() > signedMax<sword>() || signedMin<T>() < signedMin<sword>()),
                     true>;

private:
    class Storage
    {
        // little endian
    private:
        uword *words;
        std::ptrdiff_t wordsUsedAndSign;
        static constexpr std::size_t staticWordCount = 2;
        union
        {
            uword staticWords[staticWordCount];
            std::size_t wordsAllocated;
        };
        static_assert(sizeof(wordsAllocated) <= sizeof(staticWords), "");

    private:
        void initialize_to_zero() noexcept
        {
            words = &staticWords[0];
            wordsUsedAndSign = 1;
            staticWords[0] = 0;
        }

    public:
        constexpr Storage() noexcept : words(&staticWords[0]), wordsUsedAndSign(1), staticWords{}
        {
        }
        constexpr explicit Storage(udword value) noexcept
            : words(&staticWords[0]),
              wordsUsedAndSign(highWord(value) ? 2 : 1),
              staticWords{lowWord(value), highWord(value)}
        {
        }
        constexpr explicit Storage(sdword value) noexcept : Storage(unsignedAbs(value))
        {
            if(value < 0)
                wordsUsedAndSign = -wordsUsedAndSign;
        }
        constexpr explicit Storage(uword value) noexcept : words(&staticWords[0]),
                                                           wordsUsedAndSign(1),
                                                           staticWords{value}
        {
        }
        constexpr explicit Storage(sword value) noexcept : Storage(unsignedAbs(value))
        {
            if(value < 0)
                wordsUsedAndSign = -wordsUsedAndSign;
        }
        constexpr Storage(uword value, NeedDWordTag<false, false>) noexcept : Storage(value)
        {
        }
        constexpr Storage(sword value, NeedDWordTag<false, true>) noexcept : Storage(value)
        {
        }
        constexpr Storage(udword value, NeedDWordTag<true, false>) noexcept : Storage(value)
        {
        }
        constexpr Storage(sdword value, NeedDWordTag<true, true>) noexcept : Storage(value)
        {
        }
        std::size_t size() const noexcept
        {
            if(wordsUsedAndSign < 0)
                return -wordsUsedAndSign;
            return wordsUsedAndSign;
        }
        bool isStaticallyAllocated() const noexcept
        {
            return words == &staticWords[0];
        }
        std::size_t capacity() const noexcept
        {
            if(isStaticallyAllocated())
                return staticWordCount;
            return wordsAllocated;
        }
        Storage(const Storage &rt) : staticWords{}
        {
            wordsUsedAndSign = rt.wordsUsedAndSign;
            std::size_t rtSize = rt.size();
            if(rtSize > staticWordCount)
            {
                wordsAllocated = rtSize;
                words = new uword[rtSize];
            }
            else
            {
                words = &staticWords[0];
            }
            for(std::size_t i = 0, end = rtSize; i < end; i++)
                words[i] = rt.words[i];
        }
        Storage(Storage &&rt) noexcept
            : words(rt.isStaticallyAllocated() ? &staticWords[0] : rt.words),
              wordsUsedAndSign(rt.wordsUsedAndSign),
              staticWords{}
        {
            if(rt.isStaticallyAllocated())
                for(std::size_t i = 0, end = rt.size(); i < end; i++)
                    staticWords[i] = rt.staticWords[i];
            rt.initialize_to_zero();
        }
        void clear() noexcept
        {
            if(!isStaticallyAllocated())
                delete[] words;
            initialize_to_zero();
        }
        ~Storage()
        {
            clear();
        }
        Storage &operator=(Storage &&rt) noexcept
        {
            clear();
            wordsUsedAndSign = rt.wordsUsedAndSign;
            if(rt.isStaticallyAllocated())
            {
                words = &staticWords[0];
                for(std::size_t i = 0, end = rt.size(); i < end; i++)
                    staticWords[i] = rt.staticWords[i];
            }
            else
            {
                words = rt.words;
                wordsAllocated = rt.wordsAllocated;
                rt.words = &rt.staticWords[0]; // already took
            }
            rt.initialize_to_zero();
            return *this;
        }
        Storage &operator=(const Storage &rt)
        {
            operator=(Storage(rt));
            return *this;
        }
        void swap(Storage &other) noexcept
        {
            Storage temp = std::move(other);
            other = std::move(*this);
            *this = std::move(temp);
        }
        typedef uword *iterator;
        typedef const uword *const_iterator;
        iterator begin() noexcept
        {
            return words;
        }
        const_iterator begin() const noexcept
        {
            return words;
        }
        const_iterator cbegin() const noexcept
        {
            return words;
        }
        iterator end() noexcept
        {
            return words + size();
        }
        const_iterator end() const noexcept
        {
            return words + size();
        }
        const_iterator cend() const noexcept
        {
            return words + size();
        }
        const uword &operator[](std::size_t index) const noexcept
        {
            assert(index < size());
            return words[index];
        }
        uword &operator[](std::size_t index) noexcept
        {
            assert(index < size());
            return words[index];
        }
        bool isNegative() const noexcept
        {
            return wordsUsedAndSign < 0;
        }
        bool isZero() const noexcept
        {
            return wordsUsedAndSign == 1 && words[0] == 0;
        }
        int sign() const noexcept
        {
            if(isNegative())
                return -1;
            if(isZero())
                return 0;
            return 1;
        }
        void setSign(int newSign) noexcept
        {
            wordsUsedAndSign = size();
            if(newSign < 0)
                wordsUsedAndSign = -wordsUsedAndSign;
        }
        void setSizeForTotalOverwrite(std::size_t newSize, int newSign = 1)
        {
            assert(newSize > 0
                   && newSize
                          <= static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()));
            if(newSize > capacity())
            {
                clear();
                words = new uword[newSize];
                wordsAllocated = newSize;
            }
            wordsUsedAndSign = newSize;
            if(newSign < 0)
                wordsUsedAndSign = -wordsUsedAndSign;
        }
        void setSizeForModification(std::size_t newSize)
        {
            assert(newSize > 0
                   && newSize
                          <= static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()));
            bool negative = isNegative();
            if(newSize > capacity())
            {
                auto *newWords = new uword[newSize];
                assert(newSize > size());
                for(std::size_t i = 0, end = size(); i < end; i++)
                    newWords[i] = words[i];
                for(std::size_t i = size(); i < newSize; i++)
                    newWords[i] = 0;
                clear();
                words = newWords;
                wordsAllocated = newSize;
            }
            wordsUsedAndSign = newSize;
            if(negative)
                wordsUsedAndSign = -wordsUsedAndSign;
        }
        void shrinkToSize(std::size_t newSize) noexcept
        {
            assert(newSize > 0 && newSize <= size());
            bool negative = isNegative();
            wordsUsedAndSign = newSize;
            if(negative)
                wordsUsedAndSign = -wordsUsedAndSign;
        }
        sdword toSDWord() const noexcept
        {
            constexpr auto minValue = signedMin<sdword>();
            constexpr auto maxValue = signedMax<sdword>();
            if(wordsUsedAndSign < -2) // negative and takes more than 2 words
                return minValue;
            if(wordsUsedAndSign > 2) // positive and takes more than 2 words
                return maxValue;
            uword highWord = 0;
            if(size() == 2)
                highWord = words[1];
            udword retval = makeDWord(highWord, words[0]);
            if(isNegative())
            {
                if(retval > static_cast<udword>(-static_cast<udword>(minValue)))
                    return minValue;
                retval = -retval;
            }
            else if(retval > static_cast<udword>(maxValue))
                return maxValue;
            return retval;
        }
        sword toSWord() const noexcept
        {
            constexpr auto minValue = signedMin<sword>();
            constexpr auto maxValue = signedMax<sword>();
            if(wordsUsedAndSign < -1) // negative and takes more than 1 word
                return minValue;
            if(wordsUsedAndSign > 2) // positive and takes more than 1 word
                return maxValue;
            uword retval = words[0];
            if(isNegative())
            {
                if(retval > static_cast<uword>(-static_cast<uword>(minValue)))
                    return minValue;
                retval = -retval;
            }
            else if(retval > static_cast<uword>(maxValue))
                return maxValue;
            return retval;
        }
        udword toUDWord() const noexcept
        {
            udword retval;
            if(size() > 1)
                retval = makeDWord(words[1], words[0]);
            else
                retval = words[0];
            if(isNegative())
                retval = -retval;
            return retval;
        }
        uword toUWord() const noexcept
        {
            if(isNegative())
                return -words[0];
            return words[0];
        }
        uword toIntegral(NeedDWordTag<false, false>) const noexcept
        {
            return toUWord();
        }
        sword toIntegral(NeedDWordTag<false, true>) const noexcept
        {
            return toSWord();
        }
        udword toIntegral(NeedDWordTag<true, false>) const noexcept
        {
            return toUDWord();
        }
        sdword toIntegral(NeedDWordTag<true, true>) const noexcept
        {
            return toSDWord();
        }
    };

private:
    Storage words;

private:
    void normalize() noexcept
    {
        std::size_t size = words.size();
        for(; size > 0; size--)
            if(words[size - 1] != 0)
                break;
        words.shrinkToSize(size);
    }

public:
    constexpr BigInteger() noexcept : words()
    {
    }
    template <typename T, typename = std::enable_if_t<std::is_signed<T>::value>>
    constexpr BigInteger(T value) noexcept : words(value, SignedNeedDWordTag<T>{})
    {
    }
    template <
        typename T,
        int = std::enable_if_t<std::is_unsigned<T>::value && !std::is_same<bool, T>::value, int>()>
    constexpr BigInteger(T value) noexcept : words(value, UnsignedNeedDWordTag<T>{})
    {
    }
    constexpr BigInteger(bool value) noexcept : words(static_cast<uword>(value))
    {
    }
    explicit operator bool() const noexcept
    {
        return words.isZero();
    }
    int sign() const noexcept
    {
        return words.sign();
    }
    bool isNegative() const noexcept
    {
        return words.isNegative();
    }
    bool isZero() const noexcept
    {
        return words.isZero();
    }
    bool isPositive() const noexcept
    {
        return sign() > 0;
    }
    template <typename T, typename = std::enable_if_t<std::is_signed<T>::value>>
    explicit operator T() const noexcept
    {
        auto retval = words.toIntegral(SignedNeedDWordTag<T>{});
        if(retval < signedMin<T>())
            return signedMin<T>();
        if(retval > signedMax<T>())
            return signedMax<T>();
        return retval;
    }
    template <
        typename T,
        typename = std::enable_if_t<std::is_unsigned<T>::value && !std::is_same<bool, T>::value>,
        typename = void>
    explicit operator T() const noexcept
    {
        return words.toIntegral(UnsignedNeedDWordTag<T>{});
    }
};
