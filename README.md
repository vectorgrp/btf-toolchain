# btf-toolchain

This is the repository for the btf-toolchain, which helps to create Best Trace Format (BTF) files. <br />
BTF is a trace format for timing analysis that can be imported and analysed by the [TA Tool Suite](https://www.vector.com/int/en/products/products-a-z/software/ta-tool-suite/). <br />
Possible uses for the btf-toolchain could be to import a BTF file for editing or to create a BTF file from scratch. <br />
The second use case is against the background that the TA Tool suite can only import certain formats such as BTF and VX1000 csv. <br />
For additional trace formats from non-supported operating systems, the BTF toolchain can be used to write a custom X to BTF converter. <br />
See the [Documentation](doc/doxygen_html/index.html) for more information. (If the link does not work go to index.html in the doc/doxygen_html folder) <br />
The documentation is a combination of Doxygen-generated html files and markdown files. <br />
It is recommended to open it with a browser. If no browser is available, the overview.md file in doc/md can be opened instead. <br />
If you are reading this from Github you can open a preview of the documentation via [this link](https://htmlpreview.github.com/vectorgrp/btf-toolchain/main/doc/doxyygen_html/index.html). <br />
<br />

# Content:
* libBtf: reading and writing of BTF files
* libHelper: utilities like logging
* pybtf: bingings to the C++ libraries for Python
<br />

# Dependencies:
* programmes:
	* [required] cmake
	* [required] gcc (9 or higher),  msvc (19.22 or higher) or clang (8 or higher)
	* [optional] python3
	* [optional] ccach: for build acceleration
	* [optional] doxygen and graphviz: for doxygen documentation generation
	* [optional] clang-tidy and/or cppcheck: for static source code analysis
	* [optional] include-what-you-use: for static source code analysis
	* [optional] clang-format: for source code formatting
* libraries:
	* [optional] pybind11: required for python bindings. Automatically installed
	* [optional] pybind11-stubgen
	* [optional] wheel
	* [optional] catch2: required for testing
<br />

# Build:
## Linux
1. mkdir build && cd build
2. cmake ..
3. cmake --build . --parallel 8
4. cpack
5. python3 ../setup.py bdist_wheel --bdist-dir ~/temp/bdistwheel

## Windows
1. Start a Developer Powershell for VS 2019/2022 (or any console which fulfills the requirements)
2. mkdir build && cd build
3. cmake ..
4. cmake --build
5. python ..\setup.py bdist_wheel
<br />

# Contact:
Got any questions or problems? <br />
Feel free to contact us via email (support@vector.com) or by submitting a support form on the [Vector Support Page](https://support.vector.com/gst).
