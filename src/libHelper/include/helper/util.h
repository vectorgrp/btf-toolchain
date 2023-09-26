#pragma once

/* util.h */

/* Copyright (c) 2023 Vector Informatik GmbH

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */


#include <fstream>
#include <iostream>
#include <string>

namespace helper
{
namespace util
{
/**
 * @brief Converts a wstirng to string.
 * @param in The wstring to be converted.
 * 
 * @return The converted string.
 */
std::string wstrTostr(const std::wstring& in);

/**
 * @brief Converts a string to wstring.
 * @param in The string to be converted.
 * 
 * @return The converted wstring.
 */
std::wstring strTowstr(const std::string& in);

/**
 * @brief wrapper for std::getline that removes CR (\r) at the end of the line.
 * @param file The input file.
 * @param out Object where the extracted line is stored.
 */
void getline(std::ifstream& file, std::string& out);

/**
 * @brief Template to enable a safe dynamic cast by catching nullptr.
*/
template <typename To, typename From> To safe_dynamic_cast(From src) noexcept
{
    To to{nullptr};
    try
    {
        to = dynamic_cast<To>(src);
    }
    catch (...)
    {
        to = nullptr;
    }

    if (to == nullptr)
    {
        std::cout << "Fatal Error: dynamic cast failed" << std::endl;
        std::terminate();
    }
    return to;
}

} // namespace util
} // namespace helper