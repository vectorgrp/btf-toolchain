/* util.cpp */

/* 
 * Copyright (c) 2023 Vector Informatik GmbH
 *
 * SPDX-License-Identifier: MIT
*/


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