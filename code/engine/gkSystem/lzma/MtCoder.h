//////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2011-2015 Kaiming Yi
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
	
*/
//////////////////////////////////////////////////////////////////////////



/* MtCoder.h -- Multi-thread Coder
2009-11-19 : Igor Pavlov : Public domain */

#ifndef __MT_CODER_H
#define __MT_CODER_H

#include "Threads.h"

EXTERN_C_BEGIN

typedef struct
{
  CThread thread;
  CAutoResetEvent startEvent;
  CAutoResetEvent finishedEvent;
  int stop;
  
  THREAD_FUNC_TYPE func;
  LPVOID param;
  THREAD_FUNC_RET_TYPE res;
} CLoopThread;

void LoopThread_Construct(CLoopThread *p);
void LoopThread_Close(CLoopThread *p);
WRes LoopThread_Create(CLoopThread *p);
WRes LoopThread_StopAndWait(CLoopThread *p);
WRes LoopThread_StartSubThread(CLoopThread *p);
WRes LoopThread_WaitSubThread(CLoopThread *p);

#ifndef _7ZIP_ST
#define NUM_MT_CODER_THREADS_MAX 32
#else
#define NUM_MT_CODER_THREADS_MAX 1
#endif

typedef struct
{
  UInt64 totalInSize;
  UInt64 totalOutSize;
  ICompressProgress *progress;
  SRes res;
  CCriticalSection cs;
  UInt64 inSizes[NUM_MT_CODER_THREADS_MAX];
  UInt64 outSizes[NUM_MT_CODER_THREADS_MAX];
} CMtProgress;

SRes MtProgress_Set(CMtProgress *p, unsigned index, UInt64 inSize, UInt64 outSize);

struct _CMtCoder;

typedef struct
{
  struct _CMtCoder *mtCoder;
  Byte *outBuf;
  size_t outBufSize;
  Byte *inBuf;
  size_t inBufSize;
  unsigned index;
  CLoopThread thread;

  Bool stopReading;
  Bool stopWriting;
  CAutoResetEvent canRead;
  CAutoResetEvent canWrite;
} CMtThread;

typedef struct
{
  SRes (*Code)(void *p, unsigned index, Byte *dest, size_t *destSize,
      const Byte *src, size_t srcSize, int finished);
} IMtCoderCallback;

typedef struct _CMtCoder
{
  size_t blockSize;
  size_t destBlockSize;
  unsigned numThreads;
  
  ISeqInStream *inStream;
  ISeqOutStream *outStream;
  ICompressProgress *progress;
  ISzAlloc *alloc;

  IMtCoderCallback *mtCallback;
  CCriticalSection cs;
  SRes res;

  CMtProgress mtProgress;
  CMtThread threads[NUM_MT_CODER_THREADS_MAX];
} CMtCoder;

void MtCoder_Construct(CMtCoder* p);
void MtCoder_Destruct(CMtCoder* p);
SRes MtCoder_Code(CMtCoder *p);

EXTERN_C_END

#endif
