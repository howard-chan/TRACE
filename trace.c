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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "trace.h"

//---- External Globals----
#if TRACE_ERROR_DEPTH
trace_t gxTraceError;
trace_line_t gaxTraceLine[TRACE_ERROR_DEPTH];
#endif // TRACE_ERROR_DEPTH

void trace_init(trace_t *This, char *name, trace_line_t *pxLine, uint16_t usDepth, bool bIsWrap)
{
    This->usDepth = usDepth;
    This->bIsWrap = bIsWrap;
    This->usIdx = 0;
    This->ulCount = 0;
    This->axLine = pxLine;
    This->name = name;
}

void trace(trace_t *This, char *pcMessage, uint32_t ulValue)
{
    TRACE_LOCK();

    // Only store if there is room in the buffer
    if (This->usIdx < This->usDepth)
    {
        This->axLine[This->usIdx].pcMessage = pcMessage;
        This->axLine[This->usIdx].ulValue = ulValue;
        This->usIdx++;
        // If Trace Wrap is enabled, then reset index
        if (This->bIsWrap && This->usIdx >= This->usDepth)
        {
            This->usIdx = 0;
        }
    }
    // Always count the number of trace calls
    This->ulCount++;

    TRACE_UNLOCK();
}

#ifdef TRACE_OUTPUT
void trace_dump(trace_t *This)
{
    uint16_t usIdx, usIdy, usOffset;

    TRACE_LOCK();

    usOffset = (This->bIsWrap) ? This->usIdx : 0;
    TRACE_OUTPUT(TRACE_NEWLINE "====TRACE %s(total:%d, depth:%d)====  " TRACE_NEWLINE, This->name, This->ulCount, This->usDepth);
    // Output the trace buffer
    for (usIdx = 0; usIdx < This->usDepth; usIdx++)
    {
        usIdy = (usOffset + usIdx) % This->usDepth;
        // Only print valid messages
        if (This->axLine[usIdy].pcMessage)
        {
            uint32_t ulValue = This->axLine[usIdy].ulValue;
            TRACE_OUTPUT("  %6d| %s 0x%08x (%d)" TRACE_NEWLINE, usIdy, This->axLine[usIdy].pcMessage, ulValue, ulValue);
        }
    }

    TRACE_UNLOCK();
}
#else
void trace_dump(trace_t *This) { This = This; }
#endif // TRACE_OUTPUT
