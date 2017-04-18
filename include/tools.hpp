/*
 * Copyright (C) 2017  Bastien Penavayre
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include "string.hpp"

namespace constexpr_string
{
    template <class String, size_t I = 0, bool started = false, int value = 0, int sign = 1>
    constexpr int atoi()
    {
        if constexpr (I >= String::size) return value * sign;
        else if constexpr (!started)
        {
            if constexpr (!"-0123456789"_s.contains(String::instance[I])) return atoi<String, I+1, false, value, sign>();
            else if (String::instance[I] == '-') return atoi<String, I+1, false, value, sign * -1>();
            else return atoi<String, I+1, true, String::instance[I] - '0', sign>();
        }
        else if constexpr (!"0123456789"_s.contains(String::instance[I])) return value * sign;
        else return atoi<String, I+1, true, value * 10 + String::instance[I] - '0', sign>();
    }
}
