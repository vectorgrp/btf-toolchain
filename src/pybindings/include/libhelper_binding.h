#pragma once

#include "helper/helper.h"

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;

/**
 * @brief Helper function which is called inside the PYBIND11_MODULE macro and adds Python bindings to the C++ libhelper code.
 * @param[in] m Module interface for creating binding code.
*/
void init_pybtfhelper(py::module_& m);