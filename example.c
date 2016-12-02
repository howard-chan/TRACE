/*
The MIT License (MIT)

Copyright (c) 2015-2017 Howard Chan
https://github.com/howard-chan/TRACE

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <stdio.h>
#include "trace.h"

#define TEST_TRACE_DEPTH    20

// Step 1: Define the trace buffer and trace object
trace_line_t axTestBuffer[TEST_TRACE_DEPTH];
trace_t xTestTrace;

void test_func1(void)
{
    static int cnt = 0;
    TRACE_AT(&xTestTrace);
    if (++cnt % 3 == 0)
    {
        TRACE_ERROR("Error occurred in func1 @", cnt);
    }
    TRACE_AT(&xTestTrace);
}

void test_func2(void)
{
    static int cnt = 0;
    if (++cnt % 5 == 0)
    {
        TRACE_ERROR("Error occurred in func2 @", cnt);
    }
}

void main(void)
{
    // Step 2: If the TRACE_ERROR_DEPTH is defined, then you must call the
    // following before you begin tracing
    TRACE_ERROR_INIT();

    // Step 3: Initialize the trace before use, this trace buffer will wrap
    trace_init(&xTestTrace, "test", axTestBuffer, TEST_TRACE_DEPTH, true);

    // Step 4: Instrument your code with TRACE_ERROR(...) or trace(...)
    int idx;
    for (idx = 0; idx < TEST_TRACE_DEPTH +6; idx++)
    {
        trace(&xTestTrace, "Calling func1:", idx);
        test_func1();
        trace(&xTestTrace, "Calling func2:", idx);
        test_func2();
    }
    // Step 5a: Call the following to dump the trace to your logger
    TRACE_ERROR_DUMP();
    trace_dump(&xTestTrace);
}