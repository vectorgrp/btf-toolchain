/* util.cpp */

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


#include "helper/util.h"

#include <algorithm>

namespace helper::util
{
std::string wstrTostr(const std::wstring& in)
{
    auto ret = std::string(in.size(), ' ');
    std::transform(in.begin(), in.end(), ret.begin(), [](const wchar_t c) { return static_cast<char>(c); });
    return ret;
}

std::wstring strTowstr(const std::string& in)
{
    auto ret = std::wstring(in.size(), static_cast<wchar_t>(' '));
    std::transform(in.begin(), in.end(), ret.begin(), [](const char c) { return static_cast<wchar_t>(c); });
    return ret;
}

void getline(std::ifstream& file, std::string& out)
{
    std::getline(file, out);

    if (!out.empty())
    {
        if (out.back() == '\r')
        {
            out = out.substr(0, out.size() - 1);
        }
    }
}

} // namespace helper::util