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
