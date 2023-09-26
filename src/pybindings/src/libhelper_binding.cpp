/* libhelper_binding.cpp */

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


#include "libhelper_binding.h"

PYBIND11_MAKE_OPAQUE(std::unordered_map<size_t, std::string>)

void init_pybtfhelper(py::module_& m)
{
    py::module_ subm = m.def_submodule("helper", "utility library");

    py::bind_map<std::unordered_map<size_t, std::string>>(subm, "UnorderedMapStringDouble");

    py::enum_<helper::logging::LogLevel>(subm, "LogLevel")
        .value("none", helper::logging::LogLevel::none)
        .value("error", helper::logging::LogLevel::error)
        .value("warning", helper::logging::LogLevel::warning)
        .value("trace", helper::logging::LogLevel::trace);

    subm.def("initLogging", &helper::logging::initLogging, "Initializes logging", py::arg("log_level"), py::arg("log_file_path"));

    subm.def(
        "Log",
        [](helper::logging::LogLevel log_level, const std::string& message) {
            switch (log_level)
            {
            case helper::logging::LogLevel::error:
                helper::logging::printError() << message << '\n';
                break;
            case helper::logging::LogLevel::warning:
                helper::logging::printWarning() << message << '\n';
                break;
            case helper::logging::LogLevel::trace:
                helper::logging::printTrace() << message << '\n';
                break;
            case helper::logging::LogLevel::none:
                break;
            }
        },
        "Emit a log message", py::arg("log_level"), py::arg("message"));

    subm.def("flushLog", &helper::logging::flush, "Flushes the logger");
}
