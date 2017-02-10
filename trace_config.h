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

/* Step 2: Instantiate the trace buffer using the provided macros
 *   a) Create a custom config file (e.g. "trace_config.h")
 *   b) For each trace buffer, define the trace with the following macro:
 *     TRACE_CONFIG(<Trace Buffer Name>, <Depth of Trace Buffer>, <Wrap Mode>)
 *     e.g.: To create an "Error" trace buffer that can store 8 traces with no wrap
 *       TRACE_CONFIG(Error, 8, false)
 *     e.g.: To create an "Test" trace buffer that can store 20 traces with wrap
 *       TRACE_CONFIG(Test, 20, true)
 */
TRACE_CONFIG(Error, 8, false)
TRACE_CONFIG(Test, 20, true)
