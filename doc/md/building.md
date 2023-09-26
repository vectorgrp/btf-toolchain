How to build the library {#building}
========================

This section contains a description of how to build the library.
It requires some external dependencies, which are listed below. \n
\n
********************************************************************
# Dependencies

* programmes:
	* [required] cmake
	* [required] gcc (9 or higher),  msvc (19.22 or higher) or clang (8 or higher)
	* [optional] python3: required for python bindings
	* [optional] ccach: for build acceleration
	* [optional] doxygen, dot and graphviz: for doxygen documentation generation
	* [optional] clang-tidy and/or cppcheck: for static source code analysis
	* [optional] include-what-you-use: for static source code analysis
	* [optional] clang-format: for source code formatting
* libraries:
	* [optional] pybind11: required for python bindings
	* [optional] pybind11-stubgen: required for python bindings
	* [optional] wheel: required for python bindings
	* [optional] catch2: required for testing

Configuring which dependencies to include can be done via the options in the top-level CMakeLists.txt file.
The default configuration only includes dependencies marked as required.\n
\n

## Python
To use the btf-toolchain in Python you need a version of Python3 and the modules pybind11, pybind11-stubgen and wheel. \n
After installing the prerequisites, the ENABLE_PYTHON_BINDINGS option can be set to YES and the Python bindings can be built. \n
The build command is described in the Build Commands subsection.\n
After running the build command, a wheel file (.whl) is created and placed in build/dist.
To use the pybtf module, it must be installed. This can be done, for example, by going to the build/dist folder and running:\n
```console
pip install [file_name].whl
```
\n
## Documentation
The documentation is built using doxygen. To build the documentation with cmake, install doxygen, dot and graphviz and set the ENABLE_DOXYGEN option to YES. \n
The doxygen settings can be found in the cmake/Doxygen.cmake file. \n
After enabling doxygen cmake triggers doxygen to generate the documentation as html and put it into doc/doxygen_html. \n
\n

## Testing
The btf-toolchain also contains a testing setup. It requires catch2 to work and can be enabled with the ENABLE_TESTING option. \n
For more information see the Testing subsections in [How to use the library](usage.md).
 \n
 \n
********************************************************************
# Build Commands

The btf-toolchain is compatible with Windows and Linux. 
After fulfilling the required dependencies, the building process can be performed with the following commands. \n
Please note that the last command is for the Python extension. 
It will only work if the required dependencies for the Python binding are installed and the ENABLE_PYTHON_BINDINGS option is enabled.

## Linux
1. mkdir build && cd build
2. cmake ..
3. cmake --build . --parallel 8
4. cpack
5. python3.9 ../setup.py bdist_wheel --bdist-dir ~/temp/bdistwheel

## Windows
1. Start a Developer Powershell for VS 2019/2022 (or any console which fulfills the requirements)
2. mkdir build && cd build
3. cmake ..
4. cmake --build . --parallel 8
5. python ..\setup.py bdist_wheel