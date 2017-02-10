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
/*
 * This is a lightweight TRACE buffer utility designed to instrument an embedded
 * system like a printf, but stores the values to a buffer which can be dumped
 * to a configured stdout or inspected directly with a JTAG.
 *
 * The following is the procedure to enable an application with TRACE feature
 * Step 1: Configure the values in the Trace Configuration section below
 *
 * Step 2: Instantiate the trace buffer using the provided macros
 *   a) Create a custom config file (e.g. "trace_config.h")
 *   b) For each trace buffer, define the trace with the following macro:
 *     TRACE_CONFIG(<Trace Buffer Name>, <Header>, <Depth of Trace Buffer>, <Wrap Mode>)
 *     e.g.: To create an "Error" trace buffer that can store 8 traces with no wrap
 *       TRACE_CONFIG(Error, "Error Trace", 8, false)
 *     e.g.: To create an "Test" trace buffer that can store 20 traces with wrap
 *       TRACE_CONFIG(Test, "Test Trace", 20, true)
 *
 * Step 3: Define TRACE_USE_CONFIG_FILE with the file in step 2 in the section below
 *     e.g.: #define TRACE_USE_CONFIG_FILE "trace_config.h"
 *
 * Step 4: Include trace.h to the source to be instrumented,
 *     i.e.: #include "trace.h"
 *
 * Step 5: Use the following macros in your code to instrument execution:
 *   TRACE_FILE(<Trace Buffer Name>)
 *   TRACE_FUNC(<Trace Buffer Name>)
 *   TRACE(<Trace Buffer Name>, <message>, <value>)
 *
 * Step 6: Dump the trace using TRACE_DUMP or inspect with JTAG
 *   a) Call TRACE_DUMP(<Trace Buffer Name>, <reset>) to dump the contents of the trace buffer
 *   b) With a JTAG debugger (e.g. JLinkDebugger), inspecting the global variables and
 *      expanding them in the debugger will show human redable messages
 */
#ifndef __TRACE_H__
#define __TRACE_H__

#ifdef __cplusplus
extern "C" {
#endif

/********************* System Headers ************************/
#include <stdint.h>
#include <stdbool.h>

/********************* Platform Configuration ****************/
// Enable Trace
#define TRACE_ENABLE            1
// Add platform specific types here
//----Step 1: Trace Configuration [BEGIN]----
// Sets the newline for host (i.e. linux - "\n", windows - "\r\n")
#define TRACE_NEWLINE           "\n"

// [OPTIONAL] Define the following OS specific macros for thread safe operation
// #include "your_os_mutex_or_semaphore_here.h"
#define TRACE_LOCK()
#define TRACE_UNLOCK()
#define TRACE_DUMP_LOCK()
#define TRACE_DUMP_UNLOCK()

// [OPTIONAL] Define TRACE_OUTPUT to enable trace_dump
// This section maybe customized for platform specific debug facilities
// #include "your_embedded_DEBUG_OUT_here.h"
#include <stdio.h>
#define TRACE_OUTPUT(...)       printf(__VA_ARGS__)

// [OPTIONAL] Step 1a: Define TRACE_GET_TICK to enable time stamping
// This section maybe customized for platform specific debug facilities
// #include "your_embedded_GET_TICK_here.h"
extern uint32_t fake_tick(void);
#define TRACE_GET_TICK          fake_tick

// [OPTIONAL] Step 3: Use a customized "trace_config.h" file to define trace buffers
#define TRACE_USE_CONFIG_FILE   "trace_config.h"
//----Trace Configuration [END]----

/*********************** Macros ******************************/
#if TRACE_ENABLE

/**
 * @brief      Configures the trace buffer
 *
 * @param      trcName  Name of Trace object
 * @param      hdrName  Banner shown in trace dump
 * @param      depth    Depth of the trace buffer
 * @param      isWrap   true - wrap when full, false - stop tracing when full
 */
#define TRACE_CONFIG(trcName, hdrName, depth, isWrap) \
    extern trace_line_t gaxTrace##trcName##Line[depth]; \
    extern trace_t gxTrace##trcName; \
    extern trace_t *gpxTrace##trcName;

/**
 * @brief      Traces the executedline in a file
 *
 * @param      trcName  Name of Trace object
 */
#define TRACE_FILE(trcName)                     trace(gpxTrace##trcName, "--"__FILE__ " @ line", __LINE__)

/**
 * @brief      Traces the executed line in a function
 *
 * @param      trcName  Name of Trace object
 */
#define TRACE_FUNC(trcName)                     trace(gpxTrace##trcName, (char *)__func__, __LINE__)

/**
 * @brief      Adds generic message string and value to trace buffer
 *
 * @param      trcName    Name of Trace object
 * @param      pcMessage  Message string
 * @param      ulValue    value to trace
 */
#define TRACE(trcName, pcMessage, ulValue)      trace(gpxTrace##trcName, pcMessage, ulValue)

/**
 * @brief      Dump the trace buffer to the configured output
 *
 * @param      trcName  Name of Trace object
 * @param      reset    true - reset the buffer, otherwise false
 */
#define TRACE_DUMP(trcName, reset)              trace_dump(gpxTrace##trcName, reset)
#else
#define TRACE_CONFIG(trcName, hdrName, depth, isWrap)
#define TRACE_FILE(trcName)
#define TRACE_FUNC(trcName)
#define TRACE(trcName, pcMessage, ulValue)
#define TRACE_DUMP(trcName, reset)
#endif
/*********************** Typedefs ****************************/
// The trace line holds a single trace sample
typedef struct
{
#if defined(TRACE_GET_TICK)
    uint32_t ulTimeStamp;           // TimeStamp
#endif // defined(TRACE_GET_TICK)
    char * pcMessage;               // Pointer to Trace Message string
    uint32_t ulValue;               // Optional value
} trace_line_t;

typedef struct
{
    uint16_t usDepth : 15;          // The number of trace lines
    uint16_t bIsWrap : 1;           // true - wrap when full, false - stop tracing when full
    uint16_t usIdx;                 // Index to next trace line to write
    uint32_t ulCount;               // Total number of trace
    trace_line_t *axLine;           // Pointer to array of trace lines
    char *   name;                  // Name of trace
} trace_t;

/********************* Extern Variables **********************/
#ifdef TRACE_USE_CONFIG_FILE
#include TRACE_USE_CONFIG_FILE
#endif // TRACE_USE_CONFIG_FILE

/********************* Exported Functions ********************/
/**
 * @brief      Initialize or configure the TRACE object
 *
 * @param      This     Pointer to TRACE object
 * @param      name     Name of trace
 * @param      pxLine   Pointer to array of trace line (i.e. trace buffer)
 * @param[in]  usDepth  Depth of the trace buffer
 * @param[in]  bIsWrap  true - wrap when full, false - stop tracing when full
 */
void trace_init(trace_t *This, char *name, trace_line_t *pxLine, uint16_t usDepth, bool bIsWrap);

/**
 * @brief      Trace the code by adding to the trace buffer
 *
 * @param      This       Pointer to the TRACE object
 * @param      pcMessage  Null-terminated message string
 * @param[in]  ulValue    [OPTIONAL] Value to store
 */
void trace(trace_t *This, char *pcMessage, uint32_t ulValue);

/**
 * @brief      Dump the contents of the trace buffer in order of oldest to
 *             newest.
 *
 * @param      This    Pointer to the TRACE object
 * @param[in]  bReset  true - reset trace, false - leave as is
 *
 * @note       This feature is only available if TRACE_OUTPUT is defined
 */
void trace_dump(trace_t *This, bool bReset);

#ifdef __cplusplus
}
#endif

#endif // __TRACE_H__
