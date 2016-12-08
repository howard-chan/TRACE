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
#ifndef __TRACE_H__
#define __TRACE_H__

#ifdef __cplusplus
extern "C" {
#endif

/********************* System Headers ************************/
#include <stdint.h>
#include <stdbool.h>

/********************* Platform Headers *************************/
// Add platform specific types here
//----Trace Configuration [BEGIN]----
// Sets the newline for host (i.e. linux - "\n", windows - "\r\n")
#define TRACE_NEWLINE           "\n"

// [OPTIONAL] Set TRACE_ERROR_DEPTH to non-zero to enable a default "error" trace
#define TRACE_ERROR_DEPTH       8
// [OPTIONAL] Set TRACE_ERROR_WRAP to wrap when buffer is full
#define TRACE_ERROR_WRAP        false

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

// [OPTIONAL] Define TRACE_GET_TICK to enable time stamping
// This section maybe customized for platform specific debug facilities
// #include "your_embedded_GET_TICK_here.h"
extern uint32_t fake_tick(void);
#define TRACE_GET_TICK          fake_tick

//----Trace Configuration [END]----

/*********************** Macros ******************************/
//----Trace Macros----
// Convenience macro to instrument code
#define TRACE_FILE(pxTrace)                     trace(pxTrace, "--"__FILE__ " @ line", __LINE__)
#define TRACE_FUNC(pxTrace)                     trace(pxTrace, (char *)__func__, __LINE__)
// [OPTIONAL] This is a convenience macro for logging errors
#if TRACE_ERROR_DEPTH
    // Add this to the code to instrument
    #define TRACE_ERROR(pcMessage, ulValue)     trace(&gxTraceError, pcMessage, ulValue)
    // Add this at the very beginning to initialization
    #define TRACE_ERROR_INIT()                  trace_init(&gxTraceError, "ERROR", gaxTraceErrorLine, TRACE_ERROR_DEPTH, TRACE_ERROR_WRAP)
    #define TRACE_ERROR_DUMP()                  trace_dump(&gxTraceError)
#else
    #define TRACE_ERROR(pcMessage, ulValue)
    #define TRACE_ERROR_INIT()
    #define TRACE_ERROR_DUMP()
#endif // TRACE_ERROR_DEPTH

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
// Default Error trace
extern trace_t gxTraceError;
extern trace_line_t gaxTraceErrorLine[TRACE_ERROR_DEPTH];

//----Function Declarations----
/********************* Exported Functions ********************/
/**
 * @brief      Initialize the TRACE object
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
 * @param      This  Pointer to the TRACE object
 *
 * @note       This feature is only available if TRACE_OUTPUT is defined
 */
void trace_dump(trace_t *This);

#ifdef __cplusplus
}
#endif

#endif // __TRACE_H__
