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
#include <gmp.h>
#include <stdexcept>
#include <cassert>
#include <ostream>
#include <limits>
#include "string_view.h"

struct GMPInteger
{
    mpz_t value;
    GMPInteger()
    {
        mpz_init(value);
    }
    GMPInteger(const GMPInteger &rt)
    {
        mpz_init_set(value, rt.value);
    }
    explicit GMPInteger(double newValue)
    {
        mpz_init_set_d(value, newValue);
    }
    explicit GMPInteger(long newValue)
    {
        mpz_init_set_si(value, newValue);
    }
    explicit GMPInteger(unsigned long newValue)
    {
        mpz_init_set_ui(value, newValue);
    }
    explicit GMPInteger(const char *newValue, int base = 0)
    {
        if(mpz_init_set_str(value, newValue, base) != 0)
            throw std::runtime_error("mpz_init_set_str failed: invalid number");
    }
    GMPInteger(GMPInteger &&rt)
    {
        mpz_init(value);
        mpz_swap(value, rt.value);
    }
    ~GMPInteger()
    {
        mpz_clear(value);
    }
    GMPInteger &operator=(GMPInteger rt)
    {
        mpz_swap(value, rt.value);
        return *this;
    }
    operator mpz_t &() & noexcept
    {
        return value;
    }
    operator const mpz_t &() const &noexcept
    {
        return value;
    }
    operator mpz_t &&() && noexcept
    {
        return std::move(value);
    }
    operator const mpz_t &&() const &&noexcept
    {
        return std::move(value);
    }
    operator bool() const = delete;
    friend std::ostream &operator<<(std::ostream &os, const GMPInteger &value)
    {
        using namespace util::string_view_literals;
        if(mpz_sgn(value.value) == 0)
        {
            os << "0";
            return os;
        }
        int base = 10;
        auto baseIndicatorUpper = ""_sv;
        auto baseIndicatorLower = ""_sv;
        switch(os.flags() & std::ios::basefield)
        {
        case std::ios::oct:
            base = 8;
            baseIndicatorUpper = "0"_sv;
            baseIndicatorLower = "0"_sv;
            break;
        case std::ios::hex:
            base = 16;
            baseIndicatorUpper = "0X"_sv;
            baseIndicatorLower = "0x"_sv;
            break;
        default:
            break;
        }
        auto baseIndicator = ""_sv;
        if(os.flags() & std::ios::uppercase)
        {
            base = -base;
            if(os.flags() & std::ios::showbase)
                baseIndicator = baseIndicatorUpper;
        }
        else if(os.flags() & std::ios::showbase)
        {
            baseIndicator = baseIndicatorLower;
        }
        std::unique_ptr<char[]> buffer(new char[mpz_sizeinbase(value, base) + 2 + baseIndicator.size()]);
        for(std::size_t i = 0; i < baseIndicator.size(); i++)
            buffer[i] = baseIndicator[i];
        mpz_get_str(buffer.get() + baseIndicator.size(), base, value);
        os << buffer.get();
        return os;
    }
};

class BitVector
{
public:
    enum class Kind
    {
        Signed,
        Unsigned,
    };

private:
    Kind kind;
    std::size_t bitCount;
    GMPInteger value;
    struct NormalizedAlreadyTag
    {
        friend class BitVector;

    private:
        explicit NormalizedAlreadyTag() = default;
    };
    static GMPInteger normalizeUnsigned(std::size_t bitCount, GMPInteger value)
    {
        mpz_fdiv_r_2exp(value, value, bitCount);
        return value;
    }
    static GMPInteger normalizeSigned(std::size_t bitCount, GMPInteger value)
    {
        if(bitCount == 0)
        {
            mpz_set_ui(value, 0);
            return value;
        }
        bool isNegative = mpz_tstbit(value, bitCount - 1);
        if(isNegative)
            mpz_neg(value, value);
        mpz_fdiv_r_2exp(value, value, bitCount);
        if(isNegative)
            mpz_neg(value, value);
        return value;
    }
    static GMPInteger normalize(Kind kind, std::size_t bitCount, GMPInteger value)
    {
        switch(kind)
        {
        case Kind::Unsigned:
            return normalizeUnsigned(bitCount, std::move(value));
        case Kind::Signed:
            return normalizeSigned(bitCount, std::move(value));
        }
        assert(false);
        return normalizeUnsigned(bitCount, std::move(value));
    }

public:
    BitVector() : kind(Kind::Unsigned), bitCount(1), value()
    {
    }
    BitVector(Kind kind, std::size_t bitCount) : kind(kind), bitCount(bitCount), value()
    {
        assert(bitCount <= maxBitCount());
    }
    BitVector(Kind kind, std::size_t bitCount, GMPInteger value)
        : kind(kind), bitCount(bitCount), value(normalize(kind, bitCount, std::move(value)))
    {
        assert(bitCount <= maxBitCount());
    }
    BitVector(Kind kind, std::size_t bitCount, GMPInteger value, NormalizedAlreadyTag)
        : kind(kind), bitCount(bitCount), value(std::move(value))
    {
        assert(bitCount <= maxBitCount());
    }
    static constexpr std::size_t maxBitCount() noexcept
    {
        auto a = std::numeric_limits<std::size_t>::max();
        auto b = std::numeric_limits<unsigned long>::max();
        if(a < b)
            return a;
        return b;
    }
    std::size_t getBitCount() const noexcept
    {
        return bitCount;
    }
    Kind getKind() const noexcept
    {
        return kind;
    }
    const GMPInteger &getValue() const noexcept
    {
        return value;
    }
    static BitVector add(Kind kind, std::size_t bitCount, BitVector l, const BitVector &r)
    {
        mpz_add(l.value, l.value, r.value);
        return BitVector(kind, bitCount, std::move(l.value));
    }
    static BitVector subtract(Kind kind, std::size_t bitCount, BitVector l, const BitVector &r)
    {
        mpz_sub(l.value, l.value, r.value);
        return BitVector(kind, bitCount, std::move(l.value));
    }
    static BitVector multiply(Kind kind, std::size_t bitCount, BitVector l, const BitVector &r)
    {
        mpz_mul(l.value, l.value, r.value);
        return BitVector(kind, bitCount, std::move(l.value));
    }
    static BitVector shiftLeft(Kind kind, std::size_t bitCount, BitVector l, const BitVector &r)
    {
        if(mpz_cmp_ui(r.value.value, bitCount) >= 0 || mpz_sgn(r.value.value) < 0)
            mpz_set_ui(l.value, 0);
        else
            mpz_mul_2exp(l.value, l.value, mpz_get_ui(r.value));
        return BitVector(kind, bitCount, std::move(l.value));
    }
    static BitVector shiftRight(Kind kind, std::size_t bitCount, BitVector l, const BitVector &r)
    {
        if(mpz_cmp_ui(r.value.value, bitCount) >= 0 || mpz_sgn(r.value.value) < 0)
            mpz_set_ui(l.value, mpz_sgn(l.value.value) < 0 ? -1 : 0);
        else
            mpz_fdiv_r_2exp(l.value, l.value, mpz_get_ui(r.value));
        return BitVector(kind, bitCount, std::move(l.value));
    }
    static BitVector concatenate(BitVector l, const BitVector &r)
    {
        l.bitCount += r.bitCount;
        mpz_mul_2exp(l.value, l.value, r.bitCount);
        mpz_add(l.value, l.value, normalizeUnsigned(r.bitCount, r.value));
        return l;
    }
    static BitVector slice(Kind kind, std::size_t bitCount, BitVector value, std::size_t startBit)
    {
        mpz_fdiv_q_2exp(value.value, value.value, startBit);
        return BitVector(kind, bitCount, std::move(value.value));
    }
    static BitVector cast(Kind kind, std::size_t bitCount, BitVector value)
    {
        return BitVector(kind, bitCount, std::move(value.value));
    }
    static int compare(const BitVector &l, const BitVector &r) noexcept
    {
        return mpz_cmp(l.value, r.value);
    }
};
