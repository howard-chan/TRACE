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

// Please refer to trace.h for the procedure reference
/********************* System Headers ************************/
#include <stdio.h>
// Step 4: Include trace.h to the source to be instrumented,
#include "trace.h"

/*********************** Constants ***************************/
#define TEST_TRACE_DEPTH  20

/********************* Local Functions ***********************/
// Step 1a: This mimics a tick which is used for optional time stamping
uint32_t fake_tick(void)
{
    static uint32_t ulTick = 0;
    return ulTick += 10;
}

/* Step 5: Use the following macros in your code to instrument execution:
 *   TRACE_FILE(<Trace Buffer Name>)
 *   TRACE_FUNC(<Trace Buffer Name>)
 *   TRACE(<Trace Buffer Name>, <message>, <value>)
 */
void test_func1(void)
{
    // Use TRACE_FUNC() to indicate the function and line number
    TRACE_FUNC(Test);
    static int cnt = 0;
    // Simulate an error condition
    if ((++cnt % 3) == 0)
    {
        // Use TRACE_FILE() to indicate the file name and line number
        TRACE_FILE(Test);
        // Use TRACE() to trace the first few errors
        TRACE(Error, "  Event @ func1", cnt);
    }
}

void test_func2(void)
{
    TRACE_FUNC(Test);
    static int cnt = 0;
    if ((++cnt % 5) == 0)
    {
        TRACE(Error, "  Event @ func2", cnt);
    }
}

void main(void)
{
    int idx;
    TRACE(Test, "Test Start", 0);
    for (idx = 0; idx < TEST_TRACE_DEPTH + 6; idx++)
    {
        test_func1();
        test_func2();
    }
    TRACE(Test, "Test End", idx);

    /* Step 6: Dump the trace using TRACE_DUMP or inspect with JTAG
     *   a) Call TRACE_DUMP(<Trace Buffer Name>, <reset>) to dump the contents of the trace buffer
     *   b) With a JTAG debugger (e.g. JLinkDebugger), inspecting the global variables and
     *      expanding them in the debugger will show human redable messages
     */
    TRACE_DUMP(Error, true);
    TRACE_DUMP(Test, true);

    // Try again with the buffers reset
    TRACE(Test, "Test Start Again", 0);
    for (idx = 0; idx < 2; idx++)
    {
        test_func1();
        test_func2();
    }
    TRACE(Test, "Test End Again", idx);
    TRACE_DUMP(Error, false);
    TRACE_DUMP(Test, false);
}
