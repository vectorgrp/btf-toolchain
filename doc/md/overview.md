Overview {#mainpage}
===========================================

This is the documentation for the FOSS btf-toolchain. 
The btf-toolchain consists of libraries for working with Best Trace Format (BTF) files. \n
BTF is a trace format for timing analysis that can be imported and analysed by the [TA Tool Suite](https://www.vector.com/int/en/products/products-a-z/software/ta-tool-suite/). \n
The btf-toolchain is based on the BTF specification version 2.2.1. \n
For more information about the  BTF format the open-access version of the BTF specification can be downloaded from the [Vector KnowledgeBase](https://support.vector.com/kb?id=kb_article_view&sysparm_article=KB0012584).\n
The libraries are written in C++, but Python bindings are also provided.

Possible uses for the btf-toolchain could be to import a BTF file for editing or to create a BTF file from scratch. \n
The second use case is against the background that the TA Tool suite can only import certain formats such as BTF and VX1000 csv. \n
For additional trace formats from non-supported operating systems, the BTF toolchain can be used to write a custom X to BTF converter. \n
An example for such a converter is provided for csv to BTF in Python. For more information go to the "Example Converter" subsection in 
@subpage usage. \n
\n


Libraries
  - **libbtf**: reading and writing of BTF files
  - **libhelper**: utilities like logging

Python Module
  - **pybtf**: bindings to the C++ libraries

\n
Further information can be found on the following pages:
  - @subpage building
  - @subpage usage
  - @subpage source
\n
\n
## Contact
Got any questions or problems? \n
Feel free to contact us via email (support@vector.com) or by submitting a support form on the [Vector Support Page](https://support.vector.com/gst).