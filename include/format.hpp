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
#include "tools.hpp"
#include <type_traits>

namespace constexpr_format
{
    using namespace constexpr_string;
    
    template <class String>
    class format_string
    {
        template <size_t str_begin, size_t format_begin, size_t end, size_t index>
        struct substring
        {
            template <class... Args>
            static constexpr auto parse(std::tuple<Args...> const &t) {
                return String::instance.template
                    section<str_begin, (format_begin - 1)>() + std::get<index>(t);
            }
        };

        template <size_t begin, size_t end>
        static constexpr bool is_number()
        {
            if constexpr (begin >= end) return false;
            else if constexpr ("0123456789"_s.contains(String::instance[begin])) return true;
            else return is_number<begin+1, end>();
        }

        template <size_t begin, size_t end, size_t index>
        static constexpr size_t get_index()
        {
            if constexpr (!is_number<begin, end>()) return index;
            else return atoi<String, begin>();
        }

        template <size_t max = 0, size_t lastEnd = 0, class... Args>
        struct sublist
        {
            template <size_t str_begin, size_t format_begin, size_t end, size_t index>
            using push_format = sublist<(max < index ? index : max), end, Args...,
                substring<str_begin, format_begin, end, index>>;
        
            static constexpr size_t size = sizeof...(Args);
            static constexpr size_t Max = max;
        };

        template <bool is_in = false, size_t B = 0, size_t NB = 0, size_t I = 0, size_t Index = 0, class Results = sublist<>>
            static constexpr auto parser()
        {
            if constexpr (I >= String::size) return Results{};
            else
            {
                constexpr char c = String::instance[I];
                if constexpr (!is_in)
                {
                    if constexpr (c == '/') return parser<false, B, B, I+2, Index, Results>();
                    else if constexpr (c == '{') return parser<true, B, I+1, I+1, Index, Results>();
                    else return parser<false, B, B, I+1, Index, Results>();
                }
                else
                {
                    if constexpr (c == '}') return parser<false, I+1, I+1, I+1, Index+(is_number<NB, I>() ? 0 : 1),
                                                          typename Results::template push_format<B, NB, I, get_index<NB, I, Index>()>>();
                    else if constexpr (!" 0123456789"_s.contains(c)) return parser<false, B, B, I+1, Index, Results>();
                    else return parser<true, B, NB, I+1, Index, Results>();
                }
            }
        }

        static constexpr auto formats = parser();

        template <size_t Max, size_t LastEnd, class... Formats, class... Args>
        constexpr auto apply_args(sublist<Max, LastEnd, Formats...>, std::tuple<Args...> const &t) const
        {
            return (... + Formats::parse(t)) + String::instance.template section<LastEnd+1, String::size>();
        }

    public:

        template <class... Args>
        constexpr auto operator()(Args... args) const
        {
            static_assert((... && is_compile_string<Args>::value),
                          "All argument must be compile-time strings");
            static_assert(sizeof...(Args) >= formats.Max, "Not enough arguments");
            return apply_args(formats, std::forward_as_tuple(args...));
        }
    };
}

template <class Char, Char... Chars>
constexpr auto operator""_format()
{
    using namespace constexpr_format;
    return format_string<string<static_cast<char>(Chars)...>>{};
}
