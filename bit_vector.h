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
#include <memory>
#include <utility>
#include <type_traits>
class BitVector
{
private:
    class WordVector
    {
        // words are stored in little-endian order
    public:
#ifdef __SIZEOF_INT128__
        typedef std::uint64_t WordType;
        typedef unsigned __int128 DoubleWordType;
        static constexpr int wordBitWidth = 64;
#else
        typedef std::uint32_t WordType;
        typedef std::uint64_t DoubleWordType;
        static constexpr int wordBitWidth = 32;
#endif

    private:
        WordType *words;
        std::size_t wordsUsed;
        static constexpr std::size_t staticallyAllocatedWordsCount =
            sizeof(std::size_t) >= sizeof(WordType) ? sizeof(std::size_t) / sizeof(WordType) : 1;
        union
        {
            std::size_t wordsAllocated;
            WordType staticallyAllocatedWords[staticallyAllocatedWordsCount];
        };

    private:
        void freeWordsArray(WordType *wordsArray) const noexcept
        {
            if(wordsArray != staticallyAllocatedWords)
                delete[] wordsArray;
        }
        std::size_t getWordsAllocated() const noexcept
        {
            if(words != staticallyAllocatedWords)
                return wordsAllocated;
            return staticallyAllocatedWordsCount;
        }
        void resizeWordsArray(std::size_t newSize)
        {
            if(newSize > wordsUsed)
            {
                std::size_t oldWordsAllocated = getWordsAllocated();
                if(newSize > oldWordsAllocated)
                {
                    WordType *oldWordsArray = words;
                    std::size_t newWordsAllocated = oldWordsAllocated * 2;
                    if(newSize > newWordsAllocated)
                        newWordsAllocated = newSize;
                    words = new WordType[newWordsAllocated];
                    for(std::size_t i = 0; i < wordsUsed; i++)
                        words[i] = oldWordsArray[i];
                    // sign extend
                    for(std::size_t i = wordsUsed; i < newSize; i++)
                        words[i] = 0;
                    freeWordsArray(oldWordsArray);
                    wordsAllocated = newWordsAllocated;
                }
            }
            wordsUsed = newSize;
        }

    public:
        constexpr WordVector() noexcept : words(staticallyAllocatedWords),
                                          wordsUsed(0),
                                          staticallyAllocatedWords{}
        {
        }
        constexpr WordVector(WordType value) noexcept : words(staticallyAllocatedWords),
                                                        wordsUsed(value != 0 ? 1 : 0),
                                                        staticallyAllocatedWords{value}
        {
            static_assert(staticallyAllocatedWordsCount >= 1, "");
        }
        WordVector(WordVector &&rt) noexcept
            : words(rt.words == rt.staticallyAllocatedWords ? staticallyAllocatedWords : rt.words),
              wordsUsed(rt.wordsUsed),
              staticallyAllocatedWords{}
        {
            if(rt.words == rt.staticallyAllocatedWords)
            {
                for(std::size_t i = 0; i < wordsUsed; i++)
                    staticallyAllocatedWords[i] = rt.staticallyAllocatedWords[i];
            }
            else
            {
                wordsAllocated = rt.wordsAllocated;
            }
            rt.words = rt.staticallyAllocatedWords;
            rt.wordsUsed = 0;
        }
        WordVector(const WordVector &rt)
            : words(staticallyAllocatedWords), wordsUsed(0), staticallyAllocatedWords{}
        {
            resizeWordsArray(rt.wordsUsed);
            for(std::size_t i = 0, end = rt.wordsUsed; i < end; i++)
                words[i] = rt.words[i];
        }

    private:
        void swapStaticWithStatic(WordVector &other) noexcept
        {
            WordType temp[staticallyAllocatedWordsCount] = {};
            for(std::size_t i = 0; i < wordsUsed; i++)
                temp[i] = staticallyAllocatedWords[i];
            for(std::size_t i = 0; i < other.wordsUsed; i++)
                staticallyAllocatedWords[i] = other.staticallyAllocatedWords[i];
            for(std::size_t i = 0; i < wordsUsed; i++)
                other.staticallyAllocatedWords[i] = temp[i];
            std::swap(wordsUsed, other.wordsUsed);
        }
        void swapStaticWithNonstatic(WordVector &other) noexcept
        {
            WordType temp[staticallyAllocatedWordsCount] = {};
            for(std::size_t i = 0; i < wordsUsed; i++)
                temp[i] = staticallyAllocatedWords[i];
            wordsAllocated = other.wordsAllocated;
            for(std::size_t i = 0; i < wordsUsed; i++)
                other.staticallyAllocatedWords[i] = temp[i];
            words = other.words;
            other.words = other.staticallyAllocatedWords;
            std::swap(wordsUsed, other.wordsUsed);
        }
        void swapNonstaticWithStatic(WordVector &other) noexcept
        {
            other.swapStaticWithNonstatic(*this);
        }
        void swapNonstaticWithNonstatic(WordVector &other) noexcept
        {
            std::swap(words, other.words);
            std::swap(wordsUsed, other.wordsUsed);
            std::swap(wordsAllocated, other.wordsAllocated);
        }

    public:
        void swap(WordVector &other) noexcept
        {
            if(words == staticallyAllocatedWords)
            {
                if(other.words == other.staticallyAllocatedWords)
                    swapStaticWithStatic(other);
                else
                    swapStaticWithNonstatic(other);
            }
            else
            {
                if(other.words == other.staticallyAllocatedWords)
                    swapNonstaticWithStatic(other);
                else
                    swapNonstaticWithNonstatic(other);
            }
        }
        WordVector &operator=(const WordVector &rt)
        {
            WordVector(rt).swap(*this);
            return *this;
        }
        WordVector &operator=(WordVector &&rt)
        {
            WordVector(std::move(rt)).swap(*this);
            return *this;
        }
        ~WordVector()
        {
            freeWordsArray(words);
        }
        std::size_t size() const noexcept
        {
            return wordsUsed;
        }
        bool empty() const noexcept
        {
            return wordsUsed == 0;
        }
        std::size_t capacity() const noexcept
        {
            return getWordsAllocated();
        }
        WordType &operator[](std::size_t index) noexcept
        {
            return words[index];
        }
        const WordType &operator[](std::size_t index) const noexcept
        {
            return words[index];
        }
        WordType &front() noexcept
        {
            return words[0];
        }
        const WordType &front() const noexcept
        {
            return words[0];
        }
        WordType &back() noexcept
        {
            return words[wordsUsed - 1];
        }
        const WordType &back() const noexcept
        {
            return words[wordsUsed - 1];
        }
        typedef WordType *iterator;
        typedef const WordType *const_iterator;
        iterator begin() noexcept
        {
            return words;
        }
        iterator end() noexcept
        {
            return words + wordsUsed;
        }
        const_iterator begin() const noexcept
        {
            return words;
        }
        const_iterator end() const noexcept
        {
            return words + wordsUsed;
        }
        const_iterator cbegin() const noexcept
        {
            return words;
        }
        const_iterator cend() const noexcept
        {
            return words + wordsUsed;
        }
        void resize(std::size_t newSize)
        {
            resizeWordsArray(newSize);
        }
        WordType getMostSignificantWord() const noexcept
        {
            if(empty())
                return 0;
            return back();
        }
        WordType getLeastSignificantWord() const noexcept
        {
            if(empty())
                return 0;
            return front();
        }
    };
#error finish
};
