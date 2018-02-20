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
};

class BitVector
{
private:
    GMPInteger value;
    std::size_t bitCount;
#error finish
};
