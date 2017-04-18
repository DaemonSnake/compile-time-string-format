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

#include <tuple> //size_t forward_as_tuple
#include <utility> //std::index_sequence

namespace constexpr_string
{
    template <char... Chars>
    struct string
    {
        static constexpr char c_str[sizeof...(Chars)+1] = {Chars..., '\0'};
        static constexpr string instance = {};
        static constexpr size_t size = sizeof...(Chars);
    
        constexpr bool operator==(string<Chars...> const &) const { return true; }

        template <char... OChars> constexpr
        bool operator==(string<OChars...> const &) const { return false; }

        template <char... OChars> constexpr
        string<Chars..., OChars...> operator+(string<OChars...> const &) const { return {}; }

        template <size_t Offset, size_t... Indexs> constexpr
        auto section(std::index_sequence<Indexs...>) const
        {
            return string<c_str[Indexs + Offset]...>{};
        }
    
        template <size_t From, size_t To> constexpr
        auto section() const
        {
            return section<From>(std::make_index_sequence<To - From>{});
        }

        constexpr char operator[](int index) const { return c_str[index]; }

        template<size_t I = 0>
        constexpr bool contains(char value) const
        {
            if constexpr (I >= size) return false;
            else if (c_str[I] == value) return true;
            else return contains<I+1>(value);
        }
    };

    template <class> struct is_compile_string { static constexpr bool value = false; };

    template <char... Chars> struct is_compile_string<string<Chars...>> {
        static constexpr bool value = true;
    };
}

//string literal to string class
template <class Char, Char... Chars>
constexpr auto operator""_s() {
    return constexpr_string::string<static_cast<char>(Chars)...>{};
}

//number to string class
template <char... Chars>
constexpr auto operator""_s() {
    return constexpr_string::string<Chars...>{};
}
