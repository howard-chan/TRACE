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

// Step 0: This mimics a tick which is used for optional time stamping
uint32_t fake_tick(void)
{
    static uint32_t ulTick = 0;
    return ulTick += 10;
}

// Step 1: Define the trace buffer and trace object
trace_line_t axTestBuffer[TEST_TRACE_DEPTH];
trace_t xTestTrace;

void test_func1(void)
{
    // Use TRACE_FUNC() to indicate the function and line number
    TRACE_FUNC(&xTestTrace);
    static int cnt = 0;
    // Simulate an error condition
    if ((++cnt % 3) == 0)
    {
        // Use TRACE_FILE() to indicate the file name and line number
        TRACE_FILE(&gxTraceError);
        // Use TRACE_ERROR() to trace the first few errors
        TRACE_ERROR("  Event @ func1", cnt);
    }
}

void test_func2(void)
{
    TRACE_FUNC(&xTestTrace);
    static int cnt = 0;
    if ((++cnt % 5) == 0)
    {
        TRACE_ERROR("  Event @ func2", cnt);
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
    trace(&xTestTrace, "Test Start", 0);
    for (idx = 0; idx < TEST_TRACE_DEPTH +6; idx++)
    {
        test_func1();
        test_func2();
    }
    trace(&xTestTrace, "Test End", idx);
    // Step 5a: Call the following to dump the trace to your logger
    TRACE_ERROR_DUMP();
    trace_dump(&xTestTrace);
}
