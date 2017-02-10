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

/********************* System Headers ************************/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/********************* Local Headers *************************/
#include "trace.h"

#if TRACE_ENABLE
/*********************** Macros ******************************/
#undef  TRACE_CONFIG
// NOTE: This macro creates the trace buffer objects in memory
#define TRACE_CONFIG(trcName, depth, isWrap)    trace_line_t gaxTrace##trcName##Line[depth]; \
                                                trace_t gxTrace##trcName = { \
                                                    .usDepth = depth, \
                                                    .bIsWrap = isWrap, \
                                                    .usIdx = 0, \
                                                    .ulCount = 0, \
                                                    .axLine = gaxTrace##trcName##Line, \
                                                    .name = #trcName, \
                                                }; \
                                                trace_t *gpxTrace##trcName = &gxTrace##trcName;

/********************* Configuration *************************/
#ifdef TRACE_USE_CONFIG_FILE
#include TRACE_USE_CONFIG_FILE
#endif // TRACE_USE_CONFIG

/********************* Global Variables **********************/
/********************* Exported Functions ********************/
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
    // Step 1: Lock the resource
    TRACE_LOCK();
    // Step 2: Always count the number of trace calls
    This->ulCount++;
    // Only store if there is room in the buffer (i.e. no-wrap case)
    if (This->usIdx < This->usDepth)
    {
        // Step 3.1: get the current index and increment
        uint16_t usIdx = This->usIdx;
        This->usIdx++;
        // If Trace Wrap is enabled, then reset index
        if (This->bIsWrap && This->usIdx >= This->usDepth)
        {
            This->usIdx = 0;
        }
		// Step 3.2: Unlock the resource
        TRACE_UNLOCK();
        // Step 3.3: Store the trace line.  Assume there is enough time to write at slot before wrap
#if defined(TRACE_GET_TICK)
        This->axLine[usIdx].ulTimeStamp = TRACE_GET_TICK();  // Tick [in ms]
#endif // defined(TRACE_GET_TICK)
        This->axLine[usIdx].pcMessage = pcMessage;
        This->axLine[usIdx].ulValue = ulValue;
    }
	else
	{
		// Step 3: Unlock the resource
        TRACE_UNLOCK();
	}
}

#ifdef TRACE_OUTPUT
void trace_dump(trace_t *This, bool bReset)
{
    uint16_t usIdx, usIdy, usOffset;
#if defined(TRACE_GET_TICK)
    uint32_t ulLastStamp = 0;
#endif // defined(TRACE_GET_TICK)

    TRACE_DUMP_LOCK();

    usOffset = (This->bIsWrap) ? This->usIdx : 0;
    TRACE_OUTPUT(TRACE_NEWLINE "====TRACE[%s] (total:%d, depth:%d)====  " TRACE_NEWLINE, This->name, This->ulCount, This->usDepth);
    // Output the trace buffer
    for (usIdx = 0; usIdx < This->usDepth; usIdx++)
    {
        usIdy = (usOffset + usIdx) % This->usDepth;
        // Only print valid messages
        if (This->axLine[usIdy].pcMessage && usIdy < This->ulCount)
        {
            uint32_t ulValue = This->axLine[usIdy].ulValue;
#if defined(TRACE_GET_TICK)
            uint32_t ulStamp = This->axLine[usIdy].ulTimeStamp;
            TRACE_OUTPUT("  %6d|%10d ms| %s: 0x%08x (%d)" TRACE_NEWLINE, usIdy, ulStamp - ulLastStamp, This->axLine[usIdy].pcMessage, ulValue, ulValue);
            ulLastStamp = ulStamp;
#else
            TRACE_OUTPUT("  %6d| %s: 0x%08x (%d)" TRACE_NEWLINE, usIdy, This->axLine[usIdy].pcMessage, ulValue, ulValue);
#endif // defined(TRACE_GET_TICK)
        }
    }
    // Reset the trace buffer for a new capture
    if (bReset)
    {
        This->usIdx = 0;
        This->ulCount = 0;
    }

    TRACE_DUMP_UNLOCK();
}
#else
void trace_dump(trace_t *This, bool bReset) { This = This; bReset = bReset; }
#endif // TRACE_OUTPUT
#endif // TRACE_ENABLE