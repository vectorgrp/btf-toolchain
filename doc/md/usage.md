How to use the library {#usage}
======================

This section describes how to use the btf-toolchain. 
An user can choose to either work with either the provided C++ API or the Python API. \n
For the Python API to work the C++ libraries must be built.
In both cases the btf-toolchain provides the necessary functions to create a syntactically correct BTF file. \n
At the end of the document there are some important notes regarding the use of the btf-toolchain.
\n
********************************************************************
# C++

This section shows the possible functionalities of the C++ libraries. 
The following Getting Started subsection shows the intended workflow when creating a BTF file. \n
Furthermore, the Logging subsection describes how the logging feature works and the Testing subsection explains where to find the test cases for the btf-toolchain.\n

## Getting started

To work with BTF files an object of type BtfFile is required. This can be created by calling the constructor of btf::BtfFile \n
(A detailed description of the constructor and its input parameters can be found in the source code documentation).

```cpp
#include "btf.h"

btf::BtfFile btfFile("test.btf");
```

Now either an existing BTF file can be read in:
```cpp
btfFile.importFromFile("input.btf");
``` 

Or events can be emitted:
```cpp
btfFile.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::start, true);
btf.runnableEvent(200, "Core1", "Runnable1", btf::Runnable::Events::start);
``` 

To write the BTF file to disc, run:
```cpp
btfFile.finish();
``` 
For more examples, see the Testing subsection.
\n
## Logging
The btf-toolchain also includes a logging library that allows you to print log message to the console or to a file.
The library supports multiple log levels:
* error
* warning
* trace

The default behavior is log level warning and printing the message to cout. This can be changed by calling initLogging:

```cpp
helper::logging::initLogging(helper::logging::LogLevel::trace, "log.txt");
```

This tells the libraries to print all possible messages and write them to the log.txt file. If the string to the file is empty, the messages are printed to cout.

A log message can be written with:

```cpp
helper::logging::printTrace() << "This is a trace message" << '\n';
helper::logging::printWarning() << "This is a warning message" << '\n';
helper::logging::printError() << "This is an error message" << '\n';
```

To write the log data into the file the logger must be flushed:

```cpp
helper::logging::flush();
```
\n
## Testing
The btf-toolchain contains a test setup created using the catch2 framework. To enable the testing framework, activate the ENABLE_TESTING option. \n 
The test cases check the basic functionality of the btf-toolchain. They can be found in test/libBtfTest.cpp. \n
In total, the file contains 22 test cases with different example use cases of the btf library and one test case for the logging feature. \n
To run the tests, execute test.exe in /build/test/Debug/ after building the project with cmake.
\n
\n
********************************************************************
# Python

The C++ APIs are also exposed to Python and can be used in a Python script.
Only the pybtf module, which is the link to the C++ library needs, to be imported.
\n
## Getting started

To work with BTF files an object of type BtfFile is required. This can be created by calling the C++ constructor of BtfFile via the Python binding.

```py
import pybtf

btfFile = pybtf.pybtf.BtfFile("test.btf", pybtf.pybtf.BtfFile.Timescale.nano_seconds, True, True, False, False)
```

The meaning of the three boolean parameters is explained in the Important Notes section.
Next, either an existing BTF file can be imported:
```py
btfFile.importFromFile("input.btf")
``` 

Or events can be emitted:
```py
btfFile.processEvent(100, "Core1", "Task1", 0, pybtf.pybtf.ProcessEvent.start, True)
btfFile.runnableEvent(200, "Core1", "Runnable1", pybtf.pybtf.RunnableEvent.start)
``` 

To write the BTF file to disc, run:
```py
btfFile.finish()
``` 
\n
## Logging
The logging library can also be used in Python using the Python binding. \n
Here, the default behavior is also log level warning and printing the message to cout.
This can be changed by calling initLogging:

```py
import pybtf
pybtf.pybtf.helper.initLogging(pybtf.pybtf.helper.LogLevel.trace, "log.txt")
```

A log message can be written with:
```py
pybtf.pybtf.helper.Log(pybtf.pybtf.helper.LogLevel.trace, "This is a trace message")
pybtf.pybtf.helper.Log(pybtf.pybtf.helper.LogLevel.warning, "This is a warning message")
pybtf.pybtf.helper.Log(pybtf.pybtf.helper.LogLevel.error, "This is an error message")
```

To write log data to the file, the logger must be flushed:
```py
pybtf.pybtf.helper.flushLog()
```
\n
## Testing
For the Python version of the btf-toolchain there is also a test framework based on the unittest module. \n
The test file contains the same tests as the C++ version and is located in pytest/test.py. \n
To execute the test framework, open the pytest folder in a command prompt and execute test.py:

```py
python test.py
```

## Example Converter
In addition to the test framework, an example for a csv to BTF converter in Python is also provided. \n
The example converter is located in pytest/csv_example.py and can convert the file trace.csv to a btf file. \n
trace.csv is a short system flow trace created with [Lauterbach](https://www.lauterbach.com/).
Please note that this script is just a simple example of how the btf-toolchain can be used and does not cover every csv to BTF use case.\n
However, it can be used as a template to get started.
\n
\n
********************************************************************
# Important Notes

This section contains further information on best practices, limitations and general usage of the btf-toolchain.
\n
## Input Parameters
In general, the input parameters of the functions and their description can be found in the source code documentation. \n
Here, two functions in particular are highlighted: the BtfFile() constructor and processEvent().\n
The BtfFile constructor has a total of six input parameters, five of which have a default value. \n
Four of these parameters are booleans that affect the behaviour of the btf-toolchain and should be configured as needed. \n
The effect of these parameters is documented in the source code documentation of the BtfFile class and in the testframework 
are test cases that show the effect of different parameter configurations.\n
The processEvent() function also has an extra boolean parameter with a default value that indicates whether the process event is a Task or an ISR. The default is a Task. \n

## When to start the BTF trace
In the BTF format, the Stimulus, Runnable, Scheduler, OS and Signal events have one or more event types that require a running process as the event source. \n
Therefore, the btf-toolchain operates under the rule that a process event that allocates the process to a core must have occurred before any of the above events. \n
This means that if the trace is started at any point during runtime and, for example, the process start event is missing, the btf-toolchain will report either \n
a "no task running" or "source task not running" error if one of the above events is triggered with that process (or core on which the process should run) as the source.\n
An exception to this rule is made for runnable events if a process event of the source process occurs in the trace later on. \n
In this case, there is a fallback mechanism that collects all runnables without a task and then assigns them to the correct task. \n
This particular use case is also illustrated in the "Start of trace during runnable execution" test case.\n

## Limitations
The btf-toolchain is based on the BTF Technical Specifiaction Version 2.2.1. However, not all features described in the specification could be implemented. \n
In the following, the limitations of the btf-toolchain are listed.

Event Entity | Event Type | Limitation
-------------|------------|-----------
Stimulus     | trigger    | Although the specification allows for an external or inter-process source,\n the btf-toolchain only supports an external source.
OS Event     | set_event  | The specification allows a process or a stimulus as a source.\n However, the btf-toolchain only supports a process or core as a source.