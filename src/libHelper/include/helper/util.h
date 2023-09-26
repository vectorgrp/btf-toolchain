#pragma once

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